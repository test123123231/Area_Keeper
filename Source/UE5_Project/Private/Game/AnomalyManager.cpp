#include "Game/AnomalyManager.h"
#include "Anomaly/StationaryAnomaly.h"
#include "Anomaly/ChasingAnomaly.h"
#include "NavigationSystem.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Game/AreaKeeperGameState.h"
#include "Components/AnomalousPropertyComponent.h"
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
	if (ModifiableMeshActors.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("AnomalyManager: ModifiableMeshActors 배열이 비어있습니다! 에디터에서 액터를 할당해야 합니다."));
	}

	InitializeAvailableActorList();
}


void AAnomalyManager::InitializeAvailableActorList()
{
	// 마스터 리스트를 사용 가능 리스트로 그대로 복사
	AvailableModifiableActors = ModifiableMeshActors;

	if (AvailableModifiableActors.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("AnomalyManager: ModifiableMeshActors 배열이 비어있습니다!"));
	}
}


void AAnomalyManager::ReturnActorToAvailableList(AStaticMeshActor* ActorToReturn)
{
	if (ActorToReturn && ModifiableMeshActors.Contains(ActorToReturn))
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


bool AAnomalyManager::SetupAnomalyVisuals(EAnomalyCategory AnomalyEffect, FVector& OutSpawnLocation, UAnomalousPropertyComponent*& OutAnomalousComp)
{
	OutAnomalousComp = nullptr;
	OutSpawnLocation = FVector::ZeroVector;

	if (AnomalyEffect == EAnomalyCategory::EAC_Intrusion)
	{
		if (!GetRandomSpawnLocation(OutSpawnLocation))
		{
			UE_LOG(LogTemp, Warning, TEXT("AnomalyManager: '새 물체' 스폰 위치를 찾는 데 실패했습니다."));
			return false;
		}
		return true;
	}
	else
	{
		// 1. "사용 가능" 목록에서 랜덤하게 하나를 선택
		int32 RandomIndex = FMath::RandRange(0, AvailableModifiableActors.Num() - 1);
		AStaticMeshActor* TargetActor = AvailableModifiableActors[RandomIndex];

		// 2. [핵심] 사용했으므로 "사용 가능" 목록에서 즉시 제거
		AvailableModifiableActors.RemoveAt(RandomIndex);

		if (!IsValid(TargetActor))
		{
			UE_LOG(LogTemp, Warning, TEXT("AnomalyManager: AvailableModifiableActors 배열에서 유효하지 않은 액터를 선택했습니다."));
			// 액터가 파괴되었다면 ReturnActorToAvailableList를 호출할 수 없으므로 목록에서 영구 제거됨
			return false;
		}

		// 3. 대상 액터에 컴포넌트를 찾거나 동적으로 추가
		UAnomalousPropertyComponent* Comp = TargetActor->FindComponentByClass<UAnomalousPropertyComponent>();
		if (!Comp)
		{
			Comp = NewObject<UAnomalousPropertyComponent>(TargetActor, TEXT("AnomalousProperty"));
			if (Comp) Comp->RegisterComponent();
		}

		if (!Comp)
		{
			// 컴포넌트 생성 실패 시, 액터를 목록에 되돌려놓고 실패 반환
			AvailableModifiableActors.Add(TargetActor);
			return false;
		}

		// 4. 출력 변수 설정 및 시각 효과 적용
		OutSpawnLocation = TargetActor->GetActorLocation();
		OutAnomalousComp = Comp;

		switch (AnomalyEffect)
		{
		case EAnomalyCategory::EAC_Gigantism:
			OutAnomalousComp->StartGigantism(1.5f);
			break;
		case EAnomalyCategory::EAC_Levitation:
			OutAnomalousComp->StartLevitation(150.f);
			break;
		case EAnomalyCategory::EAC_Peculiarity:
			OutAnomalousComp->StartPeculiarity();
			break;
		}
		return true; // 성공
	}
}


void AAnomalyManager::SpawnAnomalyLogic(EAnomalyCategory AnomalyEffect, const FVector& SpawnLocation, UAnomalousPropertyComponent* AnomalousComp)
{
	// 리스트가 비어있으면 오류 로그 출력 후 종료
	if (StationaryAnomalyClassList.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("AnomalyManager: StationaryAnomalyClassList가 비어있습니다! 에디터에서 설정하세요."));
		return;
	}

	UWorld* World = GetWorld();
	if (!World) return;

	// (참고: AC_Intrusion용과 환경변조용 BP 클래스를 분리하는 것이 좋습니다)
	TSubclassOf<AStationaryAnomaly> AnomalyClass = GetRandomStationaryAnomalyClass();
	if (!AnomalyClass) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AStationaryAnomaly* NewAnomaly = World->SpawnActor<AStationaryAnomaly>(
		AnomalyClass,
		SpawnLocation,
		FRotator::ZeroRotator,
		SpawnParams
	);

	if (NewAnomaly)
	{
		// 4. 스폰된 퍼즐에 정답과 시한부 설정
		//EAnomalyType Type = GetRandomAnomalyType();
		EAnomalyType Type;
		if (AnomalyEffect == EAnomalyCategory::EAC_Intrusion)
		{
			// "새로운 물체"는 실패 시 "쫓아오는 이상현상" 스폰
			Type = EAnomalyType::EAT_Chasing;
		}
		else
		{
			// "환경 변조" (거대화, 공중부양, 특이사항)는 실패 시 "패널티 스택"
			Type = EAnomalyType::EAT_Disappearing;
		}

		float Lifespan = StationaryAnomalyLifespan;
		NewAnomaly->InitializeAnomaly(Type, AnomalyEffect, Lifespan);

		// 5. [핵심] 환경 변조였다면, 퍼즐과 컴포넌트를 연결
		if (AnomalousComp)
		{
			NewAnomaly->SetLinkedComponent(AnomalousComp);
			// (선택적) 퍼즐 액터를 변조된 액터에 붙여서 같이 움직이게 함
			// NewAnomaly->AttachToActor(AnomalousComp->GetOwner(), FAttachmentTransformRules::KeepWorldTransform);
		}

		UE_LOG(LogTemp, Log, TEXT("AnomalyManager: '정지된 이상현상' 스폰. 범주: %s"), *UEnum::GetValueAsString(AnomalyEffect));
	}
}


void AAnomalyManager::SpawnStationaryAnomaly()
{
	UE_LOG(LogTemp, Log, TEXT("AnomalyManager: '정지된 이상현상' 스폰 시도"));

	// 어떤 이상현상을 스폰할지 랜덤하게 결정
	EAnomalyCategory AnomalyEffect = GetRandomAnomalyCategory();

	// 만약 '환경 변조'가 걸렸는데, 사용 가능한 액터가 없다면...
	if (AnomalyEffect != EAnomalyCategory::EAC_Intrusion && AvailableModifiableActors.Num() == 0)
	{
		// "새로운 물체 등장"으로 강제 변경 (폴백)
		AnomalyEffect = EAnomalyCategory::EAC_Intrusion;
		UE_LOG(LogTemp, Log, TEXT("AnomalyManager: 변조 가능 액터 없음. '새 물체'로 강제 전환."));
	}

	// 헬퍼 함수에 필요한 변수 선언
	FVector SpawnLocation;
	UAnomalousPropertyComponent* AnomalousComp = nullptr;

	// "시각 효과" 준비
	bool bVisualsReady = SetupAnomalyVisuals(AnomalyEffect, SpawnLocation, AnomalousComp);

	// 시각 효과가 준비되었으면 "논리" 스폰
	if (bVisualsReady)
	{
		SpawnAnomalyLogic(AnomalyEffect, SpawnLocation, AnomalousComp);
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
	return (FMath::RandBool()) ? EAnomalyType::EAT_Disappearing : EAnomalyType::EAT_Chasing;
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