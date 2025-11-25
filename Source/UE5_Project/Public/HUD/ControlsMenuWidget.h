#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Input/Events.h"
#include "Input/Reply.h"
#include "ControlsMenuWidget.generated.h"

class UButton;
class APlayerController;

/**
 * '조작법' 메뉴 (WBP_ControlsMenu)의 C++ 기반 클래스
 * PauseMenuWidget에 의해 생성
 */
UCLASS()
class UE5_PROJECT_API UControlsMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 이 위젯을 연 부모 메뉴(PauseMenu)를 설정
	void SetParentMenu(UUserWidget* InParentMenu);

protected:
	// (EventConstruct) 위젯을 포커스 가능하게 설정하고 버튼을 바인딩
	virtual void NativeConstruct() override;

	// (ESC 키 처리) ESC 키를 누르면 메뉴를 닫음
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	// (포커스 유지) 배경 클릭 시 포커스를 잃지 않도록 함
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

private:
	//--- BP 위젯 변수 바인딩 ---

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Back;

	//--- C++ 이벤트 핸들러 ---

	// '뒤로가기' 버튼 클릭 시 호출
	UFUNCTION()
	void OnBackClicked();

	/**
	 * 이 위젯을 닫고 부모 메뉴(PauseMenu)로 포커스를 돌려줌
	 */
	void CloseMenu();

	//--- 캐시된 참조 ---
	UPROPERTY()
	TObjectPtr<UUserWidget> ParentMenu;

	UPROPERTY()
	TObjectPtr<APlayerController> PlayerControllerRef;
};
