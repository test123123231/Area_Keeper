// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/HUDWidget.h"
#include "Components/TextBlock.h"

void UHUDWidget::UpdateHealth(float CurrentHealth)
{
    if (HealthText) 
    { 
        HealthText->SetText(FText::Format(
            FText::FromString(TEXT("x {0}")),
            FText::AsNumber(CurrentHealth))); 
    }
}

void UHUDWidget::UpdateAmulet(float CurrentAmulet)
{
    if (AmuletText) 
    { 
        AmuletText->SetText(FText::AsNumber(CurrentAmulet)); 
    }
}