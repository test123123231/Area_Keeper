#include "Anomaly/StationaryAnomaly.h"
#include "Character/PlayerCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Anomaly/ChasingAnomaly.h"
#include "Game/AreaKeeperGameState.h"
#include "Game/AnomalyManager.h"
#include "Components/AnomalousPropertyComponent.h"
#include "Engine/StaticMeshActor.h"


AStationaryAnomaly::AStationaryAnomaly()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AnomalyMesh"));
	RootComponent = Mesh;

	Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}


void AStationaryAnomaly::BeginPlay()
{
	Super::BeginPlay();

	if (Mesh)
	{
		DynamicMaterial = Mesh->CreateAndSetMaterialInstanceDynamic(0);
	}

	GameStateRef = GetWorld() ? GetWorld()->GetGameState<AAreaKeeperGameState>() : nullptr;
	AnomalyManagerRef = Cast<AAnomalyManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AAnomalyManager::StaticClass()));
}


// AnomalyManager가 스폰 직후 호출
void AStationaryAnomaly::InitializeAnomaly(EAnomalyType Type, EAnomalyCategory Category, float InLifespan)
{
	AnomalyType = Type;
	CorrectCategory = Category;

	// 미해결 타이머 시작
	GetWorld()->GetTimerManager().SetTimer(
		ResolveTimerHandle,
		this,
		&AStationaryAnomaly::OnResolveTimerExpired,
		InLifespan,
		false
	);
}


// IInteractableInterface 구현
void AStationaryAnomaly::Highlight_Implementation(bool bIsLooking)
{
	if (DynamicMaterial)
	{
		// 머티리얼에 "Color"라는 파라미터가 있어야 작동함
		DynamicMaterial->SetVectorParameterValue("Color", bIsLooking ? FLinearColor::Blue : FLinearColor::White);
	}
}


void AStationaryAnomaly::Interact_Implementation(APlayerCharacter* Interactor)
{
	if (Interactor)
	{
		// 플레이어에게 '소지' UI를 열도록 요청
		// PlayerCharacter의 TalismanRitual 함수를 호출
		Interactor->TalismanRitual(this);
	}
}


FString AStationaryAnomaly::GetInteractText_Implementation()
{
	return FString(TEXT("소지하기"));
}


void AStationaryAnomaly::ReturnLinkedActorToManager()
{
	if (LinkedComponent && AnomalyManagerRef.IsValid())
	{
		LinkedComponent->ResetToNormal();

		// 컴포넌트의 오너(AStaticMeshActor)를 가져옴
		AStaticMeshActor* OwnerActor = Cast<AStaticMeshActor>(LinkedComponent->GetOwner());
		if (OwnerActor)
		{
			// Manager에 액터를 "반납"
			AnomalyManagerRef->ReturnActorToAvailableList(OwnerActor);
		}
	}
}


// '소지' (TalismanRitual) 로직 
// '소지' UI에서 플레이어가 선택을 완료했을 때 호출됨
void AStationaryAnomaly::OnTalismanRitualFinished(EAnomalyCategory SelectedCategory)
{
	if (SelectedCategory == CorrectCategory)
	{
		// '소지' 성공
		// 패널티 스택 감소
		DecrementPenaltyStack();

		ReturnLinkedActorToManager();

		GetWorld()->GetTimerManager().ClearTimer(ResolveTimerHandle);
		Destroy();
		GameStateRef->IncrementAnomaliesSolved();
	}
	else
	{
		// '소지' 실패
		// 새 이상현상 즉시 스폰
		if (AnomalyManagerRef.IsValid())
			AnomalyManagerRef->SpawnStationaryAnomaly();
	}
}


// 미해결 타이머 만료
void AStationaryAnomaly::OnResolveTimerExpired()
{
	if (AnomalyType == EAnomalyType::EAT_Chasing && AnomalyManagerRef.IsValid())
	{
		AnomalyManagerRef->SpawnChasingAnomaly(GetActorLocation());
	}
	else // (AnomalyType == EAnomalyType::AT_Disappearing)
	{
		// 패널티 스택 증가
		IncrementPenaltyStack();
	}

	ReturnLinkedActorToManager();
	Destroy();
}


void AStationaryAnomaly::IncrementPenaltyStack()
{
	UE_LOG(LogTemp, Warning, TEXT("Incrementing Penalty Stack... (GameState N/A)"));
	if (GameStateRef.IsValid())
	{
		GameStateRef->IncrementPenaltyStack();
	}
}


void AStationaryAnomaly::DecrementPenaltyStack()
{
	UE_LOG(LogTemp, Warning, TEXT("Decrementing Penalty Stack... (GameState N/A)"));
	if (GameStateRef.IsValid())
	{
		GameStateRef->DecrementPenaltyStack();
	}
}


void AStationaryAnomaly::SpawnFailedAnomaly()
{
	UE_LOG(LogTemp, Warning, TEXT("소지 실패로 새 정지 이상 현상 추가"));
	if (AnomalyManagerRef.IsValid())
	{
		AnomalyManagerRef->SpawnStationaryAnomaly();
	}
}


/**
 * AnomalyManager가 호출하여, 이 퍼즐과 환경 변조 컴포넌트를 연결합니다.
 */
void AStationaryAnomaly::SetLinkedComponent(UAnomalousPropertyComponent* CompToLink)
{
	LinkedComponent = CompToLink;
}