#include "Game/ToolSpawner.h"
#include "Item/ToolBase.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"


AToolSpawner::AToolSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	// 스폰 위치를 나타낼 루트 컴포넌트 생성
	SpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnPoint"));
	RootComponent = SpawnPoint;
}


void AToolSpawner::BeginPlay()
{
	Super::BeginPlay();

	// 게임이 시작되면 즉시 첫 번째 도구를 스폰
	SpawnTool();
}


/**
 * 도구를 스폰하고, OnDestroyed 델리게이트에 바인딩
 */
void AToolSpawner::SpawnTool()
{
	// 스폰할 클래스가 BP에서 설정되었는지,
	// 그리고 (혹시 모를 중복 스폰 방지를 위해) 현재 스폰된 도구가 없는지 확인
	if (ToolClassToSpawn && !CurrentSpawnedTool.IsValid())
	{
		UWorld* const World = GetWorld();
		if (World)
		{
			// 스포너의 위치와 회전값을 스폰 위치로 사용
			const FVector SpawnLocation = GetActorLocation();
			const FRotator SpawnRotation = GetActorRotation();

			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			// 도구 스폰
			AToolBase* NewTool = World->SpawnActor<AToolBase>(ToolClassToSpawn, SpawnLocation, SpawnRotation, SpawnParams);

			if (NewTool)
			{
				// 스폰된 도구를 추적할 수 있도록 변수에 저장
				CurrentSpawnedTool = NewTool;

				// [핵심 로직] 스폰된 도구의 OnDestroyed 이벤트에
				// 우리의 OnSpawnedToolDestroyed 함수를 바인딩(연결)합니다.
				NewTool->OnDestroyed.AddDynamic(this, &AToolSpawner::OnSpawnedToolDestroyed);

				UE_LOG(LogTemp, Log, TEXT("%s: 도구(%s)를 스폰했습니다."), *GetName(), *NewTool->GetName());
			}
		}
	}
	else if (!ToolClassToSpawn)
	{
		UE_LOG(LogTemp, Error, TEXT("%s: ToolClassToSpawn이 설정되지 않았습니다! (BP에서 설정 필요)"), *GetName());
	}
}


/**
 * 스폰했던 도구가 파괴되면(Destroy) 호출됩니다.
 */
void AToolSpawner::OnSpawnedToolDestroyed(AActor* DestroyedActor)
{
	UE_LOG(LogTemp, Log, TEXT("%s: 스폰된 도구(%s)의 파괴를 감지했습니다. %f초 후 리스폰합니다."), *GetName(), *DestroyedActor->GetName(), RespawnDelay);

	// 리스폰 타이머를 시작합니다.
	GetWorld()->GetTimerManager().SetTimer(
		RespawnTimerHandle,
		this,
		&AToolSpawner::RespawnTool, // 타이머 만료 시 RespawnTool 함수 호출
		RespawnDelay,
		false
	);
}


/**
 * RespawnDelay 이후에 호출되어 SpawnTool 함수를 실행합니다.
 */
void AToolSpawner::RespawnTool()
{
	// 리스폰 타이머가 만료되면, 새 도구를 스폰합니다.
	SpawnTool();
}

