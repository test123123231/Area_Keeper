// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ItemBase.h"

#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

// Sets default values
AItemBase::AItemBase()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    // 메시 생성 및 루트 컴포넌트로 설정(추가)
    ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
    RootComponent = ItemMesh;
}

// Called when the game starts or when spawned
void AItemBase::BeginPlay()
{
    Super::BeginPlay();

    if (ItemMesh)
    {
        // 머티리얼을 동적으로 제어할 수 있도록 인스턴스 생성
        DynamicMaterial = ItemMesh->CreateAndSetMaterialInstanceDynamic(0);
    }

}

// Called every frame
void AItemBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

// 하이라이트 함수
void AItemBase::HighlightItem(bool bOn)
{
    if (DynamicMaterial)
    {
        // 머티리얼에 "Color"라는 파라미터가 있어야 작동함
        if (bOn)
        {
            DynamicMaterial->SetVectorParameterValue("Color", FLinearColor::Red);
        }
        else
        {
            DynamicMaterial->SetVectorParameterValue("Color", FLinearColor::White);
        }
    }
}


void AItemBase::OnPickedUp()
{
    if (ItemMesh)
    {
        ItemMesh->SetSimulatePhysics(false);
        ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
}

void AItemBase::OnDropped()
{
    if (ItemMesh)
    {
        ItemMesh->SetSimulatePhysics(true);
        ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

        UE_LOG(LogTemp, Warning, TEXT("OnDropped: %s -> Collision+Physics 즉시 켜짐"), *GetName());
    }
}



