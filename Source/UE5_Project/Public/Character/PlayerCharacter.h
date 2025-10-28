#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "InputActionValue.h"
#include "PlayerCharacter.generated.h"
class UQuickSlot;
class AItemBase;
UCLASS()
class UE5_PROJECT_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SetQuickSlotRef(UQuickSlot* NewRef);
	void SelectQuickSlot(int32 SlotIndex);
protected:
	virtual void BeginPlay() override;


private:
	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* ViewCamera;

	// ÏûÖÎ†• Ï≤òÎ¶¨ Ìï®ÏàòÎì§
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);


	// «ˆ¿Á πŸ∂Û∫∏¥¬ æ∆¿Ã≈€(ø©±‚º≠∫Œ≈Õ √ﬂ∞°)
	AItemBase* CurrentFocusedItem;

	// æ∆¿Ã≈€ ∞®¡ˆ (∂Û¿Œ∆Æ∑π¿ÃΩ∫)
	void TraceForItems();

	void Interact();

	void PickupItem(AItemBase* Item); //æ∆¿Ã≈€ ¡˝±‚
	void ChangeItem(AItemBase* Item, const FVector& Location); //æ∆¿Ã≈€ πŸ≤Ÿ±‚
	void DropHeldItem();   // æ∆¿Ã≈€ πˆ∏Æ±‚

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* DropAction;  // G≈∞ ¿‘∑¬ æ◊º«

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* InteractAction;

	// º’ø° µÈ∞Ì ¿÷¥¬ æ∆¿Ã≈€
	UPROPERTY(VisibleAnywhere, Category = "Item")
	AItemBase* HeldItem;

	// º’ø° æ∆¿Ã≈€ ∫Ÿ¿œ º“ƒœ ¿Ã∏ß
	UPROPERTY(EditDefaultsOnly, Category = "Item")
	FName HandSocketName = "RightHandSocket";

	UQuickSlot* QuickSlotRef; // UI ¿ß¡¨ ¬¸¡∂
};