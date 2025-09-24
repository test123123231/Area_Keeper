#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerCharacterController.generated.h"



class UInputMappingContext;

class UHUDWidget;


UCLASS()
class UE5_PROJECT_API APlayerCharacterController : public APlayerController
{
	GENERATED_BODY()

protected:
    virtual void BeginPlay() override;
        
    UPROPERTY(EditDefaultsOnly, Category="UI")
    TSubclassOf<UHUDWidget> HUDWidgetClass;

    UPROPERTY() 
    TObjectPtr<UHUDWidget> HUDRef;



private:
    // �⺻ �Է� ���� ���ؽ�Ʈ�� ������ ����
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bind", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputMappingContext> DefaultMappingContext;
	
    
};
