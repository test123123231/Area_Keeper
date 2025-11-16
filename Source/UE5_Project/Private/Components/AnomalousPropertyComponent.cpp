#include "Components/AnomalousPropertyComponent.h"
#include "GameFramework/Actor.h"
#include "Item/ItemBase.h"
#include "Materials/MaterialInstanceDynamic.h" // 동적 머티리얼 제어용
#include "Components/PrimitiveComponent.h" // 루트 컴포넌트의 물리를 끄기 위함

UAnomalousPropertyComponent::UAnomalousPropertyComponent()
{
	// 이 컴포넌트는 틱이 필요 없습니다.
	PrimaryComponentTick.bCanEverTick = false;

	// 기본값 초기화
	bIsAnomalous = false;
	OriginalScale = FVector(1.0f, 1.0f, 1.0f);
	OriginalLocation = FVector(0.0f, 0.0f, 0.0f);
}


void UAnomalousPropertyComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if (!Owner) return;

	// [중요] 액터의 '현재 상태'를 저장합니다.
	// 스케일은 항상 루트 컴포넌트의 스케일을 사용합니다.
	OriginalScale = Owner->GetActorScale3D();

	// 위치는 루트 컴포넌트의 상대 위치를 저장합니다 (만약 부모에 붙어있을 경우 대비)
	if (Owner->GetRootComponent())
	{
		OriginalLocation = Owner->GetRootComponent()->GetRelativeLocation();
	}

	// '특이사항'을 대비해 원본 머티리얼을 캐시합니다.
	CacheOriginalMaterials();
}


/**
 * 나중에 ResetToNormal()을 호출할 때를 대비해
 * 이 액터의 현재 머티리얼을 저장합니다.
 */
void UAnomalousPropertyComponent::CacheOriginalMaterials()
{
	AActor* Owner = GetOwner();
	if (!Owner) return;

	// 이 액터의 모든 StaticMeshComponent를 찾습니다.
	TArray<UStaticMeshComponent*> MeshComponents;
	Owner->GetComponents<UStaticMeshComponent>(MeshComponents);

	for (UStaticMeshComponent* MeshComp : MeshComponents)
	{
		if (MeshComp)
		{
			// 각 메쉬의 머티리얼 슬롯 수만큼 반복
			for (int32 i = 0; i < MeshComp->GetNumMaterials(); ++i)
			{
				// 현재 머티리얼을 OriginalMaterials 맵에 저장
				OriginalMaterials.Add(FMaterialCacheKey(MeshComp, i), MeshComp->GetMaterial(i));
			}
		}
	}
}


/**
 * (AnomalyManager가 호출) 액터를 공중에 띄웁니다.
 */
void UAnomalousPropertyComponent::StartLevitation(float FloatHeight)
{
	if (bIsAnomalous) return; // 이미 다른 이상현상이 적용 중이면 무시

	AActor* Owner = GetOwner();
	if (!Owner) return;

	// 강제로 물리를 끄고, 모빌리티가 'Static'이면 'Movable'로 강제 변경
	UPrimitiveComponent* RootComp = Cast<UPrimitiveComponent>(Owner->GetRootComponent());
	// 
	if (RootComp->GetMobility() == EComponentMobility::Static)
	{
		RootComp->SetMobility(EComponentMobility::Movable);
	}

	if (RootComp && RootComp->IsSimulatingPhysics())
	{
		RootComp->SetSimulatePhysics(false);
	}

	// 원래 위치를 기준으로 새 위치 계산
	FVector NewLocation = OriginalLocation + FVector(0.0f, 0.0f, FloatHeight);

	// 루트 컴포넌트의 상대 위치를 변경
	if (Owner->GetRootComponent())
	{
		Owner->GetRootComponent()->SetRelativeLocation(NewLocation);
	}

	bIsAnomalous = true;
	CurrentAnomalyType = EAnomalyCategory::EAC_Levitation;
}


/**
 * (AnomalyManager가 호출) 액터의 크기를 키웁니다.
 */
void UAnomalousPropertyComponent::StartGigantism(float ScaleMultiplier)
{
	if (bIsAnomalous) return;

	AActor* Owner = GetOwner();
	if (!Owner) return;

	// 원래 크기를 기준으로 새 크기 계산
	FVector NewScale = OriginalScale * ScaleMultiplier;
	Owner->SetActorScale3D(NewScale);

	bIsAnomalous = true;
	CurrentAnomalyType = EAnomalyCategory::EAC_Gigantism;
}


/**
 * (AnomalyManager가 호출) AItemBase 등 상호작용 가능한 물체의 특이사항을 발동
 */
void UAnomalousPropertyComponent::StartPeculiarity()
{
	if (bIsAnomalous) return;

	AActor* Owner = GetOwner();
	if (!Owner) return;

	// 예시: 모든 메쉬의 머티리얼을 '이상하게' 변경 (예: 빨간색)
	TArray<UStaticMeshComponent*> MeshComponents;
	Owner->GetComponents<UStaticMeshComponent>(MeshComponents);

	for (UStaticMeshComponent* MeshComp : MeshComponents)
	{
		if (MeshComp)
		{
			for (int32 i = 0; i < MeshComp->GetNumMaterials(); ++i)
			{
				// 동적 머티리얼 인스턴스를 생성
				UMaterialInstanceDynamic* DynamicMat = MeshComp->CreateAndSetMaterialInstanceDynamic(i);
				if (DynamicMat)
				{
					// 색상 파라미터(Color)가 있다고 가정하고 빨간색으로 변경
					// (AItemBase의 Highlight 로직과 유사)
					DynamicMat->SetVectorParameterValue("Color", FLinearColor::Red);
				}
			}
		}
	}

	bIsAnomalous = true;
	CurrentAnomalyType = EAnomalyCategory::EAC_Peculiarity;
}


/**
 * (AnomalyManager가 호출) 이상현상 해결 시 모든 변조를 원상복구
 */
void UAnomalousPropertyComponent::ResetToNormal()
{
	if (!bIsAnomalous) return; // 변조된 적이 없으면 무시

	AActor* Owner = GetOwner();
	if (!Owner) return;

	// 강제로 물리를 끄고, 모빌리티가 'Static'이면 'Movable'로 강제 변경
	UPrimitiveComponent* RootComp = Cast<UPrimitiveComponent>(Owner->GetRootComponent());
	// 
	if (RootComp->GetMobility() == EComponentMobility::Static)
	{
		RootComp->SetMobility(EComponentMobility::Movable);
	}

	if (RootComp && RootComp->IsSimulatingPhysics())
	{
		RootComp->SetSimulatePhysics(false);
	}

	// 1. 크기 및 위치 복원
	Owner->SetActorScale3D(OriginalScale);
	if (Owner->GetRootComponent())
	{
		Owner->GetRootComponent()->SetRelativeLocation(OriginalLocation);
	}

	// 2. 만약 '특이사항'이었다면 머티리얼도 복원
	if (CurrentAnomalyType == EAnomalyCategory::EAC_Peculiarity)
	{
		for (auto const& [Key, Material] : OriginalMaterials)
		{
			if (Key.MeshComponent && Material)
			{
				// 캐시해 둔 OriginalMaterial로 되돌림
				Key.MeshComponent->SetMaterial(Key.MaterialIndex, Material.Get());
			}
		}
	}

	bIsAnomalous = false;
	CurrentAnomalyType = EAnomalyCategory::EAC_None;
}