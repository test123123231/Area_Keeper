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

void UHUDWidget::ShowCenterText()
{
    if (CenterText)
    {
        CenterText->SetVisibility(ESlateVisibility::Visible);
    }
}

void UHUDWidget::HideCenterText()
{
    if (CenterText)
    {
        CenterText->SetVisibility(ESlateVisibility::Hidden);
    }
}

void UHUDWidget::UpdateCenterText(const FString& Text)
{
    if (CenterText)
    {
        CenterText->SetText(FText::FromString(Text));
    }
}

void UHUDWidget::ShowTimeText()
{
    if (TimeText)
    {
        TimeText->SetVisibility(ESlateVisibility::Visible);
    }
}

void UHUDWidget::HideTimeText()
{
    if (TimeText)
    {
        TimeText->SetVisibility(ESlateVisibility::Hidden);
    }
}

void UHUDWidget::UpdateTimeText(const FString& Text)
{
    if (TimeText)
    {
        TimeText->SetText(FText::FromString(Text));
    }
}