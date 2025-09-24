#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerCharacterController.generated.h"



class UInputMappingContext;
class UHUDWidget;
class UAttributeComponent;


UCLASS()
class UE5_PROJECT_API APlayerCharacterController : public APlayerController
{
	GENERATED_BODY()

protected:
    virtual void BeginPlay() override;
    virtual void OnPossess(APawn* InPawn) override;
    virtual void OnUnPossess() override;

    UFUNCTION()
    void HandleHealthChanged(float NewHealth);
    
    UFUNCTION()
    void HandleAmuletChanged(float NewAmulet);

    void BindToPawnDelegates(APawn* InPawn);
    void UnbindFromPawnDelegates();
        
    UPROPERTY(EditDefaultsOnly, Category="UI")
    TSubclassOf<UHUDWidget> HUDWidgetClass;

    UPROPERTY() 
    TObjectPtr<UHUDWidget> HUDRef;

    UPROPERTY()
    TObjectPtr<UAttributeComponent> BoundAttribute = nullptr;



private:
    // �⺻ �Է� ���� ���ؽ�Ʈ�� ������ ����
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bind", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputMappingContext> DefaultMappingContext;
	
    
};
