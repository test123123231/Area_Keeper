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
#include "TimerManager.h"
#include "Game/AreaKeeperGameState.h"
#include "Game/GameTypes.h"


APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// 3인칭 카메라 설정
	/*SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 300.f;
	SpringArm->bUsePawnControlRotation = true;*/

	//// 'AreaKeeper'의 3인칭 이동 설정을 가져옴 (APlayerCharacter가 오버라이드할 수 있음)
	//GetCharacterMovement()->bOrientRotationToMovement = true;
	//GetCharacterMovement()->RotationRate = FRotator(0.f, 360.f, 0.f);
	//bUseControllerRotationYaw = false;
	//bUseControllerRotationPitch = false;
	//bUseControllerRotationRoll = false;

	// 컨트롤러(마우스)가 회전할 때 캐릭터 몸통도 같이 회전하도록 설정
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// 이동 방향으로 캐릭터가 자동으로 회전하는 것을 방지
	GetCharacterMovement()->bOrientRotationToMovement = false;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(GetRootComponent());
	ViewCamera->bUsePawnControlRotation = true;

	FlashLightComponent = CreateDefaultSubobject<USpotLightComponent>(TEXT("FlashLightComponent"));
	FlashLightComponent->SetupAttachment(ViewCamera); // 1인칭 시 카메라에 부착
	FlashLightComponent->SetIntensity(8000.0f);
	FlashLightComponent->SetIntensityUnits(ELightUnits::Unitless);
	FlashLightComponent->SetLightColor(FLinearColor::White);
	FlashLightComponent->SetAttenuationRadius(2500.0f);
	FlashLightComponent->SetInnerConeAngle(15.0f);
	FlashLightComponent->SetOuterConeAngle(25.0f);
	FlashLightComponent->bAffectsWorld = true;
	FlashLightComponent->SetCastShadows(true);
	FlashLightComponent->SetVisibility(false); // 기본적으로 꺼져있음

	CurrentFocusedInteractable = nullptr;
	HeldItem = nullptr;
	bIsInvincible = false;
	bIsTalismanRitualUIOpen = false;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}


void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// '소지' UI가 열려있으면 플레이어 틱(추적, 충전)을 멈춤
	if (bIsTalismanRitualUIOpen || !IsAlive()) return;

	HandleCharging(DeltaTime);
	TraceForInteractable();
}


void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	Tags.Add(FName("PlayerCharacter"));

	// AttributeComponent에 기본 값을 할당 함
	if (GetAttributes())
	{
		GetAttributes()->SetMaxHealth(3.f);
		GetAttributes()->SetMaxTalisman(5.f);
		GetAttributes()->SetHealth(3.f);
		GetAttributes()->SetTalisman(5.f);
	}

	GameStateRef = GetWorld() ? GetWorld()->GetGameState<AAreaKeeperGameState>() : nullptr;
	PlayerControllerRef = Cast<APlayerCharacterController>(GetController());
}


void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);

		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &APlayerCharacter::OnInteractPressed);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Completed, this, &APlayerCharacter::OnInteractReleased);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Canceled, this, &APlayerCharacter::OnInteractReleased);

		EnhancedInputComponent->BindAction(DropAction, ETriggerEvent::Started, this, &APlayerCharacter::OnDropItem);

		// 손전등, 웅크리기 바인딩
		EnhancedInputComponent->BindAction(FlashlightAction, ETriggerEvent::Started, this, &APlayerCharacter::OnFlashlightPressed);
		//EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &APlayerCharacter::OnCrouchPressed);
		// EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &APlayerCharacter::StopCrouch); // (주석) 토글 방식이므로 Started만 사용
	}
}


void APlayerCharacter::Die()
{
	Super::Die();

	if (GameStateRef.IsValid())
	{
		GameStateRef->GameOver(true);
	}
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


void APlayerCharacter::OnFlashlightPressed()
{
	// 손전등 끄고 켜기
	if (FlashLightComponent)
	{
		bool bIsVisible = FlashLightComponent->IsVisible();
		FlashLightComponent->SetVisibility(!bIsVisible);
	}
}
//
//void APlayerCharacter::OnCrouchPressed()
//{
//	// (SRS 12.2.1) 웅크리기 토글
//	if (bIsCrouched)
//	{
//		UnCrouch();
//	}
//	else
//	{
//		Crouch();
//	}
//}


//  상호작용 마스터 로직
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

		UpdateInteractionPrompt(HitInteractable);
		CurrentFocusedInteractable = HitInteractable;
	}
}


void APlayerCharacter::UpdateInteractionPrompt(TScriptInterface<IInteractableInterface>& HitInteractable)
{
	
	if (!PlayerControllerRef)
	{
		return;
	}

	FString InteractText;
	bool bTextShow = false;

	if (HitInteractable)
	{
		UObject* FocusedObj = HitInteractable.GetObject();
		if (AChasingAnomaly* Chasing = Cast<AChasingAnomaly>(FocusedObj))
		{
			if (HeldItem && Cast<AToolBase>(HeldItem))
			{
				InteractText = IInteractableInterface::Execute_GetInteractText(FocusedObj);
				bTextShow = true;
			}
			else
			{
				bTextShow = false;
			}
		}
		else
		{
			InteractText = IInteractableInterface::Execute_GetInteractText(HitInteractable.GetObject());
			bTextShow = true;
		}
	}

	if (bTextShow)
	{
		PlayerControllerRef->ShowText(0);
		PlayerControllerRef->UpdateText(InteractText, 0);
	}
	else
	{
		PlayerControllerRef->HideText(0);
	}
}


// OnInteractPressed 로직 단순화
void APlayerCharacter::OnInteractPressed()
{
	// 쫓아오는 이상현상을 바라보고 있고, 손에 '도구'를 들고 있는가?
	if (Cast<AChasingAnomaly>(CurrentFocusedInteractable.GetObject()) && HeldItem && Cast<AToolBase>(HeldItem))
	{
		UseTool();
		return;
	}

	// AStationaryAnomaly, AItemBase, AChargeableItem 등 모든 나머지 IInteractableInterface 객체는 이 범용 로직을 따름
	if (CurrentFocusedInteractable)
	{
		// AStationaryAnomaly -> Interact_Implementation -> Player->StartExorcism(this)
		// AItemBase -> Interact_Implementation -> Player->PickupItem(this)
		IInteractableInterface::Execute_Interact(CurrentFocusedInteractable.GetObject(), this);
		return;
	}

}


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
	if (!Item || !QuickSlotRef || !PlayerControllerRef) return;
	PlayerControllerRef -> HideText(0);
	int32 TargetSlotIndex = QuickSlotRef->GetCurrentSlotIndex();
	if (TargetSlotIndex == INDEX_NONE) TargetSlotIndex = 0;

	if (HeldItem && HeldItem != Item)
	{
		QuickSlotRef->RemoveItemAt(TargetSlotIndex);
		FVector DropLocation = Item->GetActorLocation();
		ChangeItem(HeldItem, DropLocation);
		HeldItem = nullptr;
	}

	Item->OnPickedUp(GetMesh(), HandSocketName);
	HeldItem = Item;
	QuickSlotRef->AssignItemToSlot(TargetSlotIndex, Item);

	if (CurrentFocusedInteractable.GetObject() == Item)
	{
		IInteractableInterface::Execute_Highlight(CurrentFocusedInteractable.GetObject(), false);
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

	if (!ItemInSlot || HeldItem != ItemInSlot) return;

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

	ItemToEquip->OnPickedUp(GetMesh(), HandSocketName);
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
	if (!bIsCharging||!PlayerControllerRef) return;
	
	PlayerControllerRef -> UpdateText(TEXT("충전하기"), 0);
	bIsCharging = false;
	ChargeTime = 0.0f;
	ChargingTarget.Reset();
}


void APlayerCharacter::HandleCharging(float DeltaTime)
{
	if (!bIsCharging || !PlayerControllerRef) return;

	if (!ChargingTarget.IsValid() || CurrentFocusedInteractable.GetObject() != ChargingTarget.Get())
	{
		StopCharge();
		return;
	}

	// 쿨타임 중일 때
	if (ChargingTarget->bIsCharged)
    {
        const float Remain = FMath::Max(0.f, ChargingTarget->RechargeCooldown - ChargingTarget->Cooldown);
		PlayerControllerRef -> ShowAutoText(2.0f, 0);
		PlayerControllerRef -> UpdateText(FString::Printf(TEXT("아직 쿨타임입니다. 남은 시간 : %.1f 초"), Remain), 0);
        return;
    }
	
	PlayerControllerRef -> ShowText(0);
	PlayerControllerRef -> UpdateText(FString::Printf(TEXT("충전 중.. %.1f초"), (2.0f - ChargeTime)), 0);
    ChargeTime += DeltaTime;


	//충전 시간이 지난후 실행
    if (ChargeTime >= RequiredChargeTime)
    {
		PlayerControllerRef -> UpdateText(TEXT("충전 완료"), 0);
		PlayerControllerRef -> ShowAutoText(2.0f, 0);
        bIsCharging = false;

		bool ChargeSuccess = ChargingTarget->OnCharged(); // 쿨타임 시작

		if (ChargeSuccess && GetAttributes())
		{
			GetAttributes()->SetTalisman(GetAttributes()->GetMaxTalisman()); //최대로 충전
		}

		ChargingTarget.Reset();
		ChargeTime = 0.0f;
	}
}
// ---


// '소지' 
void APlayerCharacter::TalismanRitual(AStationaryAnomaly* Anomaly)
{
	if (bIsTalismanRitualUIOpen || !Anomaly) return;

	// Talisman이 부족할 경우 Text 표시 후 종료
	if (GetAttributes())
	{
		if (GetAttributes()->GetTalisman() < 1.f)
		{
			auto* PC = Cast<APlayerCharacterController>(GetController());
			if (PC)
			{
				PC->ShowAutoText(2.0f, 0);
				PC->UpdateText(TEXT("지방이 부족합니다."), 0);
			}
			return;
		}
	}

	// 게임 일시 정지
	bIsTalismanRitualUIOpen = true;
	auto* PC = Cast<APlayerCharacterController>(GetController());
	if (PC)
	{
		// '지방' UI 표시
		PC->OpenTalismanUI(Anomaly);
	}

}


// '소지' UI가 닫힐 때 (UI의 버튼 등에서 호출)
void APlayerCharacter::FinishTalismanRitual(EAnomalyCategory SelectedCategory)
{
	if (!bIsTalismanRitualUIOpen) return;

	bIsTalismanRitualUIOpen = false;
	auto* PC = Cast<APlayerCharacterController>(GetController());
	if (PC)
	{
		PC->CloseTalismanUI();
	}

	if (SelectedCategory == EAnomalyCategory::EAC_None) return;

	// Talisman Count 감소
	if (GetAttributes())
	{
		float CurrentTalisman = GetAttributes()->GetTalisman();
		GetAttributes()->SetTalisman(CurrentTalisman - 1.f);
	}

	AStationaryAnomaly* Anomaly = Cast<AStationaryAnomaly>(CurrentFocusedInteractable.GetObject());
	if (Anomaly)
	{
		Anomaly->OnTalismanRitualFinished(SelectedCategory);
		CurrentFocusedInteractable = nullptr;
	}

}
// ---


// 도구 사용
void APlayerCharacter::UseTool()
{
	AToolBase* Tool = Cast<AToolBase>(HeldItem);
	if (!Tool) return;

	// 쫓아오는 이상현상에게 사용
	AChasingAnomaly* TargetAnomaly = Cast<AChasingAnomaly>(CurrentFocusedInteractable.GetObject());
	if (TargetAnomaly)
	{
		if (Tool->UseTool(TargetAnomaly)) // ToolBase.cpp의 UseTool 호출
		{
			// 사용 성공 시 손에서 제거
			QuickSlotRef->RemoveItemAt(QuickSlotRef->GetCurrentSlotIndex());
			HeldItem = nullptr;
			Tool->Destroy();
		}
	}
}
// ---


// 플레이어 피해 처리
void APlayerCharacter::HandleDamage(float DamageAmount)
{
	if (bIsInvincible)
	{
		return; // 무적 상태일 때는 데미지를 받지 않음
	}

	if (Attributes)
	{
		Attributes->ReceiveDamage(DamageAmount);

		// 1초간 무적 상태로 만듦
		bIsInvincible = true;
		GetWorld()->GetTimerManager().SetTimer(
			InvincibilityTimerHandle,
			this,
			&APlayerCharacter::ResetInvincibility,
			1.0f,
			false
		);

		// 피격 시각 효과
		// (구현 필요) APlayerCharacterController* PC = Cast<APlayerCharacterController>(GetController());
		// if (PC) { PC->PlayHitEffect(); }

		// 체력 0 이 됐는 지 확인 후 Die() 호출
		if (!Attributes->IsAlive())
		{
			Die();
		}
	}


}


void APlayerCharacter::ResetInvincibility()
{
    bIsInvincible = false;
    OnInvincibilityEnd.Broadcast(this);
}

bool APlayerCharacter::getIsInvincible()
{
	return bIsInvincible;
}