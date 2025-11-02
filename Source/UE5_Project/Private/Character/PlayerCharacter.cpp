#include "Character/PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SpotLightComponent.h"
#include "EnhancedInputComponent.h"
#include "Item/ItemBase.h"
#include "Item/ToolBase.h"
#include "Item/ChargeableItem.h"
#include "Anomaly/StationaryAnomaly.h"
#include "Anomaly/ChasingAnomaly.h"
#include "HUD/QuickSlot.h"
#include "DrawDebugHelpers.h"
#include "Components/AttributeComponent.h"
#include "PlayerController/PlayerCharacterController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interfaces/InteractableInterface.h"
#include "Components/CapsuleComponent.h"


APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true; // 틱 사용

	// 'AreaKeeper'의 3인칭 카메라/이동 설정
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 300.f;
	SpringArm->bUsePawnControlRotation = true;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(SpringArm);

	// 1인칭으로 변경 시 추가 코드
	// ViewCamera->SetupAttachment(GetMesh(), FName("head")); // 1인칭
	// ViewCamera->bUsePawnControlRotation = true;
	// SpringArm->Deactivate();

	CurrentFocusedInteractable = nullptr;
	HeldItem = nullptr;
}


void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// '소지' UI가 열려있으면 플레이어 틱(추적, 충전)을 멈춤
	if (bIsExorcismUIOpen || !IsAlive()) return;

	HandleCharging(DeltaTime);
	TraceForInteractable();
}


void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	Tags.Add(FName("PlayerCharacter"));

	GetAttributes()->SetMaxHealth(3.f);
	GetAttributes()->SetMaxAmulet(5.f);
	GetAttributes()->SetHealth(3.f);
	GetAttributes()->SetAmulet(5.f);
}


void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);

		// E키 바인딩
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &APlayerCharacter::OnInteractPressed);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Completed, this, &APlayerCharacter::OnInteractReleased);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Canceled, this, &APlayerCharacter::OnInteractReleased);

		// G키 바인딩
		EnhancedInputComponent->BindAction(DropAction, ETriggerEvent::Started, this, &APlayerCharacter::OnDropItem);

		// F키 바인딩
		//EnhancedInputComponent->BindAction(FlashlightAction, ETriggerEvent::Started, this, &APlayerCharacter::ToggleFlashlight);

		// Ctrl키 바인딩
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &APlayerCharacter::StartCrouch);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &APlayerCharacter::StopCrouch);
	}
}


void APlayerCharacter::Die()
{
	Super::Die();

	// 플레이어 입력 비활성화
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		DisableInput(PC);
	}

	// 게임오버 UI 띄우기 (GameMode 또는 PlayerController에서 처리)
}


// 기본 입력
void APlayerCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}


void APlayerCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}


void APlayerCharacter::StartCrouch()
{
	Crouch();
}


void APlayerCharacter::StopCrouch()
{
	UnCrouch();
}


// 상호작용 마스터 로직
// 매 틱 호출: IInteractableInterface를 구현한 객체를 찾음
void APlayerCharacter::TraceForInteractable()
{
	FVector Start = ViewCamera->GetComponentLocation();
	FVector ForwardVector = ViewCamera->GetForwardVector();
	FVector End = Start + (ForwardVector * 500.0f); // 5m

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	if (HeldItem) Params.AddIgnoredActor(HeldItem);

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);

	TScriptInterface<IInteractableInterface> HitInteractable = nullptr;
	if (bHit)
	{
		HitInteractable = TScriptInterface<IInteractableInterface>(HitResult.GetActor());
	}

	if (CurrentFocusedInteractable != HitInteractable)
	{
		if (CurrentFocusedInteractable)
		{
			IInteractableInterface::Execute_Highlight(CurrentFocusedInteractable.GetObject(), false);
		}
		if (HitInteractable)
		{
			IInteractableInterface::Execute_Highlight(HitInteractable.GetObject(), true);
		}
		CurrentFocusedInteractable = HitInteractable;
	}
}


// E키를 눌렀을 때: 모든 상호작용의 분기점
void APlayerCharacter::OnInteractPressed()
{
	// 쫓아오는 이상현상을 바라보고 있고, 손에 '도구'를 들고 있는가?
	if (Cast<AChasingAnomaly>(CurrentFocusedInteractable.GetObject()) && HeldItem && Cast<AToolBase>(HeldItem))
	{
		UseTool();
		return;
	}

	// '충전기'를 바라보고 있는가?
	if (AChargeableItem* Charger = Cast<AChargeableItem>(CurrentFocusedInteractable.GetObject()))
	{
		StartCharge(Charger);
		return;
	}

	// '정지된 이상현상'을 바라보고 있는가?
	if (AStationaryAnomaly* Anomaly = Cast<AStationaryAnomaly>(CurrentFocusedInteractable.GetObject()))
	{
		StartExorcism(Anomaly);
		return;
	}

	// '아이템' 또는 '도구'를 바라보고 있는가?
	if (Cast<AItemBase>(CurrentFocusedInteractable.GetObject()))
	{
		// 인터페이스의 Interact 함수 호출 (ItemBase.cpp 또는 ToolBase.cpp의 줍기 로직 실행)
		IInteractableInterface::Execute_Interact(CurrentFocusedInteractable.GetObject(), this);
		return;
	}

	// 아무것도 바라보지 않지만, 손에 '도구'를 들고 있는가? (허공에 사용)
	if (!CurrentFocusedInteractable && HeldItem && Cast<AToolBase>(HeldItem))
	{
		UseTool();
		return;
	}
}


// E키를 뗐을 때
void APlayerCharacter::OnInteractReleased()
{
	if (bIsCharging)
	{
		StopCharge();
	}
}


// 아이템 줍기/버리기
void APlayerCharacter::PickupItem(AItemBase* Item)
{
	if (!Item || !QuickSlotRef) return;

	int32 TargetSlotIndex = QuickSlotRef->GetCurrentSlotIndex();
	if (TargetSlotIndex == INDEX_NONE) TargetSlotIndex = 0;

	// 손에 다른 아이템이 있으면 교체
	if (HeldItem && HeldItem != Item)
	{
		QuickSlotRef->RemoveItemAt(TargetSlotIndex);
		FVector DropLocation = Item->GetActorLocation();
		ChangeItem(HeldItem, DropLocation);
		HeldItem = nullptr;
	}

	// 새 아이템 줍기 (ItemBase의 OnPickedUp 호출)
	Item->OnPickedUp(GetMesh(), HandSocketName);
	HeldItem = Item;
	QuickSlotRef->AssignItemToSlot(TargetSlotIndex, Item);

	if (CurrentFocusedInteractable.GetObject() == Item)
	{
		IInteractableInterface::Execute_Highlight(Item, false);
		CurrentFocusedInteractable = nullptr;
	}
}


void APlayerCharacter::ChangeItem(AItemBase* Item, const FVector& Location)
{
	if (!Item) return;
	Item->OnDropped();
	Item->SetActorLocation(Location);
}


void APlayerCharacter::OnDropItem()
{
	if (!HeldItem || !QuickSlotRef) return;

	int32 TargetSlotIndex = QuickSlotRef->GetCurrentSlotIndex();
	if (TargetSlotIndex == INDEX_NONE) TargetSlotIndex = 0;
	AItemBase* ItemInSlot = QuickSlotRef->GetItemAt(TargetSlotIndex);

	if (!ItemInSlot || HeldItem != ItemInSlot) return; // 손에 든 것과 슬롯이 다르면 무시

	QuickSlotRef->RemoveItemAt(TargetSlotIndex);

	FVector DropLocation = GetActorLocation() + GetActorForwardVector() * 100.f;
	DropLocation.Z += 30.f;

	ChangeItem(HeldItem, DropLocation);
	HeldItem = nullptr;
}
// ---


// 퀵슬롯
void APlayerCharacter::SetQuickSlotRef(UQuickSlot* NewRef)
{
	QuickSlotRef = NewRef;
}


void APlayerCharacter::SelectQuickSlot(int32 SlotIndex)
{
	if (!QuickSlotRef) return;
	QuickSlotRef->SetCurrentSlot(SlotIndex);
	AItemBase* ItemToEquip = QuickSlotRef->GetItemAt(SlotIndex);

	if (!ItemToEquip)
	{
		if (HeldItem) HeldItem->SetActorHiddenInGame(true);
		HeldItem = nullptr;
		return;
	}
	if (HeldItem == ItemToEquip)
	{
		HeldItem->SetActorHiddenInGame(false);
		return;
	}
	if (HeldItem)
	{
		HeldItem->SetActorHiddenInGame(true);
	}

	ItemToEquip->OnPickedUp(GetMesh(), HandSocketName); // ItemBase.h에 OnPickedUp 시그니처 필요
	ItemToEquip->SetActorHiddenInGame(false);
	HeldItem = ItemToEquip;
}
// ---


// '지방' 충전 
void APlayerCharacter::StartCharge(AChargeableItem* Target)
{
	if (!Target) return;

	ChargingTarget = Target;
	bIsCharging = true;
	ChargeTime = 0.0f;
}


void APlayerCharacter::StopCharge()
{
	if (!bIsCharging) return;
	if (auto* Pcc = Cast<APlayerCharacterController>(GetController()))
	{
		 Pcc->HideText();
	}
	bIsCharging = false;
	ChargeTime = 0.0f;
	ChargingTarget.Reset();
}


void APlayerCharacter::HandleCharging(float DeltaTime)
{
	if (!bIsCharging) return;

	// 여전히 같은 오브젝트를 보고 있는지(TraceForItems가 유지)
	if (!ChargingTarget.IsValid() || CurrentFocusedInteractable.GetObject() != ChargingTarget.Get())
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
		Pcc->ShowAutoText(2.0f);
		Pcc->UpdateText(FString::Printf(TEXT("아직 쿨타임입니다. 남은 시간 : %.1f 초"), Remain));
		return;
	}

	Pcc->ShowText();
	Pcc->UpdateText(FString::Printf(TEXT("충전 중.. %.1f초"), (2.0f - ChargeTime)));
	ChargeTime += DeltaTime;


	//충전 시간이 지난후 실행
	if (ChargeTime >= RequiredChargeTime)
	{
		Pcc->UpdateText(TEXT("충전 완료"));
		Pcc->ShowAutoText(2.0f);
		bIsCharging = false;

		bool ChargeSuccess = ChargingTarget->OnCharged();

		if (ChargeSuccess)
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


// '소지'
void APlayerCharacter::StartExorcism(AStationaryAnomaly* Anomaly)
{
	if (bIsExorcismUIOpen || !Anomaly) return;

	// (SRS 4.1.6) 게임 일시 정지
	bIsExorcismUIOpen = true;
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		PC->SetPause(true);
		// 플레이어 입력은 막되, UI 조작은 가능해야 함
		PC->SetInputMode(FInputModeGameAndUI());
		PC->bShowMouseCursor = true;
	}

	// '지방' UI 표시
	// Anomaly->ShowExorcismUI(this); // AStationaryAnomaly에 UI 표시 함수 구현 필요
	UE_LOG(LogTemp, Warning, TEXT("'소지' 시작. UI를 엽니다..."));

	// 참고: UI가 닫힐 때(성공/실패/취소) APlayerCharacter의 함수를 호출하여
	// bIsExorcismUIOpen = false; PC->SetPause(false); 등을 실행해야 함
}


// 도구 사용
void APlayerCharacter::UseTool()
{
	AToolBase* Tool = Cast<AToolBase>(HeldItem);
	if (!Tool) return;

	// 쫓아오는 이상현상에게 사용
	AChasingAnomaly* TargetAnomaly = Cast<AChasingAnomaly>(CurrentFocusedInteractable.GetObject());
	if (TargetAnomaly)
	{
		if (Tool->UseTool(TargetAnomaly)) // ToolBase.h/cpp에 UseTool 구현 필요
		{
			// (SRS 9.2.3) 사용 성공 시 손에서 제거
			QuickSlotRef->RemoveItemAt(QuickSlotRef->GetCurrentSlotIndex());
			HeldItem = nullptr;
		}
	}
}


// 플레이어 피해 처리 (무적 시간 로직 포함)
void APlayerCharacter::HandleDamage(float DamageAmount)
{
	if (bIsInvincible)
	{
		// 무적 상태일 때는 데미지를 받지 않음
		return;
	}

	if (Attributes)
	{
		Attributes->ReceiveDamage(DamageAmount);

		// 피해를 입었으므로 1초간 무적 상태로 만듦
		bIsInvincible = true;
		GetWorld()->GetTimerManager().SetTimer(
			InvincibilityTimerHandle,
			this,
			&APlayerCharacter::ResetInvincibility,
			1.0f, // 1초 무적
			false
		);

		// 피격 시각 효과 (예: 화면 붉어짐)
		// APlayerCharacterController* PC = Cast<APlayerCharacterController>(GetController());
		// if (PC)
		// {
		// 	PC->PlayHitEffect(); 
		// }
	}
}


void APlayerCharacter::ResetInvincibility()
{
	bIsInvincible = false;
}