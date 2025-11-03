#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameClearWidget.generated.h"


class UButton;
class UTextBlock;


/**
 * 게임 클리어 시 표시되는 위젯의 C++ 기반 클래스
 * C++에서 '메인 메뉴' 버튼 로직을 처리하고, BP에서 디자인을 담당
 */
UCLASS()
class UE5_PROJECT_API UGameClearWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * GameMode가 이 위젯을 생성한 직후 호출하여 최종 게임 결과를 전달합니다.
	 * param PenaltyStack 최종 패널티 스택
	 * param AnomaliesSolved (미래) 해결한 총 이상현상 개수
	 * param MimicEncounters (미래) 흉내쟁이 귀신에게 당한 횟수
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Clear")
	void InitializeWidget(int32 PenaltyStack, int32 AnomaliesSolved, int32 ChasingHits);

protected:
	/**
	 * C++의 OnClicked 이벤트를 BP의 버튼에 바인딩합니다.
	 */
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_MainMenu;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_PenaltyStack;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_AnomaliesSolved;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_ChasingHits;

	//// (Blueprint) BP 디자이너에서 이 이름으로 'TextBlock'을 생성해야 합니다.
	//UPROPERTY(meta = (BindWidget))
	//TObjectPtr<UTextBlock> Text_MimicEncounters;

private:
	/**
	 * '메인 메뉴로' 버튼이 클릭되면 호출됩니다.
	 */
	UFUNCTION()
	void OnMainMenuClicked();

	UPROPERTY(EditDefaultsOnly, Category = "Config")
	FName MainMenuLevelName = FName(TEXT("MainMenu"));
};
