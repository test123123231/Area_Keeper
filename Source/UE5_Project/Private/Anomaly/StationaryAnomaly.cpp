#include "Anomaly/StationaryAnomaly.h"
#include "Character/PlayerCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Anomaly/ChasingAnomaly.h"
#include "Game/AreaKeeperGameState.h"
#include "Game/AnomalyManager.h"
#include "Engine/StaticMeshActor.h"
#include "Item/ItemBase.h"
#include "Components/BoxComponent.h"


AStationaryAnomaly::AStationaryAnomaly()
{
	PrimaryActorTick.bCanEverTick = false;

	// [수정] 메쉬 컴포넌트는 루트에 붙이고, 기본적으로 숨김+충돌 없음
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AnomalyMesh"));
	RootComponent = Mesh;

	// [수정] 루트 컴포넌트를 'InteractVolume'으로 설정
	InteractVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractVolume"));
	InteractVolume->SetupAttachment(RootComponent);
	InteractVolume->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
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

	GetWorld()->GetTimerManager().SetTimer(
		ResolveTimerHandle,
		this,
		&AStationaryAnomaly::OnResolveTimerExpired,
		InLifespan,
		false
	);
}


// AAnomalyManager가 호출하여 '환경 변조' 이상현상을 설정
void AStationaryAnomaly::InitializeFromActor(AActor* ActorToReplace, EAnomalyType Type, EAnomalyCategory Category, float InLifespan)
{
	if (!ActorToReplace)
	{
		UE_LOG(LogTemp, Error, TEXT("AStationaryAnomaly: InitializeFromActor에 유효하지 않은 ActorToReplace가 전달되었습니다."));
		Destroy();
		return;
	}

	Mesh->SetHiddenInGame(true);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	InteractVolume->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	// 원본 액터 정보 저장 (메쉬 복제 안 함)
	OriginalActor = ActorToReplace;
	OriginalLocation = ActorToReplace->GetActorLocation();
	OriginalScale = ActorToReplace->GetActorScale3D();

	SetComponentsVisibilityCollision(ActorToReplace, false);

	// AStationaryAnomaly의 InteractVolume을 원본 액터 크기에 맞춤
	UpdateInteractionVolume();

	// 이 AStationaryAnomaly를 원본 액터의 위치로 이동
	SetActorLocation(OriginalLocation);
	SetActorScale3D(OriginalScale); // (스케일은 1,1,1로 유지하는 게 나을 수 있음)

	// 일반 초기화 로직 실행
	InitializeAnomaly(Type, Category, InLifespan);

	// 시각적 변조 시작
	switch (CorrectCategory)
	{
	case EAnomalyCategory::EAC_Gigantism:
		StartGigantism(1.5f);
		break;
	case EAnomalyCategory::EAC_Levitation:
		StartLevitation(150.f);
		break;
	case EAnomalyCategory::EAC_Peculiarity:
		StartPeculiarity();
		break;
	}
}


void AStationaryAnomaly::SetComponentsVisibilityCollision(AActor* ActorToReplace, bool bSet)
{
	// ActorToReploace의 모든 컴포넌트에 ECC_Visibility 충돌 비활성화
	TArray<UActorComponent*> Components = ActorToReplace->GetComponents().Array();
	for (UActorComponent* Component : Components)
	{
		UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(Component);
		if (PrimComp)
		{
			if (bSet)
			{
				PrimComp->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
			}
			else
			{
				PrimComp->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
			}
		}
	}
}


// InteractVolume 크기 조절 함수
void AStationaryAnomaly::UpdateInteractionVolume()
{
	if (!OriginalActor) return;

	// 원본 액터의 '로컬 공간' 바운딩 박스를 계산합니다.
	// (모든 자식 컴포넌트 포함, 콜리전이 꺼진 메쉬도 포함)
	const FBox LocalBounds = OriginalActor->CalculateComponentsBoundingBoxInLocalSpace(true);

	// 바운딩 박스의 '중심점'과 '반경(Extent)'을 가져옵니다.
	const FVector LocalCenter = LocalBounds.GetCenter();
	const FVector LocalExtent = LocalBounds.GetExtent();

	// InteractVolume의 '상대 위치'를 바운딩 박스의 중심점으로 설정합니다.
	//    (이것이 Z축 문제를 해결합니다)
	InteractVolume->SetRelativeLocation(LocalCenter);

	// InteractVolume의 '크기'를 바운딩 박스의 크기로 설정합니다.
	InteractVolume->SetBoxExtent(LocalExtent);
}


// 변조 함수가 'OriginalActor'와 'this'를 모두 움직임
void AStationaryAnomaly::StartLevitation(float FloatHeight)
{
	UE_LOG(LogTemp, Warning, TEXT("Starting Levitation..."));

	UPrimitiveComponent* RootComp = Cast<UPrimitiveComponent>(OriginalActor->GetRootComponent());
	if (RootComp && RootComp->GetMobility() == EComponentMobility::Static)
	{
		RootComp->SetMobility(EComponentMobility::Movable);
	}

	FVector NewLocation = OriginalLocation + FVector(0.0f, 0.0f, FloatHeight);

	if (OriginalActor) OriginalActor->SetActorLocation(NewLocation);
	this->SetActorLocation(NewLocation); // 보이지 않는 자신(InteractVolume)도 함께 이동
}


void AStationaryAnomaly::StartGigantism(float ScaleMultiplier)
{
	UE_LOG(LogTemp, Warning, TEXT("Starting Gigantism..."));

	UPrimitiveComponent* RootComp = Cast<UPrimitiveComponent>(OriginalActor->GetRootComponent());
	if (RootComp && RootComp->GetMobility() == EComponentMobility::Static)
	{
		RootComp->SetMobility(EComponentMobility::Movable);
	}

	FVector NewScale = OriginalScale * ScaleMultiplier;

	if (OriginalActor) OriginalActor->SetActorScale3D(NewScale);
	this->SetActorScale3D(NewScale); // InteractVolume도 함께 커짐

	// (스케일이 변경되었으므로 InteractVolume의 BoxExtent도 재조정 필요)
	// UpdateInteractionVolume();
}


void AStationaryAnomaly::StartPeculiarity()
{
	UE_LOG(LogTemp, Warning, TEXT("Starting Peculiarity..."));
	if (!OriginalActor) return;

	// [수정] '자신'이 아닌 'OriginalActor'의 머티리얼을 변경
	TArray<UStaticMeshComponent*> MeshComponents;
	OriginalActor->GetComponents<UStaticMeshComponent>(MeshComponents);

	for (UStaticMeshComponent* MeshComp : MeshComponents)
	{
		if (MeshComp)
		{
			for (int32 i = 0; i < MeshComp->GetNumMaterials(); ++i)
			{
				// (AnomalousPropertyComponent의 로직과 동일)
				DynamicMaterial = MeshComp->CreateAndSetMaterialInstanceDynamic(i);
				if (DynamicMaterial)
				{
					DynamicMaterial->SetVectorParameterValue("Color", FLinearColor::Red);
				}
			}
		}
	}
}


// 원본 액터 복원 및 매니저에 반납
void AStationaryAnomaly::ResetToNormal()
{
	UE_LOG(LogTemp, Warning, TEXT("Resetting to Normal..."));
	if (OriginalActor)
	{
		UPrimitiveComponent* RootComp = Cast<UPrimitiveComponent>(OriginalActor->GetRootComponent());
		if (RootComp && RootComp->GetMobility() == EComponentMobility::Static)
		{
			RootComp->SetMobility(EComponentMobility::Movable);
		}

		// 1. 원본 액터를 원상복구 (위치, 스케일, 충돌)
		OriginalActor->SetActorLocation(OriginalLocation);
		OriginalActor->SetActorScale3D(OriginalScale);
		OriginalActor->SetActorEnableCollision(true);

		// 2. 만약 '특이사항'이었다면 머티리얼도 복원
		if (CorrectCategory == EAnomalyCategory::EAC_Peculiarity)
		{
			// (머티리얼 복원 로직 필요 - AnomalousPropertyComponent의 OriginalMaterials 캐시 참조)
			// 간단하게는, OriginalActor를 Destroy()하고 새 인스턴스를 스폰할 수도 있습니다.
			// 여기서는 일단 생략합니다.
		}

		if (AnomalyManagerRef.IsValid())
		{
			AnomalyManagerRef->ReturnActorToAvailableList(OriginalActor);
		}

		SetComponentsVisibilityCollision(OriginalActor, true);
	}
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


// '소지' (TalismanRitual) 로직 
// '소지' UI에서 플레이어가 선택을 완료했을 때 호출됨
void AStationaryAnomaly::OnTalismanRitualFinished(EAnomalyCategory SelectedCategory)
{
	if (SelectedCategory == CorrectCategory)
	{
		// '소지' 성공
		// 패널티 스택 감소
		DecrementPenaltyStack();

		ResetToNormal();

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

	ResetToNormal();
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