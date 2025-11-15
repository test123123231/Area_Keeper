#pragma once

#include "CoreMinimal.h"
#include "Item/ItemBase.h"
#include "ChargeableItem.generated.h"

UCLASS()
class UE5_PROJECT_API AChargeableItem : public AItemBase
{
	GENERATED_BODY()

public:
	AChargeableItem();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	// 충전 완료 시 호출
	UFUNCTION(BlueprintCallable, Category = "Charge")
	virtual bool OnCharged();

	// 충전 쿨타임이 도는지 유무
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Charge")
	bool bIsCharged = false;

	// 쿨타임 지속 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Charge")
	float RechargeCooldown = 15.0f;

	// 누적 시간
	float Cooldown = 0.0f;
	virtual FString GetInteractText_Implementation() override;

protected:
	virtual void Highlight_Implementation(bool bOn) override;

};