// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE5_PROJECT_API UHUDWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	// Health를 표시하는 텍스트
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* HealthText;

	// Amulet를 표시하는 텍스트
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* AmuletText;

public:
	// HealthText를 업데이트한다
    UFUNCTION(BlueprintCallable)
    void UpdateHealth(float CurrentHealth);

	// AmuletText를 업데이트한다
    UFUNCTION(BlueprintCallable)
    void UpdateAmulet(float CurrentAmulet);
};
