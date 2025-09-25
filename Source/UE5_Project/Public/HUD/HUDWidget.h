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
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* HealthText;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* AmuletText;

public:
    UFUNCTION(BlueprintCallable)
    void UpdateHealth(float CurrentHealth);

    UFUNCTION(BlueprintCallable)
    void UpdateAmulet(float CurrentAmulet);
};
