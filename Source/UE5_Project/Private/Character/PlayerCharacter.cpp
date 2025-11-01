#include "Character/PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "Item/ItemBase.h"
#include "HUD/QuickSlot.h"
#include "DrawDebugHelpers.h"
#include "Item/ChargeableItem.h"
#include "Components/AttributeComponent.h"
#include "PlayerController/PlayerCharacterController.h"


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
	// 매 프레임 마다 충전
	HandleCharging(DeltaTime);
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
			//짧게 
			EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &APlayerCharacter::Interact);

            //홀드
            EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started,   this, &APlayerCharacter::StartCharge);
            EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Completed, this, &APlayerCharacter::StopCharge);
            EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Canceled,  this, &APlayerCharacter::StopCharge);
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

		Item->SetActorScale3D(FVector(0.3f, 0.3f, 0.3f));
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

	Item->SetActorScale3D(FVector(0.3f, 0.3f, 0.3f));

	Item->OnDropped();

	UE_LOG(LogTemp, Warning, TEXT("Drop: %s AfterDetach Parent=%s Loc=%s"),
		*Item->GetName(), *GetNameSafe(Item->GetAttachParentActor()), *Item->GetActorLocation().ToString());
}

void APlayerCharacter::Interact()
{
	if (!CurrentFocusedItem || !QuickSlotRef) return;

	//충전 메쉬는 소유하지 않게 하는 가드
	if (CurrentFocusedItem && Cast<AChargeableItem>(CurrentFocusedItem))
    {
        // 충전은 StartCharge/HandleCharging 로직으로만 처리
        UE_LOG(LogTemp, Display,  TEXT("충전용"));
        return;
    }

	AItemBase* NewItem = CurrentFocusedItem;
	if (!IsValid(NewItem)) return;

	int32 TargetSlotIndex = QuickSlotRef->GetCurrentSlotIndex(); // 현재 선택된 슬롯 인덱스
	if (TargetSlotIndex == INDEX_NONE) TargetSlotIndex = 0;

	// 손에 다른 아이템이 있으면 교체 처리
	if (HeldItem && HeldItem != NewItem)
	{
		// 기존 아이템을 해당 슬롯에서 제거
		QuickSlotRef->RemoveItemAt(TargetSlotIndex);

		FVector DropLocation = NewItem->GetActorLocation();
		DropLocation.Z += 50.f;
		ChangeItem(HeldItem, DropLocation);
		HeldItem = nullptr;
	}

	// 새 아이템 줍기
	PickupItem(NewItem);
	HeldItem = NewItem;
	NewItem->HighlightItem(false);
	CurrentFocusedItem = nullptr;

	QuickSlotRef->AssignItemToSlot(TargetSlotIndex, NewItem);


	UE_LOG(LogTemp, Warning, TEXT("Interact: Added %s to slot %d"), *NewItem->GetName(), TargetSlotIndex);
}

void APlayerCharacter::DropHeldItem()
{
	if (!HeldItem || !QuickSlotRef) return;

	int32 TargetSlotIndex = QuickSlotRef->GetCurrentSlotIndex();
	if (TargetSlotIndex == INDEX_NONE) TargetSlotIndex = 0;

	// 현재 슬롯의 아이템 가져오기
	AItemBase* ItemInSlot = QuickSlotRef->GetItemAt(TargetSlotIndex);

	// 빈 슬롯이면 그냥 리턴 (아무 일도 안 함)
	if (!ItemInSlot)
	{
		UE_LOG(LogTemp, Warning, TEXT("DropHeldItem: Current slot %d is empty, nothing to drop."), TargetSlotIndex);
		return;
	}

	// 현재 손에 든 아이템이 슬롯의 아이템과 다르면 드롭 금지
	if (HeldItem != ItemInSlot)
	{
		UE_LOG(LogTemp, Warning, TEXT("DropHeldItem: HeldItem does not match slot %d item."), TargetSlotIndex);
		return;
	}

	QuickSlotRef->RemoveItemAt(TargetSlotIndex); // 현재 슬롯 기준으로 제거

	FVector DropLocation = GetActorLocation() + GetActorForwardVector() * 50.f;
	DropLocation.Z += 30.f;

	HeldItem->SetActorHiddenInGame(false);
	HeldItem->SetActorEnableCollision(true);

	ChangeItem(HeldItem, DropLocation);
	HeldItem = nullptr;

	UE_LOG(LogTemp, Warning, TEXT("Dropped held item from slot %d"), TargetSlotIndex);
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
			HeldItem->SetActorHiddenInGame(true);

			HeldItem->SetActorEnableCollision(false);

			HeldItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			HeldItem->SetActorLocation(GetActorLocation());
			UE_LOG(LogTemp, Warning, TEXT("Slot %d is empty. Dropped held item."), SlotIndex);
		}
		HeldItem = nullptr;
		return;
	}

	// 이미 같은 아이템을 들고 있다면 아무 변화 없음
	if (HeldItem == ItemToEquip)
	{
		HeldItem->SetActorHiddenInGame(false);
		HeldItem->SetActorEnableCollision(false); // 손에 있을 땐 항상 꺼둬야 함
		UE_LOG(LogTemp, Warning, TEXT("Slot %d already equipped."), SlotIndex);
		//HeldItem->SetActorHiddenInGame(false); // 혹시라도 숨겨진 상태면 다시 보이게
		return;
	}

	// 기존 아이템 내려놓기
	if (HeldItem)
	{
		HeldItem->SetActorHiddenInGame(true);

		HeldItem->SetActorEnableCollision(false);

		HeldItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		HeldItem->SetActorLocation(GetActorLocation());
	} 

	// 새 아이템 손에 쥐기
	PickupItem(ItemToEquip);
	HeldItem = ItemToEquip;
	HeldItem->SetActorHiddenInGame(false);
	HeldItem->SetActorEnableCollision(false);

	UE_LOG(LogTemp, Warning, TEXT("Equipped item from slot %d: %s"), SlotIndex, *ItemToEquip->GetName());
}

void APlayerCharacter::StartCharge()
{
    // TraceForItems가 갱신해둔 현재 포커스 대상으로부터만 시작
    AChargeableItem* Target = Cast<AChargeableItem>(CurrentFocusedItem);
    if (!Target)
        return;

    ChargingTarget = Target;  // 타겟 잠금
    bIsCharging = true;
    ChargeTime = 0.0f;
}

void APlayerCharacter::StopCharge()
{
    if (!bIsCharging) return;
	if(auto* Pcc = Cast<APlayerCharacterController>(GetController()))
	{
		Pcc -> HideText(0);
	}
    bIsCharging = false;
    ChargeTime = 0.0f;
    ChargingTarget.Reset();

	UE_LOG(LogTemp, Display, TEXT("부적 충전 취소"));
}

void APlayerCharacter::HandleCharging(float DeltaTime)
{
    if (!bIsCharging) return;

    // 여전히 같은 오브젝트를 보고 있는지(TraceForItems가 유지)
    if (!ChargingTarget.IsValid() || CurrentFocusedItem != ChargingTarget.Get())
    {
        // 시선을 벗어나면 취소
        StopCharge();
        return;
    }
	auto* Pcc = Cast<APlayerCharacterController>(GetController());

	//쿨타임인지 판정
	if (ChargingTarget->bIsCharged)
    {
        const float Remain = FMath::Max(0.f, ChargingTarget->RechargeCooldown - ChargingTarget->Cooldown);
		Pcc -> ShowAutoText(2.0f);
		Pcc -> UpdateText(FString::Printf(TEXT("아직 쿨타임입니다. 남은 시간 : %.1f 초"), Remain), 0);
        return;
    }
	
	Pcc -> ShowText(0);
	Pcc -> UpdateText(FString::Printf(TEXT("충전 중.. %.1f초"), (2.0f - ChargeTime)), 0);
    ChargeTime += DeltaTime;


	//충전 시간이 지난후 실행
    if (ChargeTime >= RequiredChargeTime)
    {
		Pcc -> UpdateText(TEXT("충전 완료"), 0);
		Pcc -> ShowAutoText(2.0f);
        bIsCharging = false;

        bool ChargeSuccess = ChargingTarget->OnCharged();

		if(ChargeSuccess)
		{
			if (UAttributeComponent* Attr = FindComponentByClass<UAttributeComponent>())
    		{
        		Attr->SetAmulet(5.0f);   //최대로 충전
    		}
		
		}

		ChargingTarget.Reset();
		ChargeTime = 0.0f;
    }

}

