// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "BaseCharacter.generated.h"


class UInputAction;


UCLASS()
class UE5_PROJECT_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

	// �Է� �׼ǵ��� ���� ������
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bind")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bind")
	TObjectPtr<UInputAction> LookAction;

	// �Է� ó�� �Լ���
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

private:
	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* ViewCamera;
	
};
