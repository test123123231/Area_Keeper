#pragma once
#include "UObject/Interface.h"
#include "StageTimerInterface.generated.h"

UINTERFACE(BlueprintType)
class UStageTimerInterface : public UInterface
{
    GENERATED_BODY()
};

class IStageTimerInterface
{
    GENERATED_BODY()

public:
	// StageTimer 시작
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="StageTimer")
    void StartStageTimer();

	// StageTimer 정지
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="StageTimer")
    void StopStageTimer();

	// 대기 시간이 끝났을 때 호출되는 이벤트 함수
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="StageTimer")
    void TriggerAbnormalEvent();
};