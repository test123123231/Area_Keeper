#include "Game/AreaKeeperGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Game/AnomalyManager.h"


AAreaKeeperGameState::AAreaKeeperGameState()
{
	// 타이머를 관리하기 위해 매 틱 실행되어야 합니다.
	PrimaryActorTick.bCanEverTick = true;

	// 초기 상태 설정
	CurrentPenaltyStack = 0;
	ReadyZoneTimer = 0.0f;
	GameTimer = 0.0f;
	CurrentAnomalySpawnInterval = InitialAnomalySpawnInterval;
	CurrentPlayState = EAreaKeeperPlayState::EPS_WaitingToStart;
}


void AAreaKeeperGameState::SetCurrentAnomalySpawnInterval()
{
	// 게임 시간에 비례하여 스폰 주기 단축
	CurrentAnomalySpawnInterval = FMath::Lerp(
		InitialAnomalySpawnInterval,
		FinalAnomalySpawnInterval,
		FMath::Clamp(GameTimer / TotalGameTime, 0.0f, 1.0f)
	);
}


void AAreaKeeperGameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 현재 게임 상태에 따라 다른 로직 수행
	switch (CurrentPlayState)
	{
		// 준비 구역 대기 상태
	case EAreaKeeperPlayState::EPS_WaitingToStart:
		// 준비 타이머 카운트다운 (StartReadyZoneTimer가 호출된 이후)
		if (ReadyZoneTimer > 0.0f)
		{
			ReadyZoneTimer -= DeltaTime;
			if (ReadyZoneTimer <= 0.0f)
			{
				// 준비 시간 종료, 메인 게임 시작
				CurrentPlayState = EAreaKeeperPlayState::EPS_InProgress;
				UE_LOG(LogTemp, Log, TEXT("GameState: 준비 시간 종료. 메인 게임을 시작합니다."));

				// AnomalyManager에게 스폰 시작을 알림
				if (AnomalyManagerRef.IsValid())
				{
					AnomalyManagerRef->StartSpawning();
				}
			}
		}
		break;

		// 메인 게임 플레이 상태
	case EAreaKeeperPlayState::EPS_InProgress:
		// 메인 게임 타이머 카운트업
		GameTimer += DeltaTime;

		SetCurrentAnomalySpawnInterval();

		//게임 클리어 조건 확인
		if (GameTimer >= TotalGameTime)
		{
			CurrentPlayState = EAreaKeeperPlayState::EPS_GameFinished;

			// BP GameMode가 바인딩할 수 있도록 이벤트를 브로드캐스트합니다.
			OnGameClear.Broadcast();
		}
		break;

		// 게임 종료 상태
	case EAreaKeeperPlayState::EPS_GameFinished:
		// 게임이 끝나면 Tick 로직 중지 (옵션: 틱 비활성화)
		// SetActorTickEnabled(false); 
		break;
	}
}


// 패널티 시스템
void AAreaKeeperGameState::IncrementPenaltyStack()
{
	if (CurrentPlayState != EAreaKeeperPlayState::EPS_InProgress) return; // 게임 중이 아니면 스택 변경 안 함

	// 0 미만, 5 초과 방지
	const int32 NewStack = FMath::Clamp(CurrentPenaltyStack + 1, 0, MaxPenaltyStack);

	if (NewStack != CurrentPenaltyStack)
	{
		CurrentPenaltyStack = NewStack;
		BroadcastPenaltyStackChange(); // 델리게이트 호출
	}

	// 게임 오버 조건 확인
	if (CurrentPenaltyStack >= MaxPenaltyStack)
	{
		CurrentPlayState = EAreaKeeperPlayState::EPS_GameFinished;

		// BP GameMode가 바인딩할 수 있도록 이벤트를 브로드캐스트
		OnGameOver.Broadcast(false); // (false: 체력 0 아님)
	}
}


void AAreaKeeperGameState::DecrementPenaltyStack()
{
	if (CurrentPlayState != EAreaKeeperPlayState::EPS_InProgress) return; // 게임 중이 아니면 스택 변경 안 함

	// (SRS 5.1.5) 0 미만 방지
	const int32 NewStack = FMath::Clamp(CurrentPenaltyStack - 1, 0, MaxPenaltyStack);

	if (NewStack != CurrentPenaltyStack)
	{
		CurrentPenaltyStack = NewStack;
		BroadcastPenaltyStackChange(); // 델리게이트 호출
	}
}


void AAreaKeeperGameState::BroadcastPenaltyStackChange()
{
	// UI가 바인딩할 델리게이트를 브로드캐스트합니다.
	OnPenaltyStackChanged.Broadcast(CurrentPenaltyStack);

	// (구현 필요) 여기에 스택 숫자에 따른 패널티 효과(소리, 비네트, 속도 저하)를
	// 플레이어 컨트롤러나 캐릭터에 적용하는 로직을 추가할 수 있습니다.
}


// 타이머 및 게임 흐름
void AAreaKeeperGameState::StartReadyZoneTimer()
{
	if (CurrentPlayState == EAreaKeeperPlayState::EPS_WaitingToStart && ReadyZoneTimer <= 0.0f)
	{
		UE_LOG(LogTemp, Log, TEXT("GameState: 플레이어가 준비 구역을 떠났습니다. %f초 준비 타이머를 시작합니다."), InitialReadyZoneTime);
		ReadyZoneTimer = InitialReadyZoneTime;

		// AnomalyManager 참조 캐시
		CacheManagerReferences();
	}
}


void AAreaKeeperGameState::CacheManagerReferences()
{
	if (!AnomalyManagerRef.IsValid())
	{
		AActor* FoundManager = UGameplayStatics::GetActorOfClass(this, AAnomalyManager::StaticClass());
		AnomalyManagerRef = Cast<AAnomalyManager>(FoundManager);

		if (!AnomalyManagerRef.IsValid())
		{
			UE_LOG(LogTemp, Error, TEXT("GameState: 레벨에서 AnomalyManager를 찾을 수 없습니다! BP_AnomalyManager를 레벨에 배치했는지 확인하세요."));
		}
	}
}

