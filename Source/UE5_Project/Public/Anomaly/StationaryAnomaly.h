// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractableInterface.h" // 인터페이스
#include "Anomaly/AnomalyTypes.h"  // Enum
#include "StationaryAnomaly.generated.h"

class APlayerCharacter;
class UStaticMeshComponent;
class AAnomalyManager; // 전방 선언
class AHousekeeperGameState; // 전방 선언

/**
 * (SRS 1) 플레이어가 '소지'(Exorcism)해야 하는 정지된 이상현상입니다.
 */
UCLASS()
class UE5_PROJECT_API AStationaryAnomaly : public AActor, public IInteractableInterface
{
	GENERATED_BODY()

public:
	AStationaryAnomaly();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* Mesh;

	// --- 이상현상 속성 ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anomaly")
	EAnomalyType AnomalyType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anomaly")
	EAnomalyCategory CorrectCategory;

	// --- 타이머 ---
	FTimerHandle ResolveTimerHandle;

	/** (SRS 2.1.1, 3.1.1) 미해결 상태로 타이머 만료 시 호출 */
	void OnResolveTimerExpired();

	// --- '소지' (Exorcism) 로직 ---

	/** (SRS 4.1.2) 플레이어가 상호작용 시 '소지' UI 시작 */
	void StartExorcism(APlayerCharacter* Player);

	// --- 헬퍼 함수 ---
	// TODO: GameState 및 AnomalyManager 구현 후 주석 해제
	// AAnomalyManager* GetAnomalyManager();
	// AHousekeeperGameState* GetHousekeeperGameState();

	/** (SRS 2.1.1) 쫓아오는 이상현상 스폰 */
	void SpawnChasingAnomaly();

	/** (SRS 3.1.2) 패널티 스택 증가 */
	void IncrementPenaltyStack();

	/** (SRS 5.1.4) 패널티 스택 감소 */
	void DecrementPenaltyStack();

	/** (SRS 4.1.5) 소지 실패 시 새 이상현상 스폰 */
	void SpawnFailedAnomaly();

public:
	// --- IInteractableInterface 구현 ---
	virtual void Highlight_Implementation(bool bOn) override;
	virtual void Interact_Implementation(APlayerCharacter* Interactor) override;
	virtual FString GetInteractText_Implementation() override;

	/** * AnomalyManager가 스폰 직후 이 함수를 호출하여 이상현상을 설정합니다.
	 * (SRS 1.1.2, 1.1.5)
	 */
	void InitializeAnomaly(EAnomalyType Type, EAnomalyCategory Category, float InLifespan);

	/**
	 * '소지' UI 위젯에서 플레이어가 범주를 선택했을 때 호출됩니다.
	 * @param SelectedCategory 플레이어가 UI에서 선택한 범주
	 */
	UFUNCTION(BlueprintCallable, Category = "Anomaly")
	void OnExorcismFinished(EAnomalyCategory SelectedCategory);
};
