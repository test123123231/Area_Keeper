#include "HUD/ControlsMenuWidget.h"
#include "Components/Button.h"
#include "GameFramework/PlayerController.h"
#include "Framework/Application/SlateApplication.h"


void UControlsMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	bIsFocusable = true; // 위젯을 포커스 가능하게 설정

	// 플레이어 컨트롤러 캐시
	PlayerControllerRef = GetOwningPlayer();

	// '뒤로가기' 버튼 바인딩
	if (Button_Back)
	{
		Button_Back->OnClicked.AddDynamic(this, &UControlsMenuWidget::OnBackClicked);
	}
}


// 이 위젯을 연 부모 메뉴(PauseMenu)를 설정
void UControlsMenuWidget::SetParentMenu(UUserWidget* InParentMenu)
{
	ParentMenu = InParentMenu;
}


// '뒤로가기' 버튼 클릭 시
void UControlsMenuWidget::OnBackClicked()
{
	CloseMenu();
}


// ESC 키 입력 처리
FReply UControlsMenuWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Escape)
	{
		CloseMenu();
		return FReply::Handled(); // ESC 키 입력을 처리했음
	}
	return FReply::Unhandled();
}


// 배경 클릭 시 포커스 유지
FReply UControlsMenuWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// 부모 로직(버튼 클릭 등)을 먼저 실행
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	// 이 위젯에 다시 키보드 포커스를 강제로 설정
	if (PlayerControllerRef)
	{
		TSharedPtr<SWidget> SafeWidget = this->TakeWidget();
		if (SafeWidget.IsValid())
		{
			FSlateApplication::Get().SetKeyboardFocus(SafeWidget);
		}
	}

	// 클릭 이벤트를 처리했음(Handled)으로 반환하여 뷰포트로 전파되는 것을 막음
	return FReply::Handled();
}


// 위젯을 닫고 부모 메뉴로 복귀
void UControlsMenuWidget::CloseMenu()
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
