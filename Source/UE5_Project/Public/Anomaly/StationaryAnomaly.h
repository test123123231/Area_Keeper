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
class UBoxComponent;


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
	 * AnomalyManager가 호출하여 '환경 변조' 이상현상을 설정합니다.
	 */
	void InitializeFromActor(
		AActor* ActorToReplace,
		EAnomalyType Type,
		EAnomalyCategory Category,
		float InLifespan
	);

	/**
	 * '소지' UI 위젯에서 플레이어가 범주를 선택했을 때 호출됩니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "Anomaly")
	void OnTalismanRitualFinished(EAnomalyCategory SelectedCategory);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Mesh;

	// [새로 추가] 상호작용을 위한 보이지 않는 볼륨 (새로운 루트 컴포넌트)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> InteractVolume;

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

private:
	// 패널티 스택 증가
	void IncrementPenaltyStack();

	// 패널티 스택 감소
	void DecrementPenaltyStack();

	// --- [새로 추가] 원본 액터 정보 ---
	// 'OriginalActor'가 원본 액터의 '시각'과 '물리'를 모두 제어
	UPROPERTY()
	TObjectPtr<AActor> OriginalActor;

	/** (AnomalousPropertyComponent 대체) 원본 액터의 스케일 */
	UPROPERTY()
	FVector OriginalScale;

	/** (AnomalousPropertyComponent 대체) 원본 액터의 위치 (월드) */
	UPROPERTY()
	FVector OriginalLocation;

	// --- [새로 추가] 변조 기능 ---
	/** 이 액터를 공중에 띄웁니다. */
	void StartLevitation(float FloatHeight);

	/** 이 액터의 크기를 키웁니다. */
	void StartGigantism(float ScaleMultiplier);

	/** 이 액터의 머티리얼을 변경합니다. */
	void StartPeculiarity();

	/** 원본 액터를 복원하고 매니저에게 반납합니다. */
	void ResetToNormal();

	// 원본 액터의 바운딩 박스에 맞게 InteractVolume 크기를 조절합니다.
	void UpdateInteractionVolume();

	void SetComponentsVisibilityCollision(AActor* ActorToReplace, bool bSet);

	// GameState 캐시
	UPROPERTY()
	TWeakObjectPtr<AAreaKeeperGameState> GameStateRef;

	// AnomalyManager 캐시
	UPROPERTY()
	TWeakObjectPtr<AAnomalyManager> AnomalyManagerRef;

};