#include "PlayerController/PlayerCharacterController.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Character/QuickSlot.h"   // QuickSlot 위젯 헤더
#include "Character/PlayerCharacter.h"

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

            // PlayerCharacter와 연결
            APlayerCharacter* PlayerChar1 = Cast<APlayerCharacter>(GetPawn());
            if (PlayerChar1)
            {
                PlayerChar1->SetQuickSlotRef(QuickSlotWidget);
            }

            UE_LOG(LogTemp, Log, TEXT("QuickSlot UI added to viewport."));
        }
    }
}


void APlayerCharacterController::SetupInputComponent()
{
    Super::SetupInputComponent();

    // 숫자 키 바인딩
    InputComponent->BindKey(EKeys::One, IE_Pressed, this, &APlayerCharacterController::SelectSlot1);
    InputComponent->BindKey(EKeys::Two, IE_Pressed, this, &APlayerCharacterController::SelectSlot2);
}

void APlayerCharacterController::SelectSlot1()
{
    APlayerCharacter* PlayerChar2 = Cast<APlayerCharacter>(GetPawn());
    if (PlayerChar2)
        PlayerChar2->SelectQuickSlot(0);
}

void APlayerCharacterController::SelectSlot2()
{
    APlayerCharacter* PlayerChar3 = Cast<APlayerCharacter>(GetPawn());
    if (PlayerChar3)
        PlayerChar3->SelectQuickSlot(1);
}