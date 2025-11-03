#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ReadyZone.generated.h"

class UBoxComponent;
class APlayerCharacter;
class AAreaKeeperGameState;

/**
 * 플레이 준비 구역을 정의하는 액터
 * 플레이어가 이 구역을 떠나는 것을 감지하여 GameState의 준비 타이머를 시작
 */
UCLASS()
class UE5_PROJECT_API AReadyZone : public AActor
{
	GENERATED_BODY()

public:
	AReadyZone();

protected:
	virtual void BeginPlay() override;

	/** 이 구역의 범위를 나타내는 트리거 볼륨 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* TriggerVolume;

	/**
	 * 액터가 이 볼륨을 나갈 때 호출되는 함수
	 */
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	/** GameState 캐시 */
	UPROPERTY()
	TWeakObjectPtr<AAreaKeeperGameState> GameStateRef;

	/** 타이머가 이미 한 번 시작되었는지 확인 (중복 호출 방지) */
	bool bTimerHasBeenTriggered;
};
