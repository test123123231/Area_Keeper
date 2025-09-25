#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"


class UInputAction;
class UAttributeComponent;


UCLASS()
class UE5_PROJECT_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	// 입력 액션들을 담을 변수들
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bind")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bind")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* Attributes;
};
