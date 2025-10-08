#include "PlayerController/PlayerCharacterController.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Character/QuickSlot.h"   // ���� QuickSlot ���� ���

void APlayerCharacterController::BeginPlay()
{
    Super::BeginPlay();

    // ���� �÷��̾� ����ý����� �����ɴϴ�.
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        // �⺻ ���� ���ؽ�Ʈ�� �߰��մϴ�.
        Subsystem->AddMappingContext(DefaultMappingContext, 0);
    }

    if (QuickSlotWidgetClass)
    {
        QuickSlotWidget = CreateWidget<UQuickSlot>(this, QuickSlotWidgetClass);
        if (QuickSlotWidget)
        {
            QuickSlotWidget->AddToViewport();
            UE_LOG(LogTemp, Log, TEXT("QuickSlot UI added to viewport."));
        }
    }
}
