#include "Game/AnomalyManager.h"
#include "Anomaly/StationaryAnomaly.h"
#include "Anomaly/ChasingAnomaly.h"
#include "NavigationSystem.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Game/AreaKeeperGameState.h"
#include "Engine/StaticMeshActor.h"


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

	// 디자이너가 액터를 할당했는지 확인
	if (ModifiableActors.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("AnomalyManager: ModifiableMeshActors 배열이 비어있습니다! 에디터에서 액터를 할당해야 합니다."));
	}

	InitializeAvailableActorList();
}


void AAnomalyManager::InitializeAvailableActorList()
{
	// 마스터 리스트를 사용 가능 리스트로 그대로 복사
	AvailableModifiableActors = ModifiableActors;

	if (AvailableModifiableActors.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("AnomalyManager: ModifiableActors 배열이 비어있습니다!"));
	}
}


void AAnomalyManager::ReturnActorToAvailableList(AActor* ActorToReturn)
{
	if (ActorToReturn && ModifiableActors.Contains(ActorToReturn))
	{
		// 마스터 리스트에 있는 액터가 맞는지 확인 후, '사용 가능' 목록에 다시 추가
		AvailableModifiableActors.AddUnique(ActorToReturn);
	}
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
	UE_LOG(LogTemp, Log, TEXT("AnomalyManager: '정지된 이상현상' 스폰 시도"));

	// 어떤 이상현상을 스폰할지 랜덤하게 결정
	EAnomalyCategory AnomalyEffect = GetRandomAnomalyCategory();

	// '환경 변조'가 걸렸는데 사용 가능한 액터가 없으면 '새 물체'로 강제 변경 (폴백)
	if (AnomalyEffect != EAnomalyCategory::EAC_Intrusion && AvailableModifiableActors.Num() == 0)
	{
		AnomalyEffect = EAnomalyCategory::EAC_Intrusion;
		UE_LOG(LogTemp, Log, TEXT("AnomalyManager: 변조 가능 액터 없음. '새 물체'로 강제 전환."));
	}

	// 스폰 준비
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("AnomalyManager: World가 비어있습니다."));
		return;
	}

	TSubclassOf<AStationaryAnomaly> AnomalyClass;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// 4. 로직 분기
	if (AnomalyEffect == EAnomalyCategory::EAC_Intrusion)
	{
		// --- 4-A. "새로운 물체" 스폰 ---
		FVector SpawnLocation;
		if ((SpawnLocation = GetRandomSpawnLocation()).IsZero())
		{
			UE_LOG(LogTemp, Warning, TEXT("AnomalyManager: '새 물체' 스폰 위치를 찾는 데 실패했습니다."));
			return;
		}

		AnomalyClass = GetRandomStationaryAnomalyClass();
		if (!AnomalyClass) return;

		AStationaryAnomaly* NewAnomaly = World->SpawnActor<AStationaryAnomaly>(
			AnomalyClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams
		);

		if (NewAnomaly)
		{
			EAnomalyType Type = EAnomalyType::EAT_Chasing; // '새 물체'는 '추적' 타입
			float Lifespan = StationaryAnomalyLifespan;
			NewAnomaly->InitializeAnomaly(Type, AnomalyEffect, Lifespan);
			UE_LOG(LogTemp, Log, TEXT("AnomalyManager: '새 물체' 이상현상 스폰."));
		}
	}
	else
	{
		// --- 4-B. "환경 변조" 스폰 ---
		// "사용 가능" 목록에서 랜덤하게 하나를 선택하고 제거
		int32 RandomIndex = FMath::RandRange(0, AvailableModifiableActors.Num() - 1);
		AActor* TargetActor = AvailableModifiableActors[RandomIndex];
		AvailableModifiableActors.RemoveAt(RandomIndex);

		if (!IsValid(TargetActor))
		{
			UE_LOG(LogTemp, Warning, TEXT("AnomalyManager: AvailableModifiableActors 배열에서 유효하지 않은 액터를 선택했습니다."));
			return;
		}

		// AStationaryAnomaly를 스폰 (위치는 TargetActor의 위치지만, InitializeFromActor가 덮어쓸 것임)
		AnomalyClass = BaseStationaryAnomalyClass;
		AStationaryAnomaly* NewAnomaly = World->SpawnActor<AStationaryAnomaly>(
			AnomalyClass, TargetActor->GetActorLocation(), TargetActor->GetActorRotation(), SpawnParams
		);

		if (NewAnomaly)
		{
			EAnomalyType Type = EAnomalyType::EAT_Disappearing; // '환경 변조'는 '패널티' 타입
			float Lifespan = StationaryAnomalyLifespan;

			// 원본 액터 숨김/복사/변조를 한 번에 처리
			NewAnomaly->InitializeFromActor(TargetActor, Type, AnomalyEffect, Lifespan);
			UE_LOG(LogTemp, Log, TEXT("AnomalyManager: '환경 변조' 이상현상 스폰. 대상: %s"), *TargetActor->GetName());
		}
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


FVector AAnomalyManager::GetRandomSpawnLocation()
{
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	
	// SpawnZone 비어 있는 경우 대비
    if (SpawnZones.Num() == 0)
	{
		UE_LOG(LogTemp, Log, TEXT("SpawnZone 비어있음"));
		return FVector::ZeroVector;
	}

	 // SpawnZone 하나를 랜덤으로 선택
	int32 BoxSelected = FMath::RandRange(0, SpawnZones.Num() - 1);
    AActor* Zone = SpawnZones[BoxSelected];
    if (!Zone)
	{
		UE_LOG(LogTemp, Log, TEXT("선택된 Zone이 nullptr"));
		return FVector::ZeroVector;
	}
 	FBox Box = Zone->GetComponentsBoundingBox(true);

	// 박스 내 랜덤 위치 할당
	const float X = FMath::RandRange(Box.Min.X, Box.Max.X);
    const float Y = FMath::RandRange(Box.Min.Y, Box.Max.Y);
    const float Z = Box.Max.Z;
	FVector RandomPoint(X, Y, Z);

	if (NavSys)
	{
    	FNavLocation NavPoint;

		// 검색 범위 내에서의 가까운 navmesh 찾기
		if (NavSys->ProjectPointToNavigation(RandomPoint, NavPoint, FVector(300.f,300.f,800.f)))
		{
			return NavPoint.Location;
		}

		// 선택된 존에서 주변에 완전히 랜덤한 nevmesh 위치(위 분기에서 못 찾았을 때)
		if (NavSys->GetRandomPointInNavigableRadius(Zone->GetActorLocation(), 1500.f, NavPoint))
		{
			return NavPoint.Location;
		}
	}

	// navmesh가 없으면
	return RandomPoint;
}


EAnomalyCategory AAnomalyManager::GetRandomAnomalyCategory() const
{
	// EAnomalyCategory에 정의된 범주 중 랜덤 선택
	// AC_None (0)을 제외하고 1부터 마지막 항목까지의 범위에서 랜덤 값을 고름

	// EAnomalyCategory::AC_Environmental 값을 가져옵니다 (enum의 마지막 항목이라고 가정).
	int32 MaxCategoryIndex = static_cast<int32>(EAnomalyCategory::EAC_Intrusion);
	if (MaxCategoryIndex <= 0)
	{
		return EAnomalyCategory::EAC_None; // enum이 비어있는 경우
	}

	int32 RandomIndex = FMath::RandRange(1, MaxCategoryIndex);
	//return static_cast<EAnomalyCategory>(RandomIndex);
	return EAnomalyCategory::EAC_Intrusion; // 테스트용으로 항상 Peculiarity 반환
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