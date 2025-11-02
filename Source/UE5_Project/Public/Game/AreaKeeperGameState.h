#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Game/GameTypes.h"
#include "AreaKeeperGameState.generated.h"


class AAnomalyManager;


// 패널티 스택 변경 시 UI 바인딩용 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPenaltyStackChanged, int32, NewStackCount);
// 게임 오버/클리어 시 BP GameMode가 바인딩할 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameOver, bool, bPlayerDied);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameClear);

/**
 * 게임의 모든 상태 변수(패널티, 타이머)를 관리합니다. (싱글플레이 전용)
 * AStationaryAnomaly는 이 클래스의 함수를 호출하여 패널티 스택을 변경
 */
UCLASS()
class UE5_PROJECT_API AAreaKeeperGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AAreaKeeperGameState();

	

	// (AAnomalyManager가 호출) 현재 스폰 주기를 반환
	UFUNCTION(BlueprintPure, Category = "Game State | Timer")
	float GetCurrentAnomalySpawnInterval() const { return CurrentAnomalySpawnInterval; }

	// AGameStateBase 오버라이드
	virtual void Tick(float DeltaTime) override;


	// (BP용) 게임 오버 시 호출됩니다. BP GameMode에서 이 이벤트를 바인딩
	UPROPERTY(BlueprintAssignable, Category = "Game Events")
	FOnGameOver OnGameOver;

	// (BP용) 게임 클리어 시 호출됩니다. BP GameMode에서 이 이벤트를 바인딩
	UPROPERTY(BlueprintAssignable, Category = "Game Events")
	FOnGameClear OnGameClear;

	// 타이머 및 게임 흐름
	// (GameMode가 호출) 플레이어가 준비 구역을 떠났을 때 60초 준비 타이머를 시작
	UFUNCTION(BlueprintCallable, Category = "Game State | Timer")
	void StartReadyZoneTimer();

	// 패널티 시스템 
	/**
	* (AStationaryAnomaly가 호출) 미해결 이상현상으로 인해 패널티 스택을 1 증가
	*/
	UFUNCTION(BlueprintCallable, Category = "Game State | Penalty")
	void IncrementPenaltyStack();

	/**
	 * (AStationaryAnomaly가 호출) '소지' 성공으로 패널티 스택을 1 감소
	 */
	UFUNCTION(BlueprintCallable, Category = "Game State | Penalty")
	void DecrementPenaltyStack();

	// 현재 패널티 스택을 반환
	UFUNCTION(BlueprintPure, Category = "Game State | Penalty")
	int32 GetPenaltyStack() const { return CurrentPenaltyStack; }

	// 패널티 스택 변경 시 브로드캐스트
	UPROPERTY(BlueprintAssignable, Category = "Game State | Penalty")
	FOnPenaltyStackChanged OnPenaltyStackChanged;

protected:
	// 현재 패널티 스택. 0~5 사이 값
	UPROPERTY(VisibleInstanceOnly, Category = "Game State | Penalty")
	int32 CurrentPenaltyStack;

	// 서버와 클라이언트 모두에서 델리게이트를 브로드캐스트합니다.
	void BroadcastPenaltyStackChange();

	// 현재 게임 플레이 상태를 반환
	UFUNCTION(BlueprintPure, Category = "Game State | Timer")
	EAreaKeeperPlayState GetCurrentPlayState() const { return CurrentPlayState; }

	// 현재 스폰 주기를 설정 (20초에서 15초 사이 선형 보간)
	void SetCurrentAnomalySpawnInterval();

protected:
	// 현재 게임 플레이 상태
	UPROPERTY(VisibleInstanceOnly, Category = "Game State | Timer")
	EAreaKeeperPlayState CurrentPlayState;

	// 60초 준비 타이머 (카운트다운)
	UPROPERTY(VisibleInstanceOnly, Category = "Game State | Timer")
	float ReadyZoneTimer;

	// 20분 생존 타이머 (카운트업)
	UPROPERTY(VisibleInstanceOnly, Category = "Game State | Timer")
	float GameTimer;

	// 현재 이상현상 생성 주기 (20초 -> 15초)
	UPROPERTY(VisibleInstanceOnly, Category = "Game State | Timer")
	float CurrentAnomalySpawnInterval;

	// 설정값 (에디터에서 수정 가능)

	UPROPERTY(EditDefaultsOnly, Category = "Game Config")
	float InitialReadyZoneTime = 60.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Game Config")
	float TotalGameTime = 1200.0f; // 20분

	UPROPERTY(EditDefaultsOnly, Category = "Game Config")
	float InitialAnomalySpawnInterval = 20.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Game Config")
	float FinalAnomalySpawnInterval = 15.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Game Config")
	int32 MaxPenaltyStack = 5;

	// 헬퍼
private:
	// AnomalyManager 캐시
	UPROPERTY()
	TWeakObjectPtr<AAnomalyManager> AnomalyManagerRef;

	// AnomalyManager 참조를 캐시
	void CacheManagerReferences();
};

