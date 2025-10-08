#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerCharacterController.generated.h"

class UQuickSlot;
class UInputMappingContext;


UCLASS()
class UE5_PROJECT_API APlayerCharacterController : public APlayerController
{
	GENERATED_BODY()

protected:
    virtual void BeginPlay() override;

private:
    // �⺻ �Է� ���� ���ؽ�Ʈ�� ������ ����
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bind", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputMappingContext> DefaultMappingContext;
	
    UPROPERTY(EditAnywhere, Category = "UI") // ������ ui�߰�
    TSubclassOf<UQuickSlot> QuickSlotWidgetClass;

    UPROPERTY()
    UQuickSlot* QuickSlotWidget;
};
