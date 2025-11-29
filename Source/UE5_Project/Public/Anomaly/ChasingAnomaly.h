#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/InteractableInterface.h"
#include "Perception/AIPerceptionComponent.h"
#include "Anomaly/AnomalyTypes.h"
#include "ChasingAnomaly.generated.h"


class UAISenseConfig_Sight;
class AAIController;
class APlayerCharacter;
class AAreaKeeperGameState;


UCLASS()
class UE5_PROJECT_API AChasingAnomaly : public ACharacter, public IInteractableInterface
{
	GENERATED_BODY()

public:
	AChasingAnomaly();

	// IInteractableInterface
	virtual void Highlight_Implementation(bool bOn) override;
	virtual void Interact_Implementation(APlayerCharacter* Interactor) override;
	virtual FString GetInteractText_Implementation() override;

	// 이 이상현상을 제거
	UFUNCTION(BlueprintCallable, Category = "Anomaly")
	void Banish();

	/**
	* 이 이상현상을 퇴치하는 데 필요한 도구 타입을 반환합니다.
	*/
	UFUNCTION(BlueprintPure, Category = "Anomaly")
	EToolType GetRequiredToolType() const { return RequiredToolType; }

	void SetHighlightColor(bool bIsCompatible);
	void ClearHighlight();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// AI 상태 및 컴포넌트

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	EChasingAnomalyState CurrentState;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	UAIPerceptionComponent* AIPerception;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	UAISenseConfig_Sight* SightConfig;

	UPROPERTY()
	TObjectPtr<AAIController> AnomalyController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<AActor> PlayerTarget;

	/**
	*이상현상을 퇴치하는 데 필요한 도구 타입
	* BP_ChasingAnomaly 블루프린트의 Class Defaults에서 설정
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anomaly")
	EToolType RequiredToolType;

	// AI 속성

	// 추적 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float ChasingSpeed;

	// NavMesh를 무시하고 돌진을 시작할 거리 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float CloseAttackRange = 100.f;

	// MoveTo가 멈추는 거리 (크게 중요하지 않음)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float AcceptanceRadius = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float SightRadius = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float LoseChaseRange = 1200.f;

	// GameState 참조 변수
	UPROPERTY()
	TWeakObjectPtr<AAreaKeeperGameState> GameStateRef;

	// AI 행동 함수
	UFUNCTION()
	void OnSeePawn(AActor* SeenActor, FAIStimulus Stimulus);

	virtual void ChaseTarget(AActor* Target);
	virtual void StopChasing();

	// NavMesh 기반 길찾기
	void MoveToTarget(AActor* Target);

	// NavMesh 무시하고 돌진
	void MoveDirectlyToTarget(const FVector& TargetLocation);

	// 피격 및 피해 

	UFUNCTION()
	void OnAnomalyOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void ApplyDamageToPlayer(APlayerCharacter* Player);
	
	// 무적이 끝났을 때 실행
	UFUNCTION()
	void OnPlayerInvincibilityEnd(APlayerCharacter* Player);
	
};

