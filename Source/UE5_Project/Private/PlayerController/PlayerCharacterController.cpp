#include "PlayerController/PlayerCharacterController.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "HUD/HUDWidget.h"   
#include "Components/AttributeComponent.h"
#include "GameFramework/Pawn.h"


void APlayerCharacterController::BeginPlay()
{
    Super::BeginPlay();

    // 로컬 플레이어 서브시스템을 가져옵니다.
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        // 기본 매핑 컨텍스트를 추가합니다.
        Subsystem->AddMappingContext(DefaultMappingContext, 0);
    }
    
    if (!HUDRef && HUDWidgetClass)
    {
        HUDRef = CreateWidget<UHUDWidget>(this, HUDWidgetClass);
        if (HUDRef)
        {
            HUDRef->AddToViewport();
        }
    }

    BindToPawnDelegates(GetPawn());
    if(HUDRef && BoundAttribute)
    {
        HUDRef -> UpdateHealth(BoundAttribute -> GetHelath());
        HUDRef -> UpdateAmulet(BoundAttribute -> GetAmulet());
    }
}

//possess
void APlayerCharacterController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    BindToPawnDelegates(InPawn);
}

void APlayerCharacterController::OnUnPossess()
{
    UnbindFromPawnDelegates();
    Super::OnUnPossess();
}

//bind
void APlayerCharacterController::BindToPawnDelegates(APawn* InPawn)
{
    UnbindFromPawnDelegates();
    if(!InPawn){ return; }
    if(UAttributeComponent* Attr = InPawn -> FindComponentByClass<UAttributeComponent>())
    {
        BoundAttribute = Attr;
        Attr -> OnHealthChanged.AddDynamic(this, &APlayerCharacterController::HandleHealthChanged);
        Attr -> OnAmuletChanged.AddDynamic(this, &APlayerCharacterController::HandleAmuletChanged);
    }
}

void APlayerCharacterController::UnbindFromPawnDelegates()
{
	if (BoundAttribute)
	{
		BoundAttribute->OnHealthChanged.RemoveDynamic(this, &APlayerCharacterController::HandleHealthChanged);
        BoundAttribute->OnHealthChanged.RemoveDynamic(this, &APlayerCharacterController::HandleAmuletChanged);
		BoundAttribute = nullptr;
	}
}


//handle
void APlayerCharacterController::HandleHealthChanged(float NewHealth)
{
    if(HUDRef)
    {
        HUDRef->UpdateHealth(NewHealth);
    }
}

void APlayerCharacterController::HandleAmuletChanged(float NewAmulet)
{
    if(HUDRef)
    {
        HUDRef ->UpdateAmulet(NewAmulet);
    }
}

