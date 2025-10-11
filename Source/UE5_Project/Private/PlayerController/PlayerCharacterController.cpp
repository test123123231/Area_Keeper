#include "PlayerController/PlayerCharacterController.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Character/QuickSlot.h"   // QuickSlot ���� ���
#include "Character/PlayerCharacter.h"

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

            // PlayerCharacter�� ����
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

    // ���� Ű ���ε�
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