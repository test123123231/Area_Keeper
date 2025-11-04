#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Input/Events.h"
#include "Input/Reply.h"
#include "ConfirmMainMenuWidget.generated.h"


class UButton;
class APlayerController;


/**
 * '메인 메뉴로 돌아가기' 확인 팝업 (WBP_ConfirmMainMenu)의 C++ 기반 클래스
 * PauseMenuWidget에 의해 생성됩니다.
 */
UCLASS()
class UE5_PROJECT_API UConfirmMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 이 위젯을 연 부모 메뉴(PauseMenu)를 설정
	void SetParentMenu(UUserWidget* InParentMenu);

protected:
	// (EventConstruct) 위젯을 포커스 가능하게 설정하고 버튼을 바인딩
	virtual void NativeConstruct() override;

private:
	//--- BP 위젯 변수 바인딩 ---

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Yes;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_No;

	//--- C++ 이벤트 핸들러 ---

	// '예' (메인 메뉴로 이동) 버튼 클릭 시 호출
	UFUNCTION()
	void OnYesClicked();

	// '아니오' (취소) 버튼 클릭 시 호출
	UFUNCTION()
	void OnNoClicked();

	/**
	 * 이 위젯을 닫고 부모 메뉴(PauseMenu)로 포커스를 돌려줌
	 */
	void CloseMenu();

	//--- 설정 ---

	// BP의 클래스 기본값에서 설정할 메인 메뉴 레벨의 이름
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	FName MainMenuLevelName = FName(TEXT("MainMenu"));

	//--- 캐시된 참조 ---
	UPROPERTY()
	TObjectPtr<UUserWidget> ParentMenu;

	UPROPERTY()
	TObjectPtr<APlayerController> PlayerControllerRef;
};
