#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractableInterface.h"
#include "Anomaly/AnomalyTypes.h"
#include "StationaryAnomaly.generated.h"


class APlayerCharacter;
class UStaticMeshComponent;
class AAnomalyManager;
class AChasingAnomaly;
class AAreaKeeperGameState;
class UAnomalousPropertyComponent;


/**
 * 플레이어가 '소지'(Exorcism)해야 하는 정지된 이상현상
 */
UCLASS()
class UE5_PROJECT_API AStationaryAnomaly : public AActor, public IInteractableInterface
{
	GENERATED_BODY()

public:
	AStationaryAnomaly();

	// IInteractableInterface 구현
	virtual void Highlight_Implementation(bool bIsLooking) override;
	virtual void Interact_Implementation(APlayerCharacter* Interactor) override;
	virtual FString GetInteractText_Implementation() override;

	/**
	 * AnomalyManager가 스폰 직후 이 함수를 호출하여 이상현상을 설정합니다.
	 */
	void InitializeAnomaly(EAnomalyType Type, EAnomalyCategory Category, float InLifespan);

	/**
	 * '소지' UI 위젯에서 플레이어가 범주를 선택했을 때 호출됩니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "Anomaly")
	void OnTalismanRitualFinished(EAnomalyCategory SelectedCategory);

	// AnomalyManager가 스폰 직후 이 함수를 호출하여 연결
	void SetLinkedComponent(UAnomalousPropertyComponent* CompToLink);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Mesh;

	// 이상현상 속성
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Anomaly")
	EAnomalyType AnomalyType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Anomaly")
	EAnomalyCategory CorrectCategory;

	// 타이머
	FTimerHandle ResolveTimerHandle;

	// 미해결 상태로 타이머 만료 시 호출
	void OnResolveTimerExpired();

	// 하이라이트용 동적 머티리얼
	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterial;

	// 정지된 이상현상 해결/만료될 때 원상복구시켜야 할 컴포넌트
	UPROPERTY()
	TObjectPtr<UAnomalousPropertyComponent> LinkedComponent;

private:
	// 패널티 스택 증가
	void IncrementPenaltyStack();

	// 패널티 스택 감소
	void DecrementPenaltyStack();

	// 소지 실패 시 새 이상현상 스폰
	void SpawnFailedAnomaly();

	void ReturnLinkedActorToManager();

	// GameState 캐시
	UPROPERTY()
	TWeakObjectPtr<AAreaKeeperGameState> GameStateRef;

	// AnomalyManager 캐시
	UPROPERTY()
	TWeakObjectPtr<AAnomalyManager> AnomalyManagerRef;

};