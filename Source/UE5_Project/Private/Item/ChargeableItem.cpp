#include "Item/ChargeableItem.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "PlayerController/PlayerCharacterController.h"
#include "Character/PlayerCharacter.h" 

AChargeableItem::AChargeableItem()
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(false);
}

void AChargeableItem::BeginPlay()
{
    Super::BeginPlay();
    if (DynamicMaterial)
    {
        DynamicMaterial->SetVectorParameterValue("Color", FLinearColor(0.f, 0.f, 0.f, 1.f));
    }
}

void AChargeableItem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsCharged)
    {
        Cooldown += DeltaTime;

        if (Cooldown >= RechargeCooldown)
        {
            bIsCharged = false;
            Cooldown = 0.0f;
            Highlight_Implementation(false);
            UE_LOG(LogTemp, Display, TEXT("충전 쿨타임 끝, 다시 충전 가능"));
			SetActorTickEnabled(false); 
        }
    }
}

// 충전 로직
bool AChargeableItem::OnCharged()
{
	if (bIsCharged) 
        return false;
	bIsCharged = true;
    Cooldown = 0.0f;
    Highlight_Implementation(true);
	SetActorTickEnabled(true);  
    return true;
}

// 충전용 아이템 하이라이트
void AChargeableItem::Highlight_Implementation(bool bOn)
{
    if (DynamicMaterial)
    {
		if(bOn){
			DynamicMaterial->SetVectorParameterValue("Color", FLinearColor(0.5f, 3.0f, 0.5f, 1.0f));
		}
		else{
            DynamicMaterial->SetVectorParameterValue("Color", FLinearColor(0.f, 0.f, 0.f, 1.f));
		}
    }
}

FString AChargeableItem::GetInteractText_Implementation()
{
    return FString(TEXT("충전하기"));
}

void AChargeableItem::Interact_Implementation(APlayerCharacter* Interactor)
{
    if(!Interactor) return;
    
    Interactor -> StartCharge(this);
}