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

	// �� ������ ������ ã��
	TraceForItems();
}


void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// PlayerInputComponent�� ���� �Է� ������Ʈ�� ĳ����
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// �Է� �׼ǰ� ó�� �Լ��� ���ε�
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
	// �Է°�(Vector2D)�� ������
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// ��Ʈ�ѷ��� ȸ�� ������ �������� ����/�¿� ������ ã��
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// �ش� �������� �̵� �Է��� �߰�
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}


void APlayerCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// ���콺 �Է¿� ���� ��Ʈ�ѷ��� Yaw, Pitch ���� ����
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
	FVector End = Start + (ForwardVector * 500.0f); // ī�޶� �� 5m

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	// �տ� �� �������� ����
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

	// ���� ������ ���̶���Ʈ ����
	if (CurrentFocusedItem && CurrentFocusedItem != HitItem)
	{
		CurrentFocusedItem->HighlightItem(false);
		CurrentFocusedItem = nullptr;
	}

	// ���ο� ������ ���̶���Ʈ
	if (HitItem && HitItem != CurrentFocusedItem)
	{
		HitItem->HighlightItem(true);
		CurrentFocusedItem = HitItem;
	}

}

// ������ �ݱ�
void APlayerCharacter::PickupItem(AItemBase* Item)
{
	if (!Item || !IsValid(Item)) { UE_LOG(LogTemp, Error, TEXT("Pickup FAILED: Item invalid")); return; }
	if (!GetMesh()) { UE_LOG(LogTemp, Error, TEXT("Pickup FAILED: No mesh")); return; }
	if (!GetMesh()->DoesSocketExist(HandSocketName)) { UE_LOG(LogTemp, Error, TEXT("Pickup FAILED: no socket")); return; }

	// safety����
	if (Item->ItemMesh)
	{
		Item->ItemMesh->SetSimulatePhysics(false);
		Item->ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	bool bAttached = Item->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), HandSocketName);
	if (bAttached)
	{
		// ���� transform ����
		FTransform SocketTF = GetMesh()->GetSocketTransform(HandSocketName, RTS_World);
		Item->SetActorTransform(SocketTF);
	}

	UE_LOG(LogTemp, Warning, TEXT("Pickup: %s attach=%d parent=%s loc=%s"),
		*Item->GetName(), bAttached, *GetNameSafe(Item->GetAttachParentActor()), *Item->GetActorLocation().ToString());

	Item->OnPickedUp();
}


// ������ ��������
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


// Interact (�����)
void APlayerCharacter::Interact()
{
	if (!CurrentFocusedItem) return;

	AItemBase* NewItem = CurrentFocusedItem;
	if (!IsValid(NewItem)) return;

	// ���� �տ� �ٸ� ������ ������ ���� ����
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
		// �����Կ� ���
		if (QuickSlotRef)
		{
			QuickSlotRef->AddItemToEmptySlot(NewItem);
		}
		UE_LOG(LogTemp, Warning, TEXT("Interact: Picked up %s"), *NewItem->GetName());
	}
}

void APlayerCharacter::DropHeldItem()
{
	if (HeldItem) // �տ� �������� ���� ���� ����
	{
		FVector DropLocation = GetActorLocation() + GetActorForwardVector() * 50.f;
		DropLocation.Z += 30.f; // �ٴڿ� ������ �ʵ��� ��¦ �ø�

		ChangeItem(HeldItem, DropLocation);
		UE_LOG(LogTemp, Warning, TEXT("DropHeldItem: Dropped %s"), *HeldItem->GetName());

		HeldItem = nullptr;
	}
	else
	{
		// �տ� �ƹ��͵� ������ �ƹ� �ϵ� �� ��
		UE_LOG(LogTemp, Warning, TEXT("DropHeldItem: No item in hand"));
	}
}

void APlayerCharacter::SelectQuickSlot(int32 SlotIndex)
{
	if (!QuickSlotRef) return;

	// ���� ���õ� ���� ����
	QuickSlotRef->SetCurrentSlot(SlotIndex);
	

	// ������ ������ ��������
	AItemBase* ItemToEquip = QuickSlotRef->GetItemAt(SlotIndex);

	// ������ ����ִٸ� �տ� �� ������ ��������
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

	// �̹� ���� �������� ��� �ִٸ� �ƹ� ��ȭ ����
	if (HeldItem == ItemToEquip)
	{
		UE_LOG(LogTemp, Warning, TEXT("Slot %d already equipped."), SlotIndex);
		return;
	}

	// ���� ������ ��������
	if (HeldItem)
	{
		DropHeldItem();
	}

	// ���õ� �����۸� �ʿ��� ���̰�
	ItemToEquip->SetActorHiddenInGame(false);

	// �� ������ �տ� ���
	PickupItem(ItemToEquip);
	HeldItem = ItemToEquip;

	UE_LOG(LogTemp, Warning, TEXT("Equipped item from slot %d: %s"), SlotIndex, *ItemToEquip->GetName());
}