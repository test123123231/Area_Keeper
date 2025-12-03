#include "Item/ItemBase.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Character/PlayerCharacter.h"


// Sets default values
AItemBase::AItemBase()
{
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	RootComponent = ItemMesh;

	// 아이템은 기본적으로 상호작용(Visibility 채널)이 가능해야 함
	ItemMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}


// Called when the game starts or when spawned
void AItemBase::BeginPlay()
{
	Super::BeginPlay();

	if (ItemMesh)
	{
		DynamicMaterial = ItemMesh->CreateAndSetMaterialInstanceDynamic(0);
		if (DynamicMaterial)
		{
			// 블루프린트에서 설정한 BaseColor를 머티리얼에 적용
			DynamicMaterial->SetVectorParameterValue("Color", FLinearColor(0, 0, 0, 1));
		}
		//ItemMesh->SetRenderCustomDepth(true);
		//ItemMesh->SetCustomDepthStencilValue(1);
	}
}


// 플레이어가 바라볼 때
void AItemBase::Highlight_Implementation(bool bIsLooking)
{
	if (DynamicMaterial)
	{
		// 머티리얼에 "Color"라는 파라미터가 있어야 작동함
		//DynamicMaterial->SetVectorParameterValue("Color", bIsLooking ? FLinearColor(3.0f, 0.5f, 0.5f, 1.0f) : FLinearColor(0.f, 0.f, 0.f, 1.f));
		//ItemMesh->SetRenderCustomDepth(bIsLooking);

		if (bIsLooking)
		{
			// 하이라이트: 빨간색
			DynamicMaterial->SetVectorParameterValue("Color", FLinearColor(1.0f, 0.0f, 0.0f, 1.0f));
		}
		else
		{
			// 원래 색상으로 복구
			DynamicMaterial->SetVectorParameterValue("Color", FLinearColor(0, 0, 0, 1));
		}
	}
}

// 플레이어가 상호작용(E)할 때
void AItemBase::Interact_Implementation(APlayerCharacter* Interactor)
{
	if (!Interactor)
	{
		return;
	}
}


FString AItemBase::GetInteractText_Implementation()
{
	return FString(TEXT("상호작용하기"));
}


// 아이템을 주웠을 때
void AItemBase::OnPickedUp(USceneComponent* AttachTo, FName SocketName)
{
	if (!ItemMesh) return;

	// 물리 및 충돌 비활성화
	ItemMesh->SetSimulatePhysics(false);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 부모 컴포넌트에 부착 (스케일은 원본 유지)
	FAttachmentTransformRules Rules(
		EAttachmentRule::SnapToTarget,  // Location: 소켓 위치로
		EAttachmentRule::KeepRelative,  // Rotation
		EAttachmentRule::KeepWorld,     // Scale: 원래 크기 유지
		true
	);
	AttachToComponent(AttachTo, Rules, SocketName);
	/*if (this->GetName().Contains(TEXT("Spray")) || this->GetName().Contains(TEXT("Torch")))
	{
		SetActorRelativeRotation(FRotator(-120.0f, 180.0f, -90.0f));
	}
	else
	{
		SetActorRelativeRotation(FRotator(180.0f, 180.0f, 0.0f));
	}*/
	if (this->GetName().Contains(TEXT("Spray")) || this->GetName().Contains(TEXT("Torch")))
	{
		SetActorRelativeLocation(FVector(50.0f, 5.0f, 0.0f));
		SetActorRelativeRotation(FRotator(60.0f, 125.0f, -90.0f));
	}
	else if (this->GetName().Contains(TEXT("Bat")))
	{
		// 방망이: 손 앞으로 쭉
		SetActorRelativeLocation(FVector(0.0f, 0.0f, 0.0f));  // 앞, 약간 오른쪽
		SetActorRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	}
	else
	{
		// 기본 아이템
		SetActorRelativeLocation(FVector(15.0f, 3.0f, 0.0f));
		SetActorRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	}
	//SetActorRelativeRotation(FRotator::ZeroRotator);
	//AttachToComponent(AttachTo, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), SocketName);
}


// 아이템을 떨어뜨렸을 때
void AItemBase::OnDropped()
{
	if (!ItemMesh) return;

	// 부착 해제
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	// 물리 및 충돌 활성화
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ItemMesh->SetSimulatePhysics(true);
	ItemMesh->SetEnableGravity(true);
}
