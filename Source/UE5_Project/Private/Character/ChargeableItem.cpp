// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ChargeableItem.h"
#include "Materials/MaterialInstanceDynamic.h"

AChargeableItem::AChargeableItem()
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(false);
}

void AChargeableItem::BeginPlay()
{
    Super::BeginPlay();
}

void AChargeableItem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 충전 중이면 쿨타임 경과 체크
    if (bIsCharged)
    {
        Cooldown += DeltaTime;

        if (Cooldown >= RechargeCooldown)
        {
            bIsCharged = false;
            Cooldown = 0.0f;
            HighlightItem(false);
            UE_LOG(LogTemp, Display, TEXT("충전 쿨타임 끝, 다시 충전 가능"));
			SetActorTickEnabled(false); 
        }
    }
}

void AChargeableItem::OnCharged()
{
	if (bIsCharged) return;
	bIsCharged = true;
	HighlightItem(true);

    // 임시 텍스트 표시, 추후 ui와 연결 예정
	UE_LOG(LogTemp, Display, TEXT("ui 업데이트"));
	SetActorTickEnabled(true);  
}

// 충전용 아이템 하이라이트
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