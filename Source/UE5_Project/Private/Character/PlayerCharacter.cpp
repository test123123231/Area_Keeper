#include "Character/PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "Character/ItemBase.h"
#include "Character/QuickSlot.h"
#include "DrawDebugHelpers.h"


APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 300.f;
	SpringArm->bUsePawnControlRotation = true;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(SpringArm);

	CurrentFocusedItem = nullptr;
	HeldItem = nullptr;

}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ¸Å ÇÁ·¹ÀÓ ¾ÆÀÌÅÛ Ã£±â
	TraceForItems();
}


void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// PlayerInputComponentë¥¼ í–¥ìƒëœ ì…ë ¥ ì»´í¬ë„ŒíŠ¸ë¡œ ìºìŠ¤íŒ…
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// ì…ë ¥ ì•¡ì…˜ê³¼ ì²˜ë¦¬ í•¨ìˆ˜ë¥¼ ë°”ì¸ë”©
		if (MoveAction) {
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		}
		if (LookAction) {
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
		}
		if (InteractAction) {
			EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &APlayerCharacter::Interact);
		}
		if (DropAction) {
			EnhancedInputComponent->BindAction(DropAction, ETriggerEvent::Started, this, &APlayerCharacter::DropHeldItem);
		}
	}
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();


}


void APlayerCharacter::Move(const FInputActionValue& Value)
{
	// ì…ë ¥ê°’(Vector2D)ì„ ê°€ì ¸ì˜´
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// ì»¨íŠ¸ë¡¤ëŸ¬ì˜ íšŒì „ ë°©í–¥ì„ ê¸°ì¤€ìœ¼ë¡œ ì „í›„/ì¢Œìš° ë°©í–¥ì„ ì°¾ê¸°
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// í•´ë‹¹ ë°©í–¥ìœ¼ë¡œ ì´ë™ ì…ë ¥ì„ ì¶”ê°€
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}


void APlayerCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// ë§ˆìš°ìŠ¤ ì…ë ¥ì— ë”°ë¼ ì»¨íŠ¸ë¡¤ëŸ¬ì˜ Yaw, Pitch ê°’ì„ ì¡°ì ˆ
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void APlayerCharacter::SetQuickSlotRef(UQuickSlot* NewRef)
{
	QuickSlotRef = NewRef;
}

void APlayerCharacter::TraceForItems()
{
	FVector Start = ViewCamera->GetComponentLocation();
	FVector ForwardVector = ViewCamera->GetForwardVector();
	FVector End = Start + (ForwardVector * 500.0f); // Ä«¸Ş¶ó ¾Õ 5m

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	// ¼Õ¿¡ µç ¾ÆÀÌÅÛÀº ¹«½Ã
	if (HeldItem && HeldItem->IsAttachedTo(this))
	{
		Params.AddIgnoredActor(HeldItem);
	}

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult, Start, End, ECC_Visibility, Params);

	AItemBase* HitItem = nullptr;
	if (bHit)
	{
		HitItem = Cast<AItemBase>(HitResult.GetActor());
	}

	// ÀÌÀü ¾ÆÀÌÅÛ ÇÏÀÌ¶óÀÌÆ® ÇØÁ¦
	if (CurrentFocusedItem && CurrentFocusedItem != HitItem)
	{
		CurrentFocusedItem->HighlightItem(false);
		CurrentFocusedItem = nullptr;
	}

	// »õ·Î¿î ¾ÆÀÌÅÛ ÇÏÀÌ¶óÀÌÆ®
	if (HitItem && HitItem != CurrentFocusedItem)
	{
		HitItem->HighlightItem(true);
		CurrentFocusedItem = HitItem;
	}

}

// ¾ÆÀÌÅÛ Áİ±â
void APlayerCharacter::PickupItem(AItemBase* Item)
{
	if (!Item || !IsValid(Item)) { UE_LOG(LogTemp, Error, TEXT("Pickup FAILED: Item invalid")); return; }
	if (!GetMesh()) { UE_LOG(LogTemp, Error, TEXT("Pickup FAILED: No mesh")); return; }
	if (!GetMesh()->DoesSocketExist(HandSocketName)) { UE_LOG(LogTemp, Error, TEXT("Pickup FAILED: no socket")); return; }

	// safety²ô±â
	if (Item->ItemMesh)
	{
		Item->ItemMesh->SetSimulatePhysics(false);
		Item->ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	bool bAttached = Item->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), HandSocketName);
	if (bAttached)
	{
		// °­Á¦ transform ¸ÂÃã
		FTransform SocketTF = GetMesh()->GetSocketTransform(HandSocketName, RTS_World);
		Item->SetActorTransform(SocketTF);
	}

	UE_LOG(LogTemp, Warning, TEXT("Pickup: %s attach=%d parent=%s loc=%s"),
		*Item->GetName(), bAttached, *GetNameSafe(Item->GetAttachParentActor()), *Item->GetActorLocation().ToString());

	Item->OnPickedUp();
}


// ¾ÆÀÌÅÛ ³»·Á³õ±â
void APlayerCharacter::ChangeItem(AItemBase* Item, const FVector& Location)
{
	if (!Item || !IsValid(Item)) return;

	UE_LOG(LogTemp, Warning, TEXT("Drop: %s BeforeDetach Parent=%s Phys=%d"),
		*Item->GetName(), *GetNameSafe(Item->GetAttachParentActor()), Item->ItemMesh ? Item->ItemMesh->IsSimulatingPhysics() : 0);

	Item->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	Item->SetActorLocation(Location);
	Item->OnDropped();

	UE_LOG(LogTemp, Warning, TEXT("Drop: %s AfterDetach Parent=%s Loc=%s"),
		*Item->GetName(), *GetNameSafe(Item->GetAttachParentActor()), *Item->GetActorLocation().ToString());
}


// Interact (¹æ¾îÀû)
void APlayerCharacter::Interact()
{
	if (!CurrentFocusedItem) return;

	AItemBase* NewItem = CurrentFocusedItem;
	if (!IsValid(NewItem)) return;

	// ¸¸¾à ¼Õ¿¡ ´Ù¸¥ ¾ÆÀÌÅÛ ÀÖÀ¸¸é ¸ÕÀú ³õ±â
	if (HeldItem && HeldItem != NewItem)
	{
		FVector DropLocation = NewItem->GetActorLocation();
		DropLocation.Z += 50.f;
		ChangeItem(HeldItem, DropLocation);
		UE_LOG(LogTemp, Warning, TEXT("Interact: Dropped %s"), *HeldItem->GetName());
		HeldItem = nullptr;
	}

	if (NewItem->ItemMesh)
	{
		NewItem->ItemMesh->SetSimulatePhysics(false);
		NewItem->ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	PickupItem(NewItem);
	if (IsValid(NewItem))
	{
		HeldItem = NewItem;
		NewItem->HighlightItem(false);
		CurrentFocusedItem = nullptr;
		// Äü½½·Ô¿¡ µî·Ï
		if (QuickSlotRef)
		{
			QuickSlotRef->AddItemToEmptySlot(NewItem);
		}
		UE_LOG(LogTemp, Warning, TEXT("Interact: Picked up %s"), *NewItem->GetName());
	}
}

void APlayerCharacter::DropHeldItem()
{
	if (HeldItem) // ¼Õ¿¡ ¾ÆÀÌÅÛÀÌ ÀÖÀ» ¶§¸¸ ½ÇÇà
	{
		FVector DropLocation = GetActorLocation() + GetActorForwardVector() * 50.f;
		DropLocation.Z += 30.f; // ¹Ù´Ú¿¡ ¹¯È÷Áö ¾Êµµ·Ï »ìÂ¦ ¿Ã¸²

		ChangeItem(HeldItem, DropLocation);
		UE_LOG(LogTemp, Warning, TEXT("DropHeldItem: Dropped %s"), *HeldItem->GetName());

		HeldItem = nullptr;
	}
	else
	{
		// ¼Õ¿¡ ¾Æ¹«°Íµµ ¾øÀ¸¸é ¾Æ¹« ÀÏµµ ¾È ÇÔ
		UE_LOG(LogTemp, Warning, TEXT("DropHeldItem: No item in hand"));
	}
}

void APlayerCharacter::SelectQuickSlot(int32 SlotIndex)
{
	if (!QuickSlotRef) return;

	// ÇöÀç ¼±ÅÃµÈ ½½·Ô º¯°æ
	QuickSlotRef->SetCurrentSlot(SlotIndex);
	

	// ½½·ÔÀÇ ¾ÆÀÌÅÛ °¡Á®¿À±â
	AItemBase* ItemToEquip = QuickSlotRef->GetItemAt(SlotIndex);

	// ½½·ÔÀÌ ºñ¾îÀÖ´Ù¸é ¼Õ¿¡ µç ¾ÆÀÌÅÛ ³»·Á³õ±â
	if (!ItemToEquip)
	{
		if (HeldItem)
		{
			DropHeldItem();
			UE_LOG(LogTemp, Warning, TEXT("Slot %d is empty. Dropped held item."), SlotIndex);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Slot %d is empty, nothing equipped."), SlotIndex);
		}
		return;
	}

	// ÀÌ¹Ì °°Àº ¾ÆÀÌÅÛÀ» µé°í ÀÖ´Ù¸é ¾Æ¹« º¯È­ ¾øÀ½
	if (HeldItem == ItemToEquip)
	{
		UE_LOG(LogTemp, Warning, TEXT("Slot %d already equipped."), SlotIndex);
		return;
	}

	// ±âÁ¸ ¾ÆÀÌÅÛ ³»·Á³õ±â
	if (HeldItem)
	{
		DropHeldItem();
	}

	// ¼±ÅÃµÈ ¾ÆÀÌÅÛ¸¸ ¸Ê¿¡¼­ º¸ÀÌ°Ô
	ItemToEquip->SetActorHiddenInGame(false);

	// »õ ¾ÆÀÌÅÛ ¼Õ¿¡ Áã±â
	PickupItem(ItemToEquip);
	HeldItem = ItemToEquip;

	UE_LOG(LogTemp, Warning, TEXT("Equipped item from slot %d: %s"), SlotIndex, *ItemToEquip->GetName());
}