#pragma once

#include "CoreMinimal.h"
#include "Item/ItemBase.h"
#include "Anomaly/AnomalyTypes.h"
#include "ToolBase.generated.h"


UCLASS()
class UE5_PROJECT_API AToolBase : public AItemBase
{
	GENERATED_BODY()

public:
	AToolBase();

	// AItemBase의 OnDropped 오버라이드
	virtual void OnDropped() override;

	// AItemBase의 OnPickedUp 오버라이드
	virtual void OnPickedUp(USceneComponent* AttachTo, FName SocketName) override;

	/**
	 * 이 도구를 '사용'
	 * 기본적으로 도구를 소모(파괴)
	 * 사용 대상 (예: 쫓아오는 이상현상)
	 * return 사용 성공 여부
	 */
	virtual bool UseTool(AActor* Target);

	// IInteractableInterface 오버라이드
	// 도구는 '줍기'가 아닌 '사용하기' 상호작용이 될 수 있음
	virtual void Interact_Implementation(APlayerCharacter* Interactor) override;

	virtual FString GetInteractText_Implementation() override;

	/**
	 * BP_Tool 블루프린트의 Class Defaults에서 설정
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tool")
	EToolType ToolType;

protected:
	virtual void BeginPlay() override;

	// 도구가 바닥에 떨어진 후 소멸하기까지의 시간 (초)
	UPROPERTY(EditAnywhere, Category = "Tool")
	float TimeToDestroyAfterDrop = 10.0f;

	// OnDropped 시 시작되는 자동 소멸 타이머
	FTimerHandle DestroyTimerHandle;

	// 타이머 만료 시 액터 파괴
	void OnDropTimerExpired();

};
