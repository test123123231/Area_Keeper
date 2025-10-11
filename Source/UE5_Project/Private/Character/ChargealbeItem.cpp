// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ChargealbeItem.h"
#include "Materials/MaterialInstanceDynamic.h"

AChargeableItem::AChargeableItem()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AChargeableItem::OnCharged()
{
	if (bIsCharged) return;
	bIsCharged = true;

	HighlightItem(true);

    // 임시 텍스트 표시, 추후 ui와 연결 예정
	UE_LOG(LogTemp, Display, TEXT("ui 업데이트"));
}
void AChargeableItem::HighlightItem(bool bOn)
{
    if (DynamicMaterial)
    {
		if(bOn){
			DynamicMaterial->SetVectorParameterValue("Color", FLinearColor::Green);
		}
		else{
			DynamicMaterial->SetVectorParameterValue("Color", FLinearColor::White);
		}
    }
}