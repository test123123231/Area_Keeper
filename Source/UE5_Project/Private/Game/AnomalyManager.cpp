#include "Game/AnomalyManager.h"
#include "Anomaly/StationaryAnomaly.h"
#include "Anomaly/ChasingAnomaly.h"
#include "NavigationSystem.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Game/AreaKeeperGameState.h"


AAnomalyManager::AAnomalyManager()
{
	// 스폰만 관리하므로 틱 필요 없음
	PrimaryActorTick.bCanEverTick = false;
}


void AAnomalyManager::BeginPlay()
{
	Super::BeginPlay();

	// GameState 참조 캐시 (주기적으로 타이머 간격을 가져오기 위함)
	GameStateRef = GetWorld() ? GetWorld()->GetGameState<AAreaKeeperGameState>() : nullptr;
}


void AAnomalyManager::StartSpawning()
{
	UE_LOG(LogTemp, Log, TEXT("AnomalyManager: 스폰을 시작합니다. 주기: %.1f초"), CurrentSpawnInterval);

	// 첫 번째 스폰은 즉시 실행
	OnSpawnTimerExpired();
}


void AAnomalyManager::StopSpawning()
{
	UE_LOG(LogTemp, Log, TEXT("AnomalyManager: 모든 스폰을 중지합니다."));
	GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);
	//GetWorld()->GetTimerManager().ClearTimer(MimicSpawnTimerHandle);
}


void AAnomalyManager::OnSpawnTimerExpired()
{
	// 게임 시간에 따라 스폰 주기를 조절
	if (GameStateRef.IsValid())
	{
		CurrentSpawnInterval = GameStateRef->GetCurrentAnomalySpawnInterval();

		// 타이머 재설정
		GetWorld()->GetTimerManager().SetTimer(
			SpawnTimerHandle,
			this,
			&AAnomalyManager::OnSpawnTimerExpired,
			CurrentSpawnInterval,
			true
		);
	}
	
	// '정지된 이상현상' 스폰
	SpawnStationaryAnomaly();
}


void AAnomalyManager::SpawnStationaryAnomaly()
{
	// 리스트가 비어있으면 오류 로그 출력 후 종료
	if (StationaryAnomalyClassList.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("AnomalyManager: StationaryAnomalyClassList가 비어있습니다! 에디터에서 설정하세요."));
		return;
	}

	FVector SpawnLocation;
	if (GetRandomSpawnLocation(SpawnLocation))
	{
		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			AStationaryAnomaly* NewAnomaly = World->SpawnActor<AStationaryAnomaly>(
				GetRandomStationaryAnomalyClass(),
				SpawnLocation,
				FRotator::ZeroRotator,
				SpawnParams
			);

			if (NewAnomaly)
			{
				// 스폰된 이상현상에 랜덤 속성 부여
				EAnomalyType Type = GetRandomAnomalyType();
				EAnomalyCategory Category = GetRandomAnomalyCategory();

				// 이상현상의 유지 시간 StationaryAnomalyLifespan 멤버 변수 사용
				float Lifespan = StationaryAnomalyLifespan;

				// (StationaryAnomaly.h 참조) 스폰된 액터 초기화
				NewAnomaly->InitializeAnomaly(Type, Category, Lifespan);

				UE_LOG(LogTemp, Log, TEXT("AnomalyManager: '정지된 이상현상' 스폰. 유형: %d, 범주: %d"), Type, Category);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AnomalyManager: 스폰 위치를 찾는 데 실패했습니다. NavMesh가 빌드되었는지 확인하세요."));
	}
}


void AAnomalyManager::SpawnChasingAnomaly(const FVector& Location)
{
	UWorld* World = GetWorld();
	if (World)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		World->SpawnActor<AChasingAnomaly>(
			GetRandomChasingAnomalyClass(),
			Location,
			FRotator::ZeroRotator,
			SpawnParams
		);

		UE_LOG(LogTemp, Log, TEXT("AnomalyManager: 쫓아오는 이상현상 스폰. 위치: %s"), *Location.ToString());
	}
}


bool AAnomalyManager::GetRandomSpawnLocation(FVector& OutLocation)
{
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (NavSys)
	{
		FNavLocation RandomLocation;

		// 이 Manager 액터의 위치를 기준으로 SpawnRadius 반경 내의 랜덤한 NavMesh 위 포인트를 찾습니다.
		bool bFound = NavSys->GetRandomPointInNavigableRadius(GetActorLocation(), SpawnRadius, RandomLocation);

		if (bFound)
		{
			OutLocation = RandomLocation.Location;
			return true;
		}
	}

	// NavMesh가 없거나 유효한 위치를 찾지 못한 경우
	return false;
}


EAnomalyType AAnomalyManager::GetRandomAnomalyType() const
{
	// 단순 50:50 확률로 결정
	return (FMath::RandBool()) ? EAnomalyType::AT_Disappearing : EAnomalyType::AT_Chasing;
}


EAnomalyCategory AAnomalyManager::GetRandomAnomalyCategory() const
{
	// EAnomalyCategory에 정의된 범주 중 랜덤 선택
	// AC_None (0)을 제외하고 1부터 마지막 항목까지의 범위에서 랜덤 값을 고름

	// EAnomalyCategory::AC_Environmental 값을 가져옵니다 (enum의 마지막 항목이라고 가정).
	int32 MaxCategoryIndex = static_cast<int32>(EAnomalyCategory::AC_Environmental);
	if (MaxCategoryIndex <= 0)
	{
		return EAnomalyCategory::AC_None; // enum이 비어있는 경우
	}

	int32 RandomIndex = FMath::RandRange(1, MaxCategoryIndex);
	return static_cast<EAnomalyCategory>(RandomIndex);
}


TSubclassOf<AStationaryAnomaly> AAnomalyManager::GetRandomStationaryAnomalyClass() const
{
	if (StationaryAnomalyClassList.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("AnomalyManager: StationaryAnomalyClassList가 비어있습니다!"));
		return nullptr;
	}
	int32 RandomIndex = FMath::RandRange(0, StationaryAnomalyClassList.Num() - 1);
	return StationaryAnomalyClassList[RandomIndex];
}


TSubclassOf<AChasingAnomaly> AAnomalyManager::GetRandomChasingAnomalyClass()
{
	if (ChasingAnomalyClassList.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("AnomalyManager: ChasingAnomalyClassList가 비어있습니다!"));
		return nullptr;
	}
	int32 RandomIndex = FMath::RandRange(0, ChasingAnomalyClassList.Num() - 1);
	return ChasingAnomalyClassList[RandomIndex];
}