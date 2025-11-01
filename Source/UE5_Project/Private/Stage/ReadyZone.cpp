#include "Stage/ReadyZone.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Interfaces/StageTimerInterface.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameModeBase.h"  


AReadyZone::AReadyZone()
{
    PrimaryActorTick.bCanEverTick = false;

    // BoxComponent 생성 및 루트 설정
    ZoneBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ZoneBox"));
    RootComponent = ZoneBox;

    // 트리거 전용 콜리전 설정
    ZoneBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    ZoneBox->SetCollisionProfileName(TEXT("Trigger"));
    ZoneBox->SetGenerateOverlapEvents(true);

    // OverlapEnd 바인딩
    ZoneBox->OnComponentEndOverlap.AddDynamic(this, &AReadyZone::OnOverlapEnd);
}

void AReadyZone::BeginPlay()
{
    Super::BeginPlay();
}

void AReadyZone::OnOverlapEnd(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex
)
{
    // 플레이어만 감지 (ACharacter 기반)
    if (!OtherActor || !OtherActor->IsA(ACharacter::StaticClass()))
        return;

    // GameMode 가져오기
    if (UWorld* World = GetWorld())
    {
        AGameModeBase* GM = UGameplayStatics::GetGameMode(World);
        if (GM && GM->GetClass()->ImplementsInterface(UStageTimerInterface::StaticClass()))
        {
            IStageTimerInterface::Execute_StartStageTimer(GM);
            UE_LOG(LogTemp, Display, TEXT("ReadyZone: EndOverlap → StartStageTimer"));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("ReadyZone: GM null or no interface. GM=%s"),
                   GM ? *GM->GetName() : TEXT("null"));
        }
    }
}
