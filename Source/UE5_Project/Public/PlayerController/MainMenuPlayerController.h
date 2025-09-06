#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainMenuPlayerController.generated.h"


class UInputMappingContext;
class UUserWidget;


UCLASS()
class UE5_PROJECT_API AMainMenuPlayerController : public APlayerController
{
	GENERATED_BODY()
protected:
    virtual void BeginPlay() override;

	/** * ������ ���� �޴� ������ �������Ʈ Ŭ����
	 * TSubclassOf�� Ư�� Ŭ����(���⼭�� UUserWidget)�� �ڽ� Ŭ������ �Ҵ��� �� �ֵ��� ����
	 */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> MainMenuWidgetClass;

	// ������ ���� �޴� ������ �ν��Ͻ��� ������ ����
	UPROPERTY(VisibleInstanceOnly, Category = "UI")
	TObjectPtr<UUserWidget> MainMenuWidgetInstance;

private:
    // �⺻ �Է� ���� ���ؽ�Ʈ�� ������ ����
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bind", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputMappingContext> DefaultMappingContext;
};
