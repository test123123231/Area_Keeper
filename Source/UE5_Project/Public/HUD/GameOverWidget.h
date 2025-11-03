#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameOverWidget.generated.h"

class UButton;
class UTextBlock;

/**
 * 게임 오버 화면 (SRS 6)을 관리하는 C++ 기반 위젯
 * BP에서는이 클래스를 상속받아 디자인만 수행합니다.
 */
UCLASS()
class UE5_PROJECT_API UGameOverWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	/**
	 * 위젯이 생성될 때(BeginPlay) 호출됩니다.
	 * BP의 버튼과 C++ 함수를 연결(바인딩)합니다.
	 */
	virtual void NativeConstruct() override;

	// --- BP 연결 (BindWidget) ---
	// BP 위젯의 이름과 C++ 변수 이름이 정확히 일치해야 합니다.

	// (BP) 재도전 버튼
	UPROPERTY(meta = (BindWidget))
	UButton* Button_Retry;

	// (BP) 메인 메뉴로 가기 버튼
	UPROPERTY(meta = (BindWidget))
	UButton* Button_MainMenu;

	// (BP) 게임오버 사유를 표시할 텍스트 (예: "체력 0", "패널티 5")
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_GameOverReason;


	// --- C++ 버튼 로직 ---
	// '재도전' 버튼이 클릭되면 호출됩니다.
	UFUNCTION()
	void OnRetryClicked();

	/** '메인 메뉴로' 버튼이 클릭되면 호출됩니다. */
	UFUNCTION()
	void OnMainMenuClicked();

	/**
	 * 메인 메뉴 레벨의 이름
	 * (에디터에서 C++ 클래스의 기본값으로 설정할 수 있습니다)
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	FName MainMenuLevelName = FName(TEXT("MainMenu")); // TODO: 실제 메인 메뉴 맵 이름으로 변경

public:
	/**
	 * GameMode가 이 위젯을 생성한 직후 호출하여
	 * 게임오버 사유를 텍스트로 표시합니다.
	 * @param bPlayerDied true면 체력 0, false면 패널티 5
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Over")
	void InitializeWidget(bool bPlayerDied);
};
