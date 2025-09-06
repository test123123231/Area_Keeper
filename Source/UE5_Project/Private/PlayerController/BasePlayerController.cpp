#include "PlayerController/BasePlayerController.h"
#include "EnhancedInputSubsystems.h"


void ABasePlayerController::BeginPlay()
{
    Super::BeginPlay();

    // 로컬 플레이어 서브시스템을 가져옵니다.
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        // 기본 매핑 컨텍스트를 추가합니다.
        Subsystem->AddMappingContext(DefaultMappingContext, 0);
    }
}
