#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AreaKeeperGameMode.generated.h"


class AAreaKeeperGameState;
class UGameOverWidget;
class UUserWidget;
class UGameClearWidget;


/**
 * 게임의 핵심 규칙(시작, 종료, UI 관리)을 담당하는 게임 모드
 */
UCLASS()
class UE5_PROJECT_API AAreaKeeperGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AAreaKeeperGameMode();

protected:
	/**
	 * 게임 모드 시작 시(BP의 Event BeginPlay) 호출
	 * GameState를 찾아 델리게이트를 바인딩
	 */
	virtual void BeginPlay() override;

	/**
	 * GameState의 OnGameOver 델리게이트에 바인딩되어 호출될 함수
	 */
	UFUNCTION()
	void HandleGameOver(bool bPlayerDied);

	/**
	 * GameState의 OnGameClear 델리게이트에 바인딩되어 호출될 함수
	 * (BP의 HandleGameClear 이벤트)
	 */
	UFUNCTION()
	void HandleGameClear();

	/**
	 * 플레이어 컨트롤러의 입력을 중지하고 UI 모드로 변경
	 */
	void SetPlayerInputModeToUIOnly();

	/**
	 * (BP에서 설정) 게임 오버 시 생성할 위젯 클래스
	 */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UGameOverWidget> GameOverWidgetClass;

	/**
	 * (BP에서 설정) 게임 클리어 시 생성할 위젯 클래스
	 */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UGameClearWidget> GameClearWidgetClass;

private:
	/**
	 * 캐시된 GameState 참조 변수입니다.
	 * (BP의 'Area Keeper Game State' 변수)
	 */
	UPROPERTY()
	TWeakObjectPtr<AAreaKeeperGameState> GameStateRef;

	/**
	 * 현재 화면에 표시된 UI 위젯의 인스턴스입니다.
	 */
	UPROPERTY()
	TObjectPtr<UUserWidget> CurrentScreenWidget;
};
