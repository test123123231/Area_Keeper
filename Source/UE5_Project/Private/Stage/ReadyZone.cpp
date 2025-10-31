
#include "ReadyZone.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AReadyZone::AReadyZone()
{
 	ZoneBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ZoneBox"));
    RootComponent = ZoneBox;
    ZoneBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    ZoneBox->SetCollisionProfileName(TEXT("Trigger"));
    ZoneBox->SetGenerateOverlapEvents(true);

    ZoneBox->OnComponentEndOverlap.AddDynamic(this, &AReadyZone::OnOverlapEnd);
}

void AReadyZone::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    // 현재 게임모드 가져오기
    if (UWorld* World = GetWorld())
    {
        // AActor* GM = World->GetAuthGameMode(); // AGameModeBase*
        // if (GM && GM->GetClass()->ImplementsInterface(UStageTimerInterface::StaticClass()))
        // {
        //     IStageTimerInterface::Execute_StartStageTimer(GM);
        // }
    }
}
