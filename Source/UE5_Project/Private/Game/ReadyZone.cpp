#include "Game/ReadyZone.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Character/PlayerCharacter.h"
#include "Game/AreaKeeperGameState.h"


AReadyZone::AReadyZone()
{
	PrimaryActorTick.bCanEverTick = false;
	bTimerHasBeenTriggered = false;

	// 트리거 볼륨 생성 및 루트 컴포넌트로 설정
	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	RootComponent = TriggerVolume;

	// 볼륨이 'Overlap' 이벤트만 감지하도록 설정
	TriggerVolume->SetCollisionProfileName(TEXT("Trigger"));
}


void AReadyZone::BeginPlay()
{
	Super::BeginPlay();

	// GameState 참조 캐시
	GameStateRef = GetWorld()->GetGameState<AAreaKeeperGameState>();
	if (!GameStateRef.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("AReadyZone: AAreaKeeperGameState를 찾을 수 없습니다!"));
	}

	// OverlapEnd 이벤트에 함수 바인딩
	TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &AReadyZone::OnOverlapEnd);
}


void AReadyZone::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// 타이머가 이미 시작되었다면 아무것도 하지 않음
	if (bTimerHasBeenTriggered)
	{
		return;
	}

	// 나간 액터가 플레이어인지 확인
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
	if (Player)
	{
		UE_LOG(LogTemp, Log, TEXT("ReadyZone: 플레이어가 구역을 떠났습니다. 60초 준비 타이머를 시작합니다."));

		// GameState의 함수 호출
		if (GameStateRef.IsValid())
		{
			GameStateRef->StartReadyZoneTimer();
			bTimerHasBeenTriggered = true; // 타이머가 시작되었음을 표시 (중복 방지)
		}
	}
}
