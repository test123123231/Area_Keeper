#include "PlayerController/MainMenuPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"


void AMainMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();
	// ���콺 Ŀ�� ǥ��
	bShowMouseCursor = true;
	// ���� �÷��̾� ����ý����� �����ɴϴ�.
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		// �⺻ ���� ���ؽ�Ʈ�� �߰��մϴ�.
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	// MainMenuWidgetClass ������ ��ȿ�� ���� Ŭ������ �Ҵ�Ǿ����� Ȯ���մϴ�.
	if (MainMenuWidgetClass)
	{
		// ���� �ν��Ͻ��� �����ϰ� MainMenuWidgetInstance ������ �����մϴ�.
		MainMenuWidgetInstance = CreateWidget<UUserWidget>(this, MainMenuWidgetClass);

		// ���� ������ �����ߴ��� �ٽ� �ѹ� Ȯ���մϴ�.
		if (MainMenuWidgetInstance)
		{
			// ������ ������ ����Ʈ�� �߰��Ͽ� ȭ�鿡 ǥ���մϴ�.
			MainMenuWidgetInstance->AddToViewport();
		}
	}
}