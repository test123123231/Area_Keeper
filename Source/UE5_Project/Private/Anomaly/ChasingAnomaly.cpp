#include "Anomaly/ChasingAnomaly.h"
#include "Character/PlayerCharacter.h"
#include "Components/AttributeComponent.h"
#include "Components/CapsuleComponent.h"
#include "AIController.h"
#include "Perception/AISenseConfig_Sight.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Navigation/PathFollowingComponent.h"
#include "Game/AreaKeeperGameState.h"


AChasingAnomaly::AChasingAnomaly()
{
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	CurrentState = EChasingAnomalyState::EAS_Idle;
	PlayerTarget = nullptr;

	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	SightConfig->SightRadius = SightRadius;
	SightConfig->LoseSightRadius = LoseChaseRange;
	SightConfig->PeripheralVisionAngleDegrees = 90.f;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

	AIPerception->SetDominantSense(*SightConfig->GetSenseImplementation());
	AIPerception->ConfigureSense(*SightConfig);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxWalkSpeed = 0.f;
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AChasingAnomaly::OnAnomalyOverlap);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	RequiredToolType = EToolType::ETT_None;
}


void AChasingAnomaly::BeginPlay()
{
	Super::BeginPlay();

	AnomalyController = Cast<AAIController>(GetController());

	if (AIPerception)
	{
		AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &AChasingAnomaly::OnSeePawn);
	}

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (PlayerPawn)
	{
		float PlayerSpeed = PlayerPawn->GetMovementComponent()->GetMaxSpeed();
		ChasingSpeed = PlayerSpeed * 0.7f;
	}
	else
	{
		ChasingSpeed = 300.f;
	}

	GameStateRef = GetWorld() ? GetWorld()->GetGameState<AAreaKeeperGameState>() : nullptr;
}


void AChasingAnomaly::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 추적 상태일 때만 로직 실행
	if (CurrentState == EChasingAnomalyState::EAS_Chasing)
	{
		if (PlayerTarget == nullptr)
		{
			StopChasing(); // 혹시 모를 예외처리
			return;
		}

		// 플레이어와의 거리를 매 틱 계산
		const float DistanceToPlayer = GetDistanceTo(PlayerTarget.Get());

		// (요청 1) 설정한 '추적 포기 거리'보다 멀어지면 추적 중지
		if (DistanceToPlayer > LoseChaseRange)
		{
			StopChasing();
		}
		// '밀착 돌진 거리'보다 가까워지면
		else if (DistanceToPlayer < CloseAttackRange)
		{
			// NavMesh 길찾기를 멈추고 플레이어 방향으로 돌진
			MoveDirectlyToTarget(PlayerTarget->GetActorLocation());
		}
		// 기본 경우에는 일반적인 NavMesh 길찾기 사용
		else
		{
			MoveToTarget(PlayerTarget.Get());
		}
	}
}


void AChasingAnomaly::OnSeePawn(AActor* SeenActor, FAIStimulus Stimulus)
{
	if (SeenActor == nullptr || !SeenActor->ActorHasTag(FName("PlayerCharacter")))
	{
		return;
	}

	if (Stimulus.WasSuccessfullySensed())
	{
		// 플레이어를 '감지'했을 때 ChaseTarget 호출
		ChaseTarget(SeenActor);
	}
}


void AChasingAnomaly::ChaseTarget(AActor* Target)
{
	if (Target == nullptr) return;

	PlayerTarget = Target;

	if (CurrentState == EChasingAnomalyState::EAS_Idle)
	{
		CurrentState = EChasingAnomalyState::EAS_Chasing;
		GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
	}
}


void AChasingAnomaly::StopChasing()
{
	if (CurrentState == EChasingAnomalyState::EAS_Chasing)
	{
		PlayerTarget = nullptr;
		CurrentState = EChasingAnomalyState::EAS_Idle;
		GetCharacterMovement()->MaxWalkSpeed = 0.f;

		if (AnomalyController)
		{
			AnomalyController->StopMovement();
		}
	}
}


void AChasingAnomaly::MoveToTarget(AActor* Target)
{
	if (AnomalyController && Target)
	{
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(Target);
		MoveRequest.SetAcceptanceRadius(AcceptanceRadius);

		//  MoveTo가 경로를 계속 업데이트하도록 FPathFollowingRequestResult를 확인하지 않음
		// 매 틱 호출되므로 항상 최신 플레이어 위치로 경로를 갱신
		AnomalyController->MoveTo(MoveRequest);
	}
}

// NavMesh를 무시하고 타겟을 향해 직접 이동
void AChasingAnomaly::MoveDirectlyToTarget(const FVector& TargetLocation)
{
	if (AnomalyController)
	{
		// 진행 중인 MoveTo 요청을 먼저 중단
		AnomalyController->StopMovement();
	}

	// 플레이어를 향하는 방향 벡터 계산
	FVector Direction = TargetLocation - GetActorLocation();
	Direction.Normalize();

	// 해당 방향으로 이동 입력 추가
	AddMovementInput(Direction, 1.0f);
}


void AChasingAnomaly::OnAnomalyOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
	if (!Player) return;
	
	//무적 상태일 때 이벤트와 연결
	if (Player->getIsInvincible())
	{
    	if (!Player->OnInvincibilityEnd.IsAlreadyBound(this, &AChasingAnomaly::OnPlayerInvincibilityEnd))
    	{
        	Player->OnInvincibilityEnd.AddDynamic(this, &AChasingAnomaly::OnPlayerInvincibilityEnd);
    	}
    	return;
	}
	
	if(Player->IsAlive())
	{
		ApplyDamageToPlayer(Player);
	}
}


void AChasingAnomaly::OnPlayerInvincibilityEnd(APlayerCharacter* Player)
{
    if (!Player) return;
	//범위 밖에 있으면 리턴
    if (!GetCapsuleComponent()->IsOverlappingActor(Player)) return;
	// 죽었거나 또 무적이면 리턴
    if (!Player->IsAlive() || Player->getIsInvincible()) return;

    ApplyDamageToPlayer(Player);

    // 한 번 사용 후 해제
    if (Player->OnInvincibilityEnd.IsAlreadyBound(this, &AChasingAnomaly::OnPlayerInvincibilityEnd))
    {
        Player->OnInvincibilityEnd.RemoveDynamic(this, &AChasingAnomaly::OnPlayerInvincibilityEnd);
    }
}


void AChasingAnomaly::ApplyDamageToPlayer(APlayerCharacter* Player)
{
	if (!Player || !Player->GetAttributes()) return;

	Player->HandleDamage(1.0f);
	Banish();
	GameStateRef->IncrementChasingHits();
}


void AChasingAnomaly::Banish()
{
	if (AnomalyController)
	{
		AnomalyController->StopMovement();
	}
	Destroy();
}


// IInteractableInterface 구현
void AChasingAnomaly::Highlight_Implementation(bool bOn)
{
	// 메쉬의 머티리얼 또는 외곽선 효과 적용 (구현 필요)
}


void AChasingAnomaly::Interact_Implementation(APlayerCharacter* Interactor)
{
	// PlayerCharacter::OnInteractPressed에서 로직을 처리하므로 여기서는 비워둠
}


FString AChasingAnomaly::GetInteractText_Implementation()
{
	return FString(TEXT("도구 사용"));
}

void AChasingAnomaly::SetHighlightColor(bool bIsCompatible)
{
	USkeletalMeshComponent* SkeletalMesh = GetMesh();

	if (SkeletalMesh)
	{
		UMaterialInterface* BaseMaterial = SkeletalMesh->GetMaterial(0);
		if (BaseMaterial)
		{
			UMaterialInstanceDynamic* DynMat = Cast<UMaterialInstanceDynamic>(BaseMaterial);

			if (!DynMat)
			{
				DynMat = UMaterialInstanceDynamic::Create(BaseMaterial, this);
				SkeletalMesh->SetMaterial(0, DynMat);
			}

			if (DynMat)
			{
				if (bIsCompatible)
				{
					// 파란색 외곽선
					DynMat->SetVectorParameterValue(FName("Color"), FLinearColor(0.5f, 0.5f, 3.0f, 1.0f));
				}
				else
				{
					// 빨간색 외곽선
					DynMat->SetVectorParameterValue(FName("Color"), FLinearColor(3.0f, 0.5f, 0.5f, 1.0f));
				}
			}
		}
	}
}

void AChasingAnomaly::ClearHighlight()
{
	USkeletalMeshComponent* SkeletalMesh = GetMesh();
	if (SkeletalMesh)
	{
		UMaterialInterface* BaseMaterial = SkeletalMesh->GetMaterial(0);
		if (BaseMaterial)
		{
			UMaterialInstanceDynamic* DynMat = Cast<UMaterialInstanceDynamic>(BaseMaterial);
			if (DynMat)
			{
				// 검은색(0,0,0) = 외곽선 꺼짐
				DynMat->SetVectorParameterValue(FName("Color"), FLinearColor(0.0f, 0.0f, 0.0f, 1.0f));
			}
		}
	}
}