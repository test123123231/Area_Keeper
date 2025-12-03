#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "InputActionValue.h"
#include "Anomaly/AnomalyTypes.h"
#include "PlayerCharacter.generated.h"


class UQuickSlot;
class AItemBase;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class USpotLightComponent;
class IInteractableInterface; // 인터페이스 참조
class AAreaKeeperGameState;
class APlayerCharacterController;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInvincibilityEnd, APlayerCharacter*, Player);

UCLASS()
class UE5_PROJECT_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 아이템 줍기 로직 (ItemBase가 호출)/
	void PickupItem(AItemBase* Item);

	// ABaseCharacter::Die() 오버라이드. 플레이어 입력 정지용/
	virtual void Die() override;

	virtual void HandleDamage(float DamageAmount) override;
	bool getIsInvincible();
	UPROPERTY(BlueprintAssignable, Category="State")
	FOnInvincibilityEnd OnInvincibilityEnd;

	UCameraComponent* GetViewCamera() const { return ViewCamera; }

protected:
	virtual void BeginPlay() override;

	// 플레이어 1초 무적 시간
	bool bIsInvincible;
	FTimerHandle InvincibilityTimerHandle;
	void ResetInvincibility();
	// ---

	// GameState 캐시
	UPROPERTY()
	TWeakObjectPtr<AAreaKeeperGameState> GameStateRef;

	// 컴포넌트
	/*UPROPERTY(VisibleAnywhere, Category = "Camera")
	USpringArmComponent* SpringArm;*/

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	UCameraComponent* ViewCamera;

	UPROPERTY(VisibleAnywhere, Category = "Light")
	USpotLightComponent* FlashLightComponent;

	// 입력 액션
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* LookAction;

	// E키 (줍기, 사용하기, 소지하기, 충전하기)
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* InteractAction;

	// G키 (버리기)
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* DropAction;

	// F키 (손전등)
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* FlashlightAction;

	// Ctrl키 (웅크리기) 
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* CrouchAction;

	// 기본 서 있을 때의 카메라 높이
	float DefaultCameraHeight;

	// 카메라의 '현재 높이'를 기억할 변수
	float CurrentCamHeight;


	// 입력 처리 함수 
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	/*void StartCrouch();
	void StopCrouch();*/

	// 상호작용 및 아이템
private:
	// 상호작용(E) 키를 누를 때 호출되는 마스터 함수
	void OnInteractPressed();
	// 상호작용(E) 키를 뗄 때 호출되는 마스터 함수 
	void OnInteractReleased();
	// 아이템 버리기(G) 
	void OnDropItem();
	// flashlight 토글
	void OnFlashlightPressed();
	// 웅크리기 토글
	void OnCrouchPressed();

	// 매 틱마다 상호작용 가능한 객체를 찾음
	void TraceForInteractable();

	void UpdateInteractionPrompt(TScriptInterface<IInteractableInterface>& HitInteractable);

	// 현재 바라보고 있는 상호작용 가능한 객체
	UPROPERTY()
	TScriptInterface<IInteractableInterface> CurrentFocusedInteractable;

	// 아이템 교체/내려놓기
	void ChangeItem(AItemBase* Item, const FVector& Location);

	// 손에 들고 있는 아이템 (AItemBase 또는 AToolBase)
	UPROPERTY(VisibleAnywhere, Category = "Item")
	AItemBase* HeldItem;

	UPROPERTY(EditDefaultsOnly, Category = "Item")
	FName HandSocketName = "RightHandSocket";

	// 퀵슬롯
	UPROPERTY()
	UQuickSlot* QuickSlotRef; // UI 위젯 참조

public:
	void SetQuickSlotRef(UQuickSlot* NewRef);
	void SelectQuickSlot(int32 SlotIndex);

	// '소지' (이상현상 해결) 로직 
	void TalismanRitual(class AStationaryAnomaly* Anomaly);

	// '소지' UI가 닫힐 때 (UI의 OnClose 버튼 등에서 호출)
	UFUNCTION(BlueprintCallable, Category = "TalismanRitual")
	void FinishTalismanRitual(EAnomalyCategory SelectedCategory);

	// '지방' 충전 로직
private:
	bool bIsCharging = false;
	float ChargeTime = 0.0f;
	UPROPERTY(EditAnywhere, Category = "Charge")
	float RequiredChargeTime = 2.0f; // E키 홀드 시간

	TWeakObjectPtr<class AChargeableItem> ChargingTarget;

	UPROPERTY()
	TObjectPtr<APlayerCharacterController> PlayerControllerRef;

	// 소지 UI가 열려있는지 여부, 틱 및 입력 차단용
	bool bIsTalismanRitualUIOpen = false;

	// 도구 사용 로직 
	void UseTool();

public:
	void StartCharge(AChargeableItem* Target);
	void StopCharge();
	void HandleCharging(float DeltaTime);

};

