// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/ItemBase.h"
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
	UFUNCTION(BlueprintCallable, Category="Charge")
	virtual void OnCharged();

	// 이미 충전되었는지 여부
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category="Charge")
	bool bIsCharged = false;

	// 쿨타임 지속 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Charge")
    float RechargeCooldown = 15.0f;

	 // 누적 시간
	 float Cooldown = 0.0f;

protected:
    virtual void HighlightItem(bool bOn) override;
};