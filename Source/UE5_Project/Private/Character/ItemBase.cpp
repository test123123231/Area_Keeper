// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ItemBase.h"

#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

// Sets default values
AItemBase::AItemBase()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    // �޽� ���� �� ��Ʈ ������Ʈ�� ����(�߰�)
    ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
    RootComponent = ItemMesh;
}

// Called when the game starts or when spawned
void AItemBase::BeginPlay()
{
    Super::BeginPlay();

    if (ItemMesh)
    {
        // ��Ƽ������ �������� ������ �� �ֵ��� �ν��Ͻ� ����
        DynamicMaterial = ItemMesh->CreateAndSetMaterialInstanceDynamic(0);
    }

}

// Called every frame
void AItemBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

// ���̶���Ʈ �Լ�
void AItemBase::HighlightItem(bool bOn)
{
    if (DynamicMaterial)
    {
        // ��Ƽ���� "Color"��� �Ķ���Ͱ� �־�� �۵���
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

        UE_LOG(LogTemp, Warning, TEXT("OnDropped: %s -> Collision+Physics ��� ����"), *GetName());
    }
}

/*void AItemBase::OnPickedUp()
{
    ItemMesh->SetSimulatePhysics(false);
    ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    UE_LOG(LogTemp, Warning, TEXT("OnPickedUp: %s (Physics=0, Collision=NoCollision)"),
        *GetName());
}

void AItemBase::OnDropped()
{
    // �켱 �浹�� �Ѱ�
    ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

    UE_LOG(LogTemp, Warning, TEXT("OnDropped called for %s -> Enable Collision, delay Physics"),
        *GetName());

    // Physics�� ��¦ �������Ѽ� Attach�� �浹���� �ʰ� ��
    FTimerHandle Timer;
    GetWorldTimerManager().SetTimer(Timer, [this]()
        {
            if (IsValid(this) && ItemMesh)
            {
                ItemMesh->SetSimulatePhysics(true);
                UE_LOG(LogTemp, Warning, TEXT("OnDropped Physics Enabled for %s"), *GetName());
            }
        }, 0.25f, false);
}*/


