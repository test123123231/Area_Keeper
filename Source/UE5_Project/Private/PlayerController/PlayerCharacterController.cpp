#include "PlayerController/PlayerCharacterController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Blueprint/UserWidget.h"
#include "HUD/HUDWidget.h"   
#include "Components/AttributeComponent.h"
#include "GameFramework/Pawn.h"


void APlayerCharacterController::BeginPlay()
{
    Super::BeginPlay();

    // 로컬 플레이어 서브시스템을 가져옴
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        // 기본 매핑 컨텍스트를 추가
        Subsystem->AddMappingContext(DefaultMappingContext, 0);
    }

    // 입력 모드를 게임 전용으로 변경
    FInputModeGameOnly InputModeData;
    SetInputMode(InputModeData);

    bShowMouseCursor = false; // 마우스 커서 숨기기
    
    // HUDRef가 없고 HUDWidgetclass가 있으면 HUDRef 생성
    if (!HUDRef && HUDWidgetClass)
    {
        HUDRef = CreateWidget<UHUDWidget>(this, HUDWidgetClass);
        if (HUDRef)
        {
            //UI를 표시한다
            HUDRef->AddToViewport();
        }
    }

    // 현재 소유한 폰의 attributecomponent의 이벤트에 바인딩
    BindToPawnDelegates(GetPawn());
    // 초기화
    if(HUDRef && BoundAttribute)
    {
        HUDRef -> UpdateHealth(BoundAttribute -> GetHelath());
        HUDRef -> UpdateAmulet(BoundAttribute -> GetAmulet());
    }
}


void APlayerCharacterController::SetupInputComponent()
{
    Super::SetupInputComponent();

    // Enhanced Input Component를 가져와서 액션을 바인딩
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
    {
        // IA_ToggleSettingsMenu 액션이 Triggered될 때, ToggleSettingsMenu 함수를 호출하도록 바인딩
        EnhancedInputComponent->BindAction(IA_ToggleSettingsMenu, ETriggerEvent::Started, this, &APlayerCharacterController::ToggleSettingsMenu);
    }
}


void APlayerCharacterController::ToggleSettingsMenu()
{
    // 설정 메뉴가 이미 화면에 있는지 확인
    if (SettingsMenuInstance && SettingsMenuInstance->IsInViewport())
    {
        CloseSettingMenu();
    }
    else
    {
        // 메뉴 열기
        OpenSettingMenu();
    }
}


void APlayerCharacterController::OpenSettingMenu()
{
    if (SettingsMenuWidgetClass)
    {
        // 위젯 생성
        SettingsMenuInstance = CreateWidget<UUserWidget>(this, SettingsMenuWidgetClass);
        if (SettingsMenuInstance)
        {
            SettingsMenuInstance->AddToViewport(); // 화면에 추가

            // 입력 모드를 게임 및 UI 겸용으로 변경
            FInputModeGameAndUI InputModeData;
            InputModeData.SetWidgetToFocus(SettingsMenuInstance->TakeWidget()); // 포커스를 위젯으로
            InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
            SetInputMode(InputModeData);

            bShowMouseCursor = true; // 마우스 커서 보이기
            SetPause(true); // 게임 일시정지
        }
    }
}


void APlayerCharacterController::CloseSettingMenu()
{
    UE_LOG(LogTemp, Warning, TEXT("CloseSettingMenu called"));
    // 메뉴 닫기
    SettingsMenuInstance->RemoveFromParent();
    SettingsMenuInstance = nullptr; // 포인터 정리

    // 입력 모드를 게임 전용으로 변경
    FInputModeGameOnly InputModeData;
    SetInputMode(InputModeData);

    bShowMouseCursor = false; // 마우스 커서 숨기기
    SetPause(false); // 게임 일시정지 해제
}



// possess
void APlayerCharacterController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    BindToPawnDelegates(InPawn);
}
void APlayerCharacterController::OnUnPossess()
{
    UnbindFromPawnDelegates();
    Super::OnUnPossess();
}

// bind
void APlayerCharacterController::BindToPawnDelegates(APawn* InPawn)
{
    // 중복 방지용 unbind
    UnbindFromPawnDelegates();
    if(!InPawn){ return; }
    // OnHealthChanged/OnAmuletChanged와 HandleHelathChanged/HandleAmuletChanged를 연결
    if(UAttributeComponent* Attr = InPawn -> FindComponentByClass<UAttributeComponent>())
    {
        BoundAttribute = Attr;
        Attr -> OnHealthChanged.AddDynamic(this, &APlayerCharacterController::HandleHealthChanged);
        Attr -> OnAmuletChanged.AddDynamic(this, &APlayerCharacterController::HandleAmuletChanged);
    }
}

void APlayerCharacterController::UnbindFromPawnDelegates()
{
    // OnHealthChanged/OnAmuletChanged와 HandleHelathChanged/HandleAmuletChanged를 연결 해제
	if (BoundAttribute)
	{
		BoundAttribute->OnHealthChanged.RemoveDynamic(this, &APlayerCharacterController::HandleHealthChanged);
        BoundAttribute->OnAmuletChanged.RemoveDynamic(this, &APlayerCharacterController::HandleAmuletChanged);
		BoundAttribute = nullptr;
	}
}


// handle, HUD가 존재할 때만 update 호출
void APlayerCharacterController::HandleHealthChanged(float NewHealth)
{
    if(HUDRef)
    {
        HUDRef->UpdateHealth(NewHealth);
    }
}

void APlayerCharacterController::HandleAmuletChanged(float NewAmulet)
{
    if(HUDRef)
    {
        HUDRef ->UpdateAmulet(NewAmulet);
    }
}

