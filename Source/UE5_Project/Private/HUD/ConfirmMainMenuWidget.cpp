#include "HUD/ConfirmMainMenuWidget.h"
#include "Components/Button.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"


void UConfirmMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 플레이어 컨트롤러 캐시
	PlayerControllerRef = GetOwningPlayer();

	// 버튼 바인딩
	if (Button_Yes)
	{
		Button_Yes->OnClicked.AddDynamic(this, &UConfirmMainMenuWidget::OnYesClicked);
	}
	if (Button_No)
	{
		Button_No->OnClicked.AddDynamic(this, &UConfirmMainMenuWidget::OnNoClicked);
	}
}


// 이 위젯을 연 부모 메뉴(PauseMenu)를 설정
void UConfirmMainMenuWidget::SetParentMenu(UUserWidget* InParentMenu)
{
	ParentMenu = InParentMenu;
}


// '예' (메인 메뉴로 이동) 버튼 클릭 시
void UConfirmMainMenuWidget::OnYesClicked()
{
	if (PlayerControllerRef)
	{
		// 게임 일시정지를 해제
		PlayerControllerRef->SetPause(false);

		// 입력 모드를 게임 전용으로 되돌리고 마우스를 숨김
		FInputModeGameOnly InputModeData;
		PlayerControllerRef->SetInputMode(InputModeData);
		PlayerControllerRef->bShowMouseCursor = false;
	}

	// 메인 메뉴 레벨을 로드
	UGameplayStatics::OpenLevel(this, MainMenuLevelName);
}


// '아니오' (취소) 버튼 클릭 시
void UConfirmMainMenuWidget::OnNoClicked()
{
	CloseMenu();
}


// 위젯을 닫고 부모 메뉴로 복귀
void UConfirmMainMenuWidget::CloseMenu()
{
	// 부모 메뉴(PauseMenu)가 있다면 다시 보이게 함
	if (ParentMenu)
	{
		ParentMenu->SetVisibility(ESlateVisibility::Visible);

		// 부모 위젯(PauseMenu)으로 돌아가기 위해 GameAndUI 모드로 '복원'
		if (PlayerControllerRef)
		{
			FInputModeGameAndUI InputModeData;
			InputModeData.SetWidgetToFocus(ParentMenu->TakeWidget());
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::LockInFullscreen);
			PlayerControllerRef->SetInputMode(InputModeData);
		}
	}

	// 자신을 뷰포트에서 제거
	RemoveFromParent();
}
