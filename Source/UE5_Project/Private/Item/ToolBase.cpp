#include "Item/ToolBase.h"
#include "TimerManager.h"
#include "Character/PlayerCharacter.h"
#include "Anomaly/ChasingAnomaly.h"


AToolBase::AToolBase()
{
	TimeToDestroyAfterDrop = 10.0f;
}


void AToolBase::BeginPlay()
{
	Super::BeginPlay();
}


// 아이템을 주웠을 때
void AToolBase::OnPickedUp(USceneComponent* AttachTo, FName SocketName)
{
	// 부모 로직 실행 (물리 끄기)
	Super::OnPickedUp(AttachTo, SocketName);
	// 자동 소멸 타이머 취소
	GetWorld()->GetTimerManager().ClearTimer(DestroyTimerHandle);
}


// 아이템을 떨어뜨렸을 때
void AToolBase::OnDropped()
{
	// 부모 로직 실행 (물리 켜기)
	Super::OnDropped();

	// 자동 소멸 타이머 시작
	GetWorld()->GetTimerManager().SetTimer(
		DestroyTimerHandle,
		this,
		&AToolBase::OnDropTimerExpired,
		TimeToDestroyAfterDrop,
		false
	);
}


// 도구 사용
bool AToolBase::UseTool(AActor* Target)
{
	// 대상이 '쫓아오는 이상현상'인지 확인
	AChasingAnomaly* Anomaly = Cast<AChasingAnomaly>(Target);
	if (Anomaly)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: Used on %s"), *GetName(), *Target->GetName());

		// 이상현상 제거
		Anomaly->Banish(); // AChasingAnomaly에 Banish() 함수가 있다고 가정

		// 도구 소모 (파괴)
		Destroy();
		return true;
	}

	return false;
}


// 타이머 만료 시 자동 파괴
void AToolBase::OnDropTimerExpired()
{
	UE_LOG(LogTemp, Warning, TEXT("%s: Drop timer expired, destroying self."), *GetName());
	Destroy();

	// (준비 구역 리스폰) 로직은
	// 별도의 스폰 매니저가 이 Destroy 이벤트를 감지하여 처리해야함
}


// 플레이어가 상호작용(E)할 때
void AToolBase::Interact_Implementation(APlayerCharacter* Interactor)
{
	if (Interactor)
	{
		// 도구가 바닥에 떨어져 있을 때는 '줍기'만 합니다.
		Interactor->PickupItem(this);
	}
}


FString AToolBase::GetInteractText_Implementation()
{
	return FString(TEXT("줍기"));
}
