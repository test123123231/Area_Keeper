#include "PlayerController/PlayerCharacterController.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
<<<<<<< Updated upstream
#include "Character/QuickSlot.h"   // QuickSlot ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½
=======
#include "Character/QuickSlot.h"   // QuickSlot À§Á¬ Çì´õ
>>>>>>> Stashed changes
#include "Character/PlayerCharacter.h"

void APlayerCharacterController::BeginPlay()
{
    Super::BeginPlay();

    // ï¿½ï¿½ï¿½ï¿½ ï¿½Ã·ï¿½ï¿½Ì¾ï¿½ ï¿½ï¿½ï¿½ï¿½Ã½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½É´Ï´ï¿½.
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        // ï¿½âº» ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Ø½ï¿½Æ®ï¿½ï¿½ ï¿½ß°ï¿½ï¿½Õ´Ï´ï¿½.
        Subsystem->AddMappingContext(DefaultMappingContext, 0);
    }

    if (QuickSlotWidgetClass)
    {
        QuickSlotWidget = CreateWidget<UQuickSlot>(this, QuickSlotWidgetClass);
        if (QuickSlotWidget)
        {
            QuickSlotWidget->AddToViewport();

<<<<<<< Updated upstream
            // PlayerCharacterï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
=======
            // PlayerCharacter¿Í ¿¬°á
>>>>>>> Stashed changes
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

<<<<<<< Updated upstream
    // ï¿½ï¿½ï¿½ï¿½ Å° ï¿½ï¿½ï¿½Îµï¿½
=======
    // ¼ýÀÚ Å° ¹ÙÀÎµù
>>>>>>> Stashed changes
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