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
	class UTextBlock* HPText;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* AmuletText;

public:
    UFUNCTION(BlueprintCallable)
    void UpdateHP(int32 CurrentHP);

    UFUNCTION(BlueprintCallable)
    void UpdateAmulet(int32 CurrentAmulet);
};
