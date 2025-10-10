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

	// �Է� ó�� �Լ���
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);


	// ���� �ٶ󺸴� ������(���⼭���� �߰�)
	AItemBase* CurrentFocusedItem;

	// ������ ���� (����Ʈ���̽�)
	void TraceForItems();

	void Interact();

	void PickupItem(AItemBase* Item); //������ ����
	void ChangeItem(AItemBase* Item, const FVector& Location); //������ �ٲٱ�
	void DropHeldItem();   // ������ ������

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* DropAction;  // GŰ �Է� �׼�

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* InteractAction;

	// �տ� ��� �ִ� ������
	UPROPERTY(VisibleAnywhere, Category = "Item")
	AItemBase* HeldItem;

	// �տ� ������ ���� ���� �̸�
	UPROPERTY(EditDefaultsOnly, Category = "Item")
	FName HandSocketName = "RightHandSocket";

	UQuickSlot* QuickSlotRef; // UI ���� ����
};