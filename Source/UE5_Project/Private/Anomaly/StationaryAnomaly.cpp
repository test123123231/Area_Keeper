// Fill out your copyright notice in the Description page of Project Settings.

#include "Anomaly/StationaryAnomaly.h"
#include "Character/PlayerCharacter.h" // APlayerCharacter
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Anomaly/ChasingAnomaly.h" // 쫓아오는 이상현상

// TODO: GameState 및 AnomalyManager 구현 후 주석 해제
// #include "Game/HousekeeperGameState.h"
// #include "Game/AnomalyManager.h"

AStationaryAnomaly::AStationaryAnomaly()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AnomalyMesh"));
	RootComponent = Mesh;

	// '소지' 상호작용을 위해 Visibility 채널에 응답
	Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

void AStationaryAnomaly::BeginPlay()
{
	Super::BeginPlay();

}

/** AnomalyManager가 스폰 직후 호출 */
void AStationaryAnomaly::InitializeAnomaly(EAnomalyType Type, EAnomalyCategory Category, float InLifespan)
{
	AnomalyType = Type;
	CorrectCategory = Category;

	// (SRS 1.1.2) 미해결 타이머 시작
	GetWorld()->GetTimerManager().SetTimer(
		ResolveTimerHandle,
		this,
		&AStationaryAnomaly::OnResolveTimerExpired,
		InLifespan,
		false
	);
}

// --- IInteractableInterface 구현 ---

void AStationaryAnomaly::Highlight_Implementation(bool bOn)
{
	// AItemBase와 유사하게 머티리얼 또는 외곽선 효과 적용
	// (예: Mesh->SetRenderCustomDepth(bOn);)
}

void AStationaryAnomaly::Interact_Implementation(APlayerCharacter* Interactor)
{
	if (Interactor)
	{
		// (SRS 4.1.2) 플레이어에게 '소지' UI를 열도록 요청
		Interactor->StartExorcism(this);
	}
}

FString AStationaryAnomaly::GetInteractText_Implementation()
{
	return FString(TEXT("소지하기"));
}

// --- '소지' (Exorcism) 로직 ---

/** '소지' UI에서 플레이어가 선택을 완료했을 때 호출됨 */
void AStationaryAnomaly::OnExorcismFinished(EAnomalyCategory SelectedCategory)
{
	if (SelectedCategory == CorrectCategory)
	{
		// (SRS 4.1.4) '소지' 성공
		UE_LOG(LogTemp, Warning, TEXT("Exorcism Success!"));

		// (SRS 5.1.4) 패널티 스택 감소
		DecrementPenaltyStack();

		// 미해결 타이머 정지
		GetWorld()->GetTimerManager().ClearTimer(ResolveTimerHandle);

		// 이상현상 파괴
		Destroy();
	}
	else
	{
		// (SRS 4.1.5) '소지' 실패
		UE_LOG(LogTemp, Warning, TEXT("Exorcism Failed!"));

		// 새 이상현상 즉시 스폰
		SpawnFailedAnomaly();

		// 이 이상현상은 사라지지 않고 타이머가 다 될 때까지 유지됨
	}
}

/** (SRS 2.1.1, 3.1.1) 미해결 타이머 만료 */
void AStationaryAnomaly::OnResolveTimerExpired()
{
	UE_LOG(LogTemp, Warning, TEXT("Anomaly resolve timer expired."));

	if (AnomalyType == EAnomalyType::AT_Chasing)
	{
		// (SRS 2.1.1) 쫓아오는 이상현상 스폰
		SpawnChasingAnomaly();
	}
	else // (AnomalyType == EAnomalyType::AT_Disappearing)
	{
		// (SRS 3.1.2) 패널티 스택 증가
		IncrementPenaltyStack();
	}

	// (SRS 3.1.1) 타이머가 만료되면 자신은 파괴됨
	Destroy();
}


// --- 헬퍼 함수 (GameMode/GameState 필요) ---

/*
// TODO: AHousekeeperGameState 구현 후 주석 해제
AHousekeeperGameState* AStationaryAnomaly::GetHousekeeperGameState()
{
	return GetWorld() ? GetWorld()->GetGameState<AHousekeeperGameState>() : nullptr;
}
*/

/*
// TODO: AAnomalyManager 구현 후 주석 해제
AAnomalyManager* AStationaryAnomaly::GetAnomalyManager()
{
	return Cast<AAnomalyManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AAnomalyManager::StaticClass()));
}
*/

void AStationaryAnomaly::IncrementPenaltyStack()
{
	UE_LOG(LogTemp, Warning, TEXT("Incrementing Penalty Stack... (Not implemented)"));
	// TODO: GameState 구현 후 주석 해제
	// if (AHousekeeperGameState* GS = GetHousekeeperGameState())
	// {
	// 	GS->IncrementPenaltyStack();
	// }
}

void AStationaryAnomaly::DecrementPenaltyStack()
{
	UE_LOG(LogTemp, Warning, TEXT("Decrementing Penalty Stack... (Not implemented)"));
	// TODO: GameState 구현 후 주석 해제
	// if (AHousekeeperGameState* GS = GetHousekeeperGameState())
	// {
	// 	GS->DecrementPenaltyStack();
	// }
}

void AStationaryAnomaly::SpawnFailedAnomaly()
{
	UE_LOG(LogTemp, Warning, TEXT("Spawning new anomaly due to fail... (Not implemented)"));
	// TODO: AnomalyManager 구현 후 주석 해제
	// if (AAnomalyManager* AM = GetAnomalyManager())
	// {
	// 	AM->SpawnAnomaly(); // AnomalyManager에 즉시 스폰 함수 필요
	// }
}

void AStationaryAnomaly::SpawnChasingAnomaly()
{
	UE_LOG(LogTemp, Warning, TEXT("Spawning Chasing Anomaly..."));

	UWorld* World = GetWorld();
	if (World)
	{
		World->SpawnActor<AChasingAnomaly>(
			AChasingAnomaly::StaticClass(),
			GetActorLocation(),
			GetActorRotation()
		);
	}
}
