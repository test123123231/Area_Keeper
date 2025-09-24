// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/HUDWidget.h"
#include "Components/TextBlock.h"

void UHUDWidget::UpdateHP(int32 CurrentHP)
{
    if (HPText) 
    { 
        HPText->SetText(FText::AsNumber(CurrentHP)); 
    }
}

void UHUDWidget::UpdateAmulet(int32 CurrentAmulet)
{
    if (AmuletText) 
    { 
        AmuletText->SetText(FText::AsNumber(CurrentAmulet)); 
    }
}