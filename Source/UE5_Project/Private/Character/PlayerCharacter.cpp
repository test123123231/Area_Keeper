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

	// 매 프레임 아이템 찾기
	TraceForItems();
}


void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// PlayerInputComponent를 향상된 입력 컴포넌트로 캐스팅
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// 입력 액션과 처리 함수를 바인딩
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
	// 입력값(Vector2D)을 가져옴
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// 컨트롤러의 회전 방향을 기준으로 전후/좌우 방향을 찾기
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// 해당 방향으로 이동 입력을 추가
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}


void APlayerCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// 마우스 입력에 따라 컨트롤러의 Yaw, Pitch 값을 조절
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
	FVector End = Start + (ForwardVector * 500.0f); // 카메라 앞 5m

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	// 손에 든 아이템은 무시
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

	// 이전 아이템 하이라이트 해제
	if (CurrentFocusedItem && CurrentFocusedItem != HitItem)
	{
		CurrentFocusedItem->HighlightItem(false);
		CurrentFocusedItem = nullptr;
	}

	// 새로운 아이템 하이라이트
	if (HitItem && HitItem != CurrentFocusedItem)
	{
		HitItem->HighlightItem(true);
		CurrentFocusedItem = HitItem;
	}

}

// 아이템 줍기
void APlayerCharacter::PickupItem(AItemBase* Item)
{
	if (!Item || !IsValid(Item)) { UE_LOG(LogTemp, Error, TEXT("Pickup FAILED: Item invalid")); return; }
	if (!GetMesh()) { UE_LOG(LogTemp, Error, TEXT("Pickup FAILED: No mesh")); return; }
	if (!GetMesh()->DoesSocketExist(HandSocketName)) { UE_LOG(LogTemp, Error, TEXT("Pickup FAILED: no socket")); return; }

	// safety끄기
	if (Item->ItemMesh)
	{
		Item->ItemMesh->SetSimulatePhysics(false);
		Item->ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	bool bAttached = Item->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), HandSocketName);
	if (bAttached)
	{
		// 강제 transform 맞춤
		FTransform SocketTF = GetMesh()->GetSocketTransform(HandSocketName, RTS_World);
		Item->SetActorTransform(SocketTF);
	}

	UE_LOG(LogTemp, Warning, TEXT("Pickup: %s attach=%d parent=%s loc=%s"),
		*Item->GetName(), bAttached, *GetNameSafe(Item->GetAttachParentActor()), *Item->GetActorLocation().ToString());

	Item->OnPickedUp();
}


// 아이템 내려놓기
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


// Interact (방어적)
void APlayerCharacter::Interact()
{
	if (!CurrentFocusedItem) return;

	AItemBase* NewItem = CurrentFocusedItem;
	if (!IsValid(NewItem)) return;

	// 만약 손에 다른 아이템 있으면 먼저 놓기
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
		// 퀵슬롯에 등록
		if (QuickSlotRef)
		{
			QuickSlotRef->AddItemToEmptySlot(NewItem);
		}
		UE_LOG(LogTemp, Warning, TEXT("Interact: Picked up %s"), *NewItem->GetName());
	}
}

void APlayerCharacter::DropHeldItem()
{
	if (HeldItem) // 손에 아이템이 있을 때만 실행
	{
		FVector DropLocation = GetActorLocation() + GetActorForwardVector() * 50.f;
		DropLocation.Z += 30.f; // 바닥에 묻히지 않도록 살짝 올림

		ChangeItem(HeldItem, DropLocation);
		UE_LOG(LogTemp, Warning, TEXT("DropHeldItem: Dropped %s"), *HeldItem->GetName());

		HeldItem = nullptr;
	}
	else
	{
		// 손에 아무것도 없으면 아무 일도 안 함
		UE_LOG(LogTemp, Warning, TEXT("DropHeldItem: No item in hand"));
	}
}

void APlayerCharacter::SelectQuickSlot(int32 SlotIndex)
{
	if (!QuickSlotRef) return;

	// 현재 선택된 슬롯 변경
	QuickSlotRef->SetCurrentSlot(SlotIndex);
	

	// 슬롯의 아이템 가져오기
	AItemBase* ItemToEquip = QuickSlotRef->GetItemAt(SlotIndex);

	// 슬롯이 비어있다면 손에 든 아이템 내려놓기
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

	// 이미 같은 아이템을 들고 있다면 아무 변화 없음
	if (HeldItem == ItemToEquip)
	{
		UE_LOG(LogTemp, Warning, TEXT("Slot %d already equipped."), SlotIndex);
		return;
	}

	// 기존 아이템 내려놓기
	if (HeldItem)
	{
		DropHeldItem();
	}

	// 선택된 아이템만 맵에서 보이게
	ItemToEquip->SetActorHiddenInGame(false);

	// 새 아이템 손에 쥐기
	PickupItem(ItemToEquip);
	HeldItem = ItemToEquip;

	UE_LOG(LogTemp, Warning, TEXT("Equipped item from slot %d: %s"), SlotIndex, *ItemToEquip->GetName());
}