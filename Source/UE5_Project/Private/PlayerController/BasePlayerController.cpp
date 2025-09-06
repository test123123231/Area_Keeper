#include "PlayerController/BasePlayerController.h"
#include "EnhancedInputSubsystems.h"


void ABasePlayerController::BeginPlay()
{
    Super::BeginPlay();

    // ���� �÷��̾� ����ý����� �����ɴϴ�.
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        // �⺻ ���� ���ؽ�Ʈ�� �߰��մϴ�.
        Subsystem->AddMappingContext(DefaultMappingContext, 0);
    }
}
