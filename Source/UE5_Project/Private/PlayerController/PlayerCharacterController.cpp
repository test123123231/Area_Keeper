#include "PlayerController/PlayerCharacterController.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Character/QuickSlot.h"   // 너의 QuickSlot 위젯 헤더

void APlayerCharacterController::BeginPlay()
{
    Super::BeginPlay();

    // 로컬 플레이어 서브시스템을 가져옵니다.
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        // 기본 매핑 컨텍스트를 추가합니다.
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
