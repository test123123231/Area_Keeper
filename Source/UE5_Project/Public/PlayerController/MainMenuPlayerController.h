#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainMenuPlayerController.generated.h"


class UInputMappingContext;
class UUserWidget;


UCLASS()
class UE5_PROJECT_API AMainMenuPlayerController : public APlayerController
{
	GENERATED_BODY()
protected:
    virtual void BeginPlay() override;

	/** * 생성할 메인 메뉴 위젯의 블루프린트 클래스
	 * TSubclassOf는 특정 클래스(여기서는 UUserWidget)의 자식 클래스만 할당할 수 있도록 제한
	 */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> MainMenuWidgetClass;

	// 생성된 메인 메뉴 위젯의 인스턴스를 저장할 변수
	UPROPERTY(VisibleInstanceOnly, Category = "UI")
	TObjectPtr<UUserWidget> MainMenuWidgetInstance;

private:
    // 기본 입력 매핑 컨텍스트를 저장할 변수
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bind", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputMappingContext> DefaultMappingContext;
};
