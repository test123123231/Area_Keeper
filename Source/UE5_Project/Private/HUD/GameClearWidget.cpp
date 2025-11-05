#include "HUD/GameClearWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"


void UGameClearWidget::InitializeWidget(int32 PenaltyStack, int32 AnomaliesSolved, int32 ChasingHits)
{
	// GameMode로부터 받은 값으로 텍스트를 설정합니다.
	if (Text_PenaltyStack)
	{
		Text_PenaltyStack->SetText(FText::AsNumber(PenaltyStack));
	}
	if (Text_AnomaliesSolved)
	{
		Text_AnomaliesSolved->SetText(FText::AsNumber(AnomaliesSolved));
	}
	if (Text_ChasingHits)
	{
		Text_ChasingHits->SetText(FText::AsNumber(ChasingHits));
	}
	/*if (Text_MimicEncounters)
	{
		Text_MimicEncounters->SetText(FText::AsNumber(MimicEncounters));
	}*/
}


void UGameClearWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// C++ 함수와 BP 버튼의 OnClicked 이벤트를 연결합니다.
	if (Button_MainMenu)
	{
		Button_MainMenu->OnClicked.AddDynamic(this, &UGameClearWidget::OnMainMenuClicked);
	}
}


void UGameClearWidget::OnMainMenuClicked()
{
	UE_LOG(LogTemp, Log, TEXT("GameClearWidget: '메인 메뉴로' 버튼 클릭됨"));

	// TODO: "MainMenu" 부분을 실제 메인 메뉴 레벨의 이름으로 변경해야 합니다.
	UGameplayStatics::OpenLevel(this, MainMenuLevelName);
}
