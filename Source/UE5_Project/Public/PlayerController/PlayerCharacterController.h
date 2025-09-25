#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerCharacterController.generated.h"


class UInputMappingContext;
class UHUDWidget;
class UAttributeComponent;
class UInputAction;


UCLASS()
class UE5_PROJECT_API APlayerCharacterController : public APlayerController
{
	GENERATED_BODY()

protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

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

    // 기본 입력 매핑 컨텍스트를 저장할 변수
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bind", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputMappingContext> DefaultMappingContext;

    /** Settings Menu Input Action */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* IA_ToggleSettingsMenu;

    // 생성할 위젯 블루프린트 클래스. TSubclassOf는 클래스 자체를 저장합니다.
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UUserWidget> SettingsMenuWidgetClass;

    // 실제로 생성된 위젯 인스턴스를 저장할 포인터
    UUserWidget* SettingsMenuInstance;

private:

public:
    void ToggleSettingsMenu();
	void OpenSettingMenu();

    UFUNCTION(BlueprintCallable, Category = "UI")
	void CloseSettingMenu();
};
