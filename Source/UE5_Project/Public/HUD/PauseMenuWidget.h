#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseMenuWidget.generated.h"


class UButton;
class APlayerCharacterController;
class UOptionMenuWidget;
class UControlsMenuWidget;
class UConfirmMainMenuWidget;


/**
 * 일시정지 메뉴 (WBP_PauseMenu)의 C++ 기반 클래스
 * BP에서는 이 클래스를 상속받아 디자인만 구성
**/
UCLASS()
class UE5_PROJECT_API UPauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	/**
	 * 위젯이 생성될 때 (BP의 EventConstruct) 호출
	 * 버튼 이벤트를 C++ 함수에 바인딩
	 */
	virtual void NativeConstruct() override;

private:
	//--- BP 위젯 변수 바인딩 ---

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Resume;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Option;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Controls;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_MainMenu;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Exit;

	//--- C++ 이벤트 핸들러 ---

	// '계속하기' 버튼 클릭 시
	UFUNCTION()
	void OnResumeClicked();

	// '설정' 버튼 클릭 시
	UFUNCTION()
	void OnOptionClicked();

	// '조작법' 버튼 클릭 시
	UFUNCTION()
	void OnControlsClicked();

	// '메인 메뉴로' 버튼 클릭 시
	UFUNCTION()
	void OnMainMenuClicked();

	// '게임 종료' 버튼 클릭 시
	UFUNCTION()
	void OnExitClicked();

	//--- 캐시된 참조 ---

	// NativeConstruct에서 캐시되는 플레이어 컨트롤러 참조
	UPROPERTY()
	TObjectPtr<APlayerCharacterController> PlayerControllerRef;

	//--- 하위 메뉴 위젯 ---
	// 이 메뉴에서 열어야 할 다른 메뉴들의 클래스 (BP에서 설정)

	// '설정' 버튼 클릭 시 열릴 옵션 메뉴 위젯 클래스
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UOptionMenuWidget> OptionMenuWidgetClass;

	// '조작법' 버튼 클릭 시 열릴 조작법 메뉴 위젯 클래스
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UControlsMenuWidget> ControlsMenuWidgetClass;

	// '메인 메뉴로' 버튼 클릭 시 열릴 확인 팝업 위젯 클래스
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UConfirmMainMenuWidget> ConfirmMainMenuWidgetClass;

	//--- UI 인스턴스 ---
	// [수정] 모든 자식 위젯을 '인스턴스'로 관리

	// 생성된 옵션 메뉴 인스턴스
	UPROPERTY(VisibleInstanceOnly, Category = "UI")
	TObjectPtr<UOptionMenuWidget> OptionMenuInstance;

	// 생성된 조작법 메뉴 인스턴스
	UPROPERTY(VisibleInstanceOnly, Category = "UI")
	TObjectPtr<UControlsMenuWidget> ControlsMenuInstance;

	// 생성된 메인 메뉴 확인 팝업 인스턴스
	UPROPERTY(VisibleInstanceOnly, Category = "UI")
	TObjectPtr<UConfirmMainMenuWidget> ConfirmMainMenuPopupInstance;

	// 메인 메뉴 레벨의 이름
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	FName MainMenuLevelName = FName(TEXT("MainMenu"));
};
