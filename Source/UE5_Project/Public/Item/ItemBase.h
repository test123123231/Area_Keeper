#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractableInterface.h"
#include "ItemBase.generated.h"

UCLASS()
class UE5_PROJECT_API AItemBase : public AActor, public IInteractableInterface
{
	GENERATED_BODY()

public:
	AItemBase();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	UTexture2D* ItemIcon;


	// 아이템 줍기/내려놓기 로직
	virtual void OnPickedUp(USceneComponent* AttachTo, FName SocketName);
	virtual void OnDropped();

	// IInteractableInterface 구현
	// 플레이어가 바라보면 하이라이트
	virtual void Highlight_Implementation(bool bIsLooking) override;

	// 플레이어가 상호작용(E키)하면 '줍기' 로직 실행
	virtual void Interact_Implementation(APlayerCharacter* Interactor) override;

	virtual FString GetInteractText_Implementation() override;


protected:
	virtual void BeginPlay() override;

	// 하이라이트용 동적 머티리얼
	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterial;
};
