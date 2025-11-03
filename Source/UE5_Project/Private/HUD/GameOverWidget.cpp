#include "HUD/GameOverWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"


void UGameOverWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// C++ 함수와 BP 버튼을 바인딩

	if (Button_Retry)
	{
		Button_Retry->OnClicked.AddDynamic(this, &UGameOverWidget::OnRetryClicked);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GameOverWidget: 'Button_Retry'를 찾을 수 없습니다. (BindWidget 실패)"));
	}

	if (Button_MainMenu)
	{
		Button_MainMenu->OnClicked.AddDynamic(this, &UGameOverWidget::OnMainMenuClicked);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GameOverWidget: 'Button_MainMenu'를 찾을 수 없습니다. (BindWidget 실패)"));
	}
}


// 게임오버 사유를 UI 텍스트로 설정
void UGameOverWidget::InitializeWidget(bool bPlayerDied)
{
	if (Text_GameOverReason)
	{
		if (bPlayerDied)
		{
			Text_GameOverReason->SetText(FText::FromString(TEXT("사망 (체력 0)")));
		}
		else
		{
			Text_GameOverReason->SetText(FText::FromString(TEXT("이상현상 누적 (패널티 5)")));
		}
	}
}


// '재도전' 버튼 로직: 현재 레벨을 다시 로드
void UGameOverWidget::OnRetryClicked()
{
	// 플레이어 컨트롤러를 가져옴
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		// 마우스 커서를 숨기고 입력을 게임으로 복원
		PC->bShowMouseCursor = false;
		PC->SetInputMode(FInputModeGameOnly());
	}

	// 현재 레벨의 이름을 가져와서 다시 로드
	FName CurrentLevelName = FName(*GetWorld()->GetName());
	UGameplayStatics::OpenLevel(this, CurrentLevelName, true);
}


// '메인 메뉴로' 버튼 로직: 지정된 메인 메뉴 레벨을 로드
void UGameOverWidget::OnMainMenuClicked()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		PC->bShowMouseCursor = false;
		PC->SetInputMode(FInputModeGameOnly());
	}

	UGameplayStatics::OpenLevel(this, MainMenuLevelName, true);
}
