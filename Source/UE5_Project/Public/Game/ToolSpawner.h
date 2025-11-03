#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ToolSpawner.generated.h"


class AToolBase;


/**
 * '플레이 준비 구역'에 배치되어 특정 도구(AToolBase)를 스폰하고,
 * 해당 도구가 파괴되면 일정 시간 뒤 자동으로 리스폰
 */
UCLASS()
class UE5_PROJECT_API AToolSpawner : public AActor
{
	GENERATED_BODY()

public:
	AToolSpawner();

protected:
	virtual void BeginPlay() override;

	/**
	 * 스포너가 생성한 도구가 파괴되었을 때(OnDestroyed) 자동으로 호출
	 * @param DestroyedActor 파괴된 도구 액터
	 */
	UFUNCTION()
	void OnSpawnedToolDestroyed(AActor* DestroyedActor);

	/**
	 * RespawnDelay 이후에 호출되어 새 도구를 스폰
	 */
	UFUNCTION()
	void RespawnTool();

private:
	/**
	 * 이 스포너가 생성할 도구의 블루프린트 클래스
	 * (에디터의 Details 패널에서 BP_Tool_Camera 등으로 설정)
	 */
	UPROPERTY(EditAnywhere, Category = "Tool Spawner")
	TSubclassOf<AToolBase> ToolClassToSpawn;

	/**
	 * 도구가 파괴된 후 다시 스폰되기까지 걸리는 시간 (초)
	 */
	UPROPERTY(EditAnywhere, Category = "Tool Spawner")
	float RespawnDelay = 5.0f;

	/**
	 * 현재 스폰되어 레벨에 존재하는 도구에 대한 참조
	 * 이 포인터는 도구가 파괴되면 자동으로 NULL
	 */
	UPROPERTY()
	TWeakObjectPtr<AToolBase> CurrentSpawnedTool;

	// 리스폰 타이머 핸들
	FTimerHandle RespawnTimerHandle;

	/**
	 * 스폰 위치를 시각적으로 표시하기 위한 루트 컴포넌트
	 */
	UPROPERTY(VisibleAnywhere, Category = "Tool Spawner")
	USceneComponent* SpawnPoint;

	/**
	 * 스폰 로직을 실행
	 * BeginPlay와 리스폰 타이머에 의해 호출
	 */
	void SpawnTool();
};

