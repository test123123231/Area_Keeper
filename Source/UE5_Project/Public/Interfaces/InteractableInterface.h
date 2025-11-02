#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractableInterface.generated.h"

class APlayerCharacter;

UINTERFACE(MinimalAPI, Blueprintable)
class UInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 플레이어가 상호작용할 수 있는 모든 액터(아이템, 도구, 이상현상, 충전기)가
 * 구현해야 하는 인터페이스입니다.
 */
class UE5_PROJECT_API IInteractableInterface
{
	GENERATED_BODY()

public:
	/**
	 * @brief 플레이어가 이 객체를 바라볼 때 호출됩니다.
	 * 'AreaKeeper'의 HighlightItem 함수를 대체합니다.
	 * @param bIsLooking true면 바라보기 시작, false면 바라보기 중단
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interact")
	void Highlight(bool bIsLooking);

	/**
	 * @brief 플레이어가 상호작용(E키)을 시도할 때 호출됩니다.
	 * @param Interactor 상호작용을 시도하는 캐릭터 (플레이어)
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interact")
	void Interact(APlayerCharacter* Interactor);

	/**
	 * @brief (선택적) 상호작용 시 UI에 표시할 텍스트를 반환합니다.
	 * (예: "줍기", "도구 사용", "소지하기", "충전하기")
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interact")
	FString GetInteractText();
};

