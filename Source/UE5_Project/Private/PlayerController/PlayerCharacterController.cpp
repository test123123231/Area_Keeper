#include "PlayerController/PlayerCharacterController.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "HUD/HUDWidget.h"   



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
        if (HUDRef)
        {
            HUDRef->AddToViewport();
        }
    }
}


