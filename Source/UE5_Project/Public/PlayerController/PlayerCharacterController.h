#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerCharacterController.generated.h"

class APlayerCharacter;
class UQuickSlot;
class UInputMappingContext;
class UInputAction;

UCLASS()
class UE5_PROJECT_API APlayerCharacterController : public APlayerController
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;

    virtual void SetupInputComponent() override;

private:
    // �⺻ �Է� ���� ���ؽ�Ʈ�� ������ ����
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bind", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputMappingContext> DefaultMappingContext;



    UPROPERTY(EditAnywhere, Category = "UI") // ������ ui�߰�
        TSubclassOf<UQuickSlot> QuickSlotWidgetClass;

    UPROPERTY()
    UQuickSlot* QuickSlotWidget;

    void SelectSlot1();
    void SelectSlot2();
};