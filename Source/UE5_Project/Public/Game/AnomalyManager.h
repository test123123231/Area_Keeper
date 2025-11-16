#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Anomaly/AnomalyTypes.h"
#include "AnomalyManager.generated.h"


class AStationaryAnomaly;
class AChasingAnomaly;
class AAreaKeeperGameState;


/**
 * 게임 내 모든 이상현상의 스폰, 타이밍, 생명 주기를 관리
 * GameMode에 의해 단일 인스턴스로 존재
 */
UCLASS()
class UE5_PROJECT_API AAnomalyManager : public AActor
{
	GENERATED_BODY()

public:
	AAnomalyManager();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditInstanceOnly, Category = "Anomaly Config")
	TArray<TObjectPtr<AActor>> ModifiableActors;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> AvailableModifiableActors;

	// 핵심 스폰 API
public:
	/**
	 * (GameMode/GameState가 호출) 준비 구역 타이머 종료 시 주기적인 스폰을 시작
	 */
	UFUNCTION(BlueprintCallable, Category = "Anomaly Spawning")
	void StartSpawning();

	/**
	 * (GameMode/GameState가 호출) 게임 오버 시 모든 스폰 타이머를 중지
	 */
	UFUNCTION(BlueprintCallable, Category = "Anomaly Spawning")
	void StopSpawning();

	/**
	 * (AStationaryAnomaly가 호출) '정지된 이상현상'이 '쫓아오는' 유형으로 만료되었을 때 호출
	 * param Location 스폰할 위치
	 */
	UFUNCTION(BlueprintCallable, Category = "Anomaly Spawning")
	void SpawnChasingAnomaly(const FVector& Location);

	/**
	 * 실제 '정지된 이상현상' 스폰 로직
	 */
	void SpawnStationaryAnomaly();

	UFUNCTION()
	void ReturnActorToAvailableList(AActor* ActorToReturn);

private:
	// 생성 유지 시간 (초)
	UPROPERTY(EditAnywhere, Category = "Anomaly Config")
	float StationaryAnomalyLifespan = 25.0f;

private:
	/**
	 * 주기적인 스폰 타이머가 만료될 때마다 호출
	 */
	void OnSpawnTimerExpired();

	/**
	 * NavMesh 위에서 스폰 가능한 랜덤 위치를 찾음
	 * param OutLocation 찾은 위치
	 * return 위치 찾기 성공 여부
	 */
	FVector GetRandomSpawnLocation();

	/**
	 * 스폰될 이상현상의 범주 (시각, 청각 등)를 랜덤하게 결정
	 */
	EAnomalyCategory GetRandomAnomalyCategory() const;

	// StationaryAnomalyClassList 목록 중 랜덤하게 하나를 선택
	TSubclassOf<AStationaryAnomaly> GetRandomStationaryAnomalyClass() const;

	// ChasingAnomalyClassList 목록 중 랜덤하게 하나를 선택
	TSubclassOf<AChasingAnomaly> GetRandomChasingAnomalyClass();

	void InitializeAvailableActorList();

	// 설정 프로퍼티 (에디터에서 설정)
protected:
	// (Blueprint) 스폰할 '정지된 이상현상'의 블루프린트 클래스 목록
	UPROPERTY(EditDefaultsOnly, Category = "Anomaly Config")
	TArray<TSubclassOf<AStationaryAnomaly>> StationaryAnomalyClassList;

	// (Blueprint) 스폰할 '쫓아오는 이상현상'의 블루프린트 클래스 목록
	UPROPERTY(EditDefaultsOnly, Category = "Anomaly Config")
	TArray<TSubclassOf<AChasingAnomaly>> ChasingAnomalyClassList;

	// 스폰 타이머 핸들 */
	FTimerHandle SpawnTimerHandle;

	// 이상현상을 스폰할 반경 (Manager 액터 기준)
	UPROPERTY(EditAnywhere, Category = "Anomaly Config")
	float SpawnRadius = 10000.0f; // 100m

	/** 최초 이상현상 생성 주기 (초)
	 * 이 값은 GameState에 의해 점진적으로 감소
	 */
	UPROPERTY(EditAnywhere, Category = "Anomaly Config")
	float CurrentSpawnInterval = 20.0f;

	// (미구현) 흉내쟁이 귀신 스폰 타이머 핸들 
	FTimerHandle MimicSpawnTimerHandle;

	// GameState 캐시
	UPROPERTY()
	TWeakObjectPtr<AAreaKeeperGameState> GameStateRef;
};
