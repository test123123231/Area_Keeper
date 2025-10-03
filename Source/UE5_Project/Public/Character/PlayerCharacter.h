#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "InputActionValue.h"
#include "PlayerCharacter.generated.h"
class AItemBase;

UCLASS()
class UE5_PROJECT_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* ViewCamera;

	// 입력 처리 함수들
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);




	// 현재 바라보는 아이템(여기서부터 추가)
	AItemBase* CurrentFocusedItem;

	// 아이템 감지 (라인트레이스)
	void TraceForItems();

	// E키 -> 아이템 줍기
	void Interact();

	void PickupItem(AItemBase* Item);
	void ChangeItem(AItemBase* Item, const FVector& Location);
	void DropHeldItem();   // G키로 손에 든 아이템 버리기

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* DropAction;  // G키 입력 액션

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* InteractAction;

	// 손에 들고 있는 아이템
	UPROPERTY(VisibleAnywhere, Category = "Item")
	AItemBase* HeldItem;

	// 손에 아이템 붙일 소켓 이름
	UPROPERTY(EditDefaultsOnly, Category = "Item")
	FName HandSocketName = "RightHandSocket";
};
