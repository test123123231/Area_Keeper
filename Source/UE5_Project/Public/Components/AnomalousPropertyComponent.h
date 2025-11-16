#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Anomaly/AnomalyTypes.h"
#include "AnomalousPropertyComponent.generated.h"


/**
 * '특이사항' 발생 시 원본 머티리얼로 복구하기 위해
 * 메쉬 컴포넌트와 머티리얼 인덱스를 키로 사용하는 헬퍼 구조체
 */
USTRUCT()
struct FMaterialCacheKey
{
    GENERATED_BODY()

    // TMap의 키로 사용하기 위해 TWeakObjectPtr 대신 UPROPERTY를 사용
    UPROPERTY()
    TObjectPtr<UStaticMeshComponent> MeshComponent;

    UPROPERTY()
    int32 MaterialIndex;

    // TMap 키 비교를 위한 연산자
    bool operator==(const FMaterialCacheKey& Other) const
    {
        return MeshComponent == Other.MeshComponent && MaterialIndex == Other.MaterialIndex;
    }

    // TMap 키 해싱을 위한 함수
    friend FORCEINLINE uint32 GetTypeHash(const FMaterialCacheKey& Key)
    {
        return HashCombine(GetTypeHash(Key.MeshComponent), GetTypeHash(Key.MaterialIndex));
    }
};


/**
 * 맵에 배치된 액터가 '환경 이상현상'의 대상이 될 수 있도록
 * 원본 상태를 저장하고, 변조/복원 기능을 제공하는 컴포넌트
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UE5_PROJECT_API UAnomalousPropertyComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAnomalousPropertyComponent();

protected:
    virtual void BeginPlay() override;

private:
    // --- 원본 상태 저장 변수 ---

    // 이 액터의 원본 상대 위치
    UPROPERTY()
    FVector OriginalLocation;

    // 이 액터의 원본 스케일
    UPROPERTY()
    FVector OriginalScale;

    /**
     * '특이사항' 발생 시 머티리얼을 복원하기 위해,
     * BeginPlay 시점에 원본 머티리얼을 캐시합니다.
     */
    UPROPERTY()
    TMap<FMaterialCacheKey, TObjectPtr<UMaterialInterface>> OriginalMaterials;

    // 원본 머티리얼을 캐시하는 헬퍼 함수
    void CacheOriginalMaterials();

    // --- 상태 변수 ---

    // 현재 이상현상에 의해 변조되었는지 여부
    bool bIsAnomalous;

    // 현재 적용된 이상현상의 유형 (EAnomalyCategory)
    EAnomalyCategory CurrentAnomalyType;

public:
    // --- AnomalyManager가 호출할 함수들 ---

    /**
     * (AnomalyManager가 호출) 액터를 공중에 띄웁니다. (AC_Levitation)
     * @param FloatHeight 띄울 높이 (cm)
     */
    UFUNCTION(BlueprintCallable, Category = "Anomaly")
    void StartLevitation(float FloatHeight);

    /**
     * (AnomalyManager가 호출) 액터의 크기를 키웁니다. (AC_Gigantism)
     * @param ScaleMultiplier 배율 (예: 2.5)
     */
    UFUNCTION(BlueprintCallable, Category = "Anomaly")
    void StartGigantism(float ScaleMultiplier);

    /**
     * (AnomalyManager가 호출) 액터의 머티리얼을 변경하는 등 특이사항을 발동합니다. (AC_Peculiarity)
     */
    UFUNCTION(BlueprintCallable, Category = "Anomaly")
    void StartPeculiarity();

    /**
     * (AnomalyManager가 호출) 적용된 모든 이상현상을
     * BeginPlay 시점의 원본 상태로 되돌립니다.
     */
    UFUNCTION(BlueprintCallable, Category = "Anomaly")
    void ResetToNormal();

    /**
     * (AnomalyManager가 호출) 이 컴포넌트가 현재 변조된 상태인지 반환합니다.
     */
    UFUNCTION(BlueprintPure, Category = "Anomaly")
    bool IsAnomalous() const { return bIsAnomalous; }
};