#include "PlayerController/PlayerCharacterController.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "HUD/HUDWidget.h"   
#include "Components/AttributeComponent.h"
#include "GameFramework/Pawn.h"


void APlayerCharacterController::BeginPlay()
{
    Super::BeginPlay();

    // ���� �÷��̾� ����ý����� �����ɴϴ�.
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        // �⺻ ���� ���ؽ�Ʈ�� �߰��մϴ�.
        Subsystem->AddMappingContext(DefaultMappingContext, 0);
    }

    if (HUDWidgetClass)
    {
        HUDRef = CreateWidget<UHUDWidget>(this, HUDWidgetClass);
        if (HUDRef && BoundAttribute)
        {
            HUDRef->AddToViewport();
        }
    }

    BindToPawnDelegates(GetPawn());
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

