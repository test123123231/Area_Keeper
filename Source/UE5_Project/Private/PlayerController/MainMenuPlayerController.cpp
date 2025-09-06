#include "PlayerController/MainMenuPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"


void AMainMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();
	// 마우스 커서 표시
	bShowMouseCursor = true;
	// 로컬 플레이어 서브시스템을 가져옵니다.
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		// 기본 매핑 컨텍스트를 추가합니다.
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	// MainMenuWidgetClass 변수에 유효한 위젯 클래스가 할당되었는지 확인합니다.
	if (MainMenuWidgetClass)
	{
		// 위젯 인스턴스를 생성하고 MainMenuWidgetInstance 변수에 저장합니다.
		MainMenuWidgetInstance = CreateWidget<UUserWidget>(this, MainMenuWidgetClass);

		// 위젯 생성이 성공했는지 다시 한번 확인합니다.
		if (MainMenuWidgetInstance)
		{
			// 생성된 위젯을 뷰포트에 추가하여 화면에 표시합니다.
			MainMenuWidgetInstance->AddToViewport();
		}
	}
}