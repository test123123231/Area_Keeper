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

    // controller가 소유하는 pawn을 정한다.
    virtual void OnPossess(APawn* InPawn) override;
    // 소유된 pawn을 해제한다
    virtual void OnUnPossess() override;

    // pawn을 소유했을 때 그 폰의 attributecomponent의 이벤트에 바인딩한다.
    void BindToPawnDelegates(APawn* InPawn);
    // 바인딩을 해제한다
    void UnbindFromPawnDelegates();
        

    // 위젯의 HP가 바뀔때 호출 되는 함수
    UFUNCTION()
    void HandleHealthChanged(float NewHealth);
    
    // 위젯의 Amulet이 바뀔때 호출 되는 함수
    UFUNCTION()
    void HandleAmuletChanged(float NewAmulet);

    //HUD로 사용할 위젯 블루프린트
    UPROPERTY(EditDefaultsOnly, Category="UI")
    TSubclassOf<UHUDWidget> HUDWidgetClass;

    //실제로 생성된 HUD 인스턴스
    UPROPERTY() 
    TObjectPtr<UHUDWidget> HUDRef;

    // pawn의 attributecomponent 참조
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
    void TogglePauseMenu();
    void OpenPauseMenu();

    UFUNCTION(BlueprintCallable, Category = "UI")
    void ClosePauseMenu();
};
