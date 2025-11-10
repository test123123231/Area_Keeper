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
		// 초기화: 검은색으로 설정
		if (DynamicMaterial)
		{
			DynamicMaterial->SetVectorParameterValue("Color", FLinearColor(0.f, 0.f, 0.f, 1.f));
		}
	}
}


// 플레이어가 바라볼 때
void AItemBase::Highlight_Implementation(bool bIsLooking)
{
	if (DynamicMaterial)
	{
		// 머티리얼에 "Color"라는 파라미터가 있어야 작동함
		//DynamicMaterial->SetVectorParameterValue("Color", bIsLooking ? FLinearColor::Red : FLinearColor::White);
		DynamicMaterial->SetVectorParameterValue("Color", bIsLooking ? FLinearColor::Red : FLinearColor(0.f, 0.f, 0.f, 1.f));
	}
}

// 플레이어가 상호작용(E)할 때
void AItemBase::Interact_Implementation(APlayerCharacter* Interactor)
{
	if (Interactor)
	{
		// 플레이어의 '아이템 줍기' 로직을 호출
		Interactor->PickupItem(this);
	}
}


FString AItemBase::GetInteractText_Implementation()
{
	return FString(TEXT("줍기"));
}


// 아이템을 주웠을 때
void AItemBase::OnPickedUp(USceneComponent* AttachTo, FName SocketName)
{
	if (!ItemMesh) return;

	// 물리 및 충돌 비활성화
	ItemMesh->SetSimulatePhysics(false);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 부모 컴포넌트에 부착
	AttachToComponent(AttachTo, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), SocketName);
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
