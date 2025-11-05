#include "HUD/PauseMenuWidget.h"
#include "Components/Button.h"
#include "PlayerController/PlayerCharacterController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Blueprint/UserWidget.h"
#include "HUD/OptionMenuWidget.h"
#include "HUD/ControlsMenuWidget.h"
#include "HUD/ConfirmMainMenuWidget.h"


void UPauseMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 플레이어 컨트롤러 참조 캐시
	PlayerControllerRef = Cast<APlayerCharacterController>(GetOwningPlayer());

	// 함수와 BP 버튼의 OnClicked 이벤트를 바인딩
	if (Button_Resume)
	{
		Button_Resume->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnResumeClicked);
	}
	if (Button_Option)
	{
		Button_Option->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnOptionClicked);
	}
	if (Button_Controls)
	{
		Button_Controls->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnControlsClicked);
	}
	if (Button_MainMenu)
	{
		Button_MainMenu->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnMainMenuClicked);
	}
	if (Button_Exit)
	{
		Button_Exit->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnExitClicked);
	}
}


// '계속하기' 버튼 로직
void UPauseMenuWidget::OnResumeClicked()
{
	// APlayerCharacterController에 구현된 ClosePauseMenu 함수를 호출
	if (PlayerControllerRef)
	{
		PlayerControllerRef->ClosePauseMenu();
	}
}


// '설정' - 옵션 메뉴 열기
void UPauseMenuWidget::OnOptionClicked()
{
	if (!OptionMenuWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("PauseMenuWidget: OptionMenuWidgetClass가 설정되지 않았습니다!"));
		return;
	}

	// 옵션 메뉴 '인스턴스' 생성
	if (!OptionMenuInstance)
	{
		OptionMenuInstance = CreateWidget<UOptionMenuWidget>(this, OptionMenuWidgetClass);
		if (!OptionMenuInstance) return; // 생성 실패 시 중단

		// 옵션 메뉴에 부모(자신)를 알려줌
		OptionMenuInstance->SetParentMenu(this);
	}

	// 뷰포트에 추가 및 포커스 설정
	if (OptionMenuInstance && !OptionMenuInstance->IsInViewport())
	{
		OptionMenuInstance->AddToViewport();

		if (PlayerControllerRef)
		{
			// (GameAndUI) 옵션 메뉴는 ESC 키를 사용하므로 포커스 설정
			FInputModeGameAndUI InputModeData;
			InputModeData.SetWidgetToFocus(OptionMenuInstance->TakeWidget());
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::LockInFullscreen);
			PlayerControllerRef->SetInputMode(InputModeData);
		}
		SetVisibility(ESlateVisibility::Hidden);
	}
}


// '조작법' - 조작법 메뉴 열기
void UPauseMenuWidget::OnControlsClicked()
{
	if (!ControlsMenuWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("PauseMenuWidget: ControlsMenuWidgetClass가 설정되지 않았습니다!"));
		return;
	}

	// 조작법 메뉴 '인스턴스' 생성
	if (!ControlsMenuInstance)
	{
		ControlsMenuInstance = CreateWidget<UControlsMenuWidget>(this, ControlsMenuWidgetClass);
		if (!ControlsMenuInstance) return; // 생성 실패 시 중단

		// 조작법 메뉴에 부모(자신)를 알려줌
		ControlsMenuInstance->SetParentMenu(this);
	}

	// 뷰포트에 추가 및 포커스 설정
	if (ControlsMenuInstance && !ControlsMenuInstance->IsInViewport())
	{
		ControlsMenuInstance->AddToViewport();

		if (PlayerControllerRef)
		{
			// (GameAndUI) 조작법 메뉴는 ESC 키를 사용하므로 포커스 설정
			FInputModeGameAndUI InputModeData;
			InputModeData.SetWidgetToFocus(ControlsMenuInstance->TakeWidget());
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::LockInFullscreen);
			PlayerControllerRef->SetInputMode(InputModeData);
		}
		SetVisibility(ESlateVisibility::Hidden);
	}
}


// '메인 메뉴로' - 확인 팝업 열기
void UPauseMenuWidget::OnMainMenuClicked()
{
	if (!ConfirmMainMenuWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("PauseMenuWidget: ConfirmMainMenuWidgetClass가 설정되지 않았습니다!"));
		return;
	}

	// 팝업 메뉴 '인스턴스' 생성
	if (!ConfirmMainMenuPopupInstance)
	{
		ConfirmMainMenuPopupInstance = CreateWidget<UConfirmMainMenuWidget>(this, ConfirmMainMenuWidgetClass);
		if (!ConfirmMainMenuPopupInstance) return; // 생성 실패 시 중단

		// 팝업 메뉴에 부모(자신)를 알려줌
		ConfirmMainMenuPopupInstance->SetParentMenu(this);
	}

	// 뷰포트에 추가 및 포커스 설정
	if (ConfirmMainMenuPopupInstance && !ConfirmMainMenuPopupInstance->IsInViewport())
	{
		ConfirmMainMenuPopupInstance->AddToViewport();

		if (PlayerControllerRef)
		{
			// [수정] (UIOnly) 이 팝업은 '마우스 전용'이므로 포커스를 설정하지 않습니다.
			FInputModeUIOnly InputModeData;
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::LockInFullscreen);
			PlayerControllerRef->SetInputMode(InputModeData);
		}
	}
}


// '게임 종료' 버튼 로직
void UPauseMenuWidget::OnExitClicked()
{
	// 게임 애플리케이션을 종료
	UKismetSystemLibrary::QuitGame(this, PlayerControllerRef, EQuitPreference::Quit, true);
}
