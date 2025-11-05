#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Anomaly/AnomalyTypes.h"
#include "TalismanWidget.generated.h"

class UButton;
class APlayerCharacter;

/**
 * '소지' UI (지방 선택)를 위한 C++ 기반 위젯 클래스
 * BP(WBP_Talisman)에서는 이 클래스를 상속받아 디자인만 구성
 */
UCLASS()
class UE5_PROJECT_API UTalismanWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	/**
	 * 위젯이 생성될 때 (BP의 EventConstruct) 호출
	 * 플레이어 참조를 캐시하고 버튼 이벤트를 C++ 함수에 바인딩
	 */
	virtual void NativeConstruct() override;

private:
	//--- BP 위젯 변수 바인딩 ---

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_VisualSense;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_AuditorySense;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Object;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Environment;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_GoBack;

	// C++ 이벤트 핸들러
	// BP의 OnClicked 이벤트를 대체

	UFUNCTION()
	void OnVisualSenseClicked();

	UFUNCTION()
	void OnAuditorySenseClicked();

	UFUNCTION()
	void OnObjectClicked();

	UFUNCTION()
	void OnEnvironmentClicked();

	UFUNCTION()
	void OnGoBackClicked();

	/**
	 * 버튼 클릭 시 PlayerCharacter의 함수를 호출하는 공통 헬퍼 함수
	 * @param SelectedCategory 플레이어가 선택한 이상현상 범주
	 */
	void HandleSelection(EAnomalyCategory SelectedCategory);

	//--- 캐시된 참조 ---

	// NativeConstruct에서 캐시되는 플레이어 캐릭터 참조
	UPROPERTY()
	TObjectPtr<APlayerCharacter> PlayerCharacterRef;
};
