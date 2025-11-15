#include "PlayerController/PlayerCharacterController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Blueprint/UserWidget.h"
#include "HUD/HUDWidget.h"   
#include "HUD/TalismanWidget.h"
#include "HUD/PauseMenuWidget.h"
#include "Components/AttributeComponent.h"
#include "GameFramework/Pawn.h"
#include "HUD/QuickSlot.h"
#include "TimerManager.h"
#include "Character/PlayerCharacter.h"
#include "Game/AreaKeeperGameState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"


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
        HUDRef -> UpdateAmulet(BoundAttribute -> GetTalisman());
    }

    if (QuickSlotWidgetClass)
    {
        QuickSlotWidget = CreateWidget<UQuickSlot>(this, QuickSlotWidgetClass);
        if (QuickSlotWidget)
        {
            QuickSlotWidget->AddToViewport(0);

            // PlayerCharacter에 QuickSlot 위젯 참조 설정
            APlayerCharacter* PlayerChar1 = Cast<APlayerCharacter>(GetPawn());
            if (PlayerChar1)
            {
                PlayerChar1->SetQuickSlotRef(QuickSlotWidget);
            }

            UE_LOG(LogTemp, Log, TEXT("QuickSlot UI added to viewport."));
        }
    }

    // GameState 참조를 캐시
    GameStateRef = GetWorld() ? GetWorld()->GetGameState<AAreaKeeperGameState>() : nullptr;

    // GameState가 유효하면, OnPenaltyStackChanged 델리게이트에 우리 함수를 바인딩
    if (GameStateRef.IsValid())
    {
        GameStateRef->OnPenaltyStackChanged.AddDynamic(this, &APlayerCharacterController::OnPenaltyStackUpdated);
    }

    // 플레이어의 '기본' 이동 속도를 가져와서 저장(패널티 해제 시 필요)
    APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(GetPawn());
    if (PlayerChar && PlayerChar->GetCharacterMovement())
    {
        DefaultWalkSpeed = PlayerChar->GetCharacterMovement()->MaxWalkSpeed;
    }
}


void APlayerCharacterController::SetupInputComponent()
{
    Super::SetupInputComponent();

    InputComponent->BindKey(EKeys::One, IE_Pressed, this, &APlayerCharacterController::SelectSlot1);
    InputComponent->BindKey(EKeys::Two, IE_Pressed, this, &APlayerCharacterController::SelectSlot2);

    // Enhanced Input Component를 가져와서 액션을 바인딩
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
    {
        // IA_ToggleSettingsMenu 액션이 Triggered될 때, ToggleSettingsMenu 함수를 호출하도록 바인딩
        EnhancedInputComponent->BindAction(IA_ToggleSettingsMenu, ETriggerEvent::Started, this, &APlayerCharacterController::TogglePauseMenu);
    }
}


void APlayerCharacterController::SelectSlot1()
{
    APlayerCharacter* PlayerChar2 = Cast<APlayerCharacter>(GetPawn());
    if (PlayerChar2)
        PlayerChar2->SelectQuickSlot(0);
}


void APlayerCharacterController::SelectSlot2()
{
    APlayerCharacter* PlayerChar3 = Cast<APlayerCharacter>(GetPawn());
    if (PlayerChar3)
        PlayerChar3->SelectQuickSlot(1);
}


void APlayerCharacterController::TogglePauseMenu()
{
    // 설정 메뉴가 이미 화면에 있는지 확인
    if (PauseMenuInstance && PauseMenuInstance->IsInViewport())
    {
        ClosePauseMenu();
    }
    else
    {
        // 메뉴 열기
        OpenPauseMenu();
    }
}


void APlayerCharacterController::OpenPauseMenu()
{
    if (PauseMenuWidgetClass)
    {
        // 위젯 생성
        PauseMenuInstance = CreateWidget<UPauseMenuWidget>(this, PauseMenuWidgetClass);
        if (PauseMenuInstance)
        {
            PauseMenuInstance->AddToViewport(); // 화면에 추가

            // 입력 모드를 게임 및 UI 겸용으로 변경
            FInputModeGameAndUI InputModeData;
            InputModeData.SetWidgetToFocus(PauseMenuInstance->TakeWidget()); // 포커스를 위젯으로
            InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::LockInFullscreen);
            SetInputMode(InputModeData);

            bShowMouseCursor = true; // 마우스 커서 보이기
            SetPause(true); // 게임 일시정지
        }
    }
}


void APlayerCharacterController::ClosePauseMenu()
{
    UE_LOG(LogTemp, Warning, TEXT("CloseSettingMenu called"));
    // 메뉴 닫기
    PauseMenuInstance->RemoveFromParent();
    PauseMenuInstance = nullptr; // 포인터 정리

    // 입력 모드를 게임 전용으로 변경
    FInputModeGameOnly InputModeData;
    SetInputMode(InputModeData);

    bShowMouseCursor = false; // 마우스 커서 숨기기
    SetPause(false); // 게임 일시정지 해제
}


void APlayerCharacterController::OpenTalismanUI(AStationaryAnomaly* Anomaly)
{
    if (TalismanWidgetClass)
    {
        // 위젯 생성
        TalismanInstance = CreateWidget<UTalismanWidget>(this, TalismanWidgetClass);
        if (TalismanInstance)
        {
            TalismanInstance->AddToViewport(); // 화면에 추가
            // 입력 모드를 게임 및 UI 겸용으로 변경
            FInputModeGameAndUI InputModeData;
            InputModeData.SetWidgetToFocus(TalismanInstance->TakeWidget()); // 포커스를 위젯으로
            InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::LockInFullscreen);
            SetInputMode(InputModeData);
            bShowMouseCursor = true; // 마우스 커서 보이기
            SetPause(true);
        }
    }
}


void APlayerCharacterController::CloseTalismanUI()
{
    if (TalismanInstance)
    {
        // 메뉴 닫기
        TalismanInstance->RemoveFromParent();
        TalismanInstance = nullptr; // 포인터 정리
        // 입력 모드를 게임 전용으로 변경
        FInputModeGameOnly InputModeData;
        SetInputMode(InputModeData);
        bShowMouseCursor = false; // 마우스 커서 숨기기
        SetPause(false);
    }
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
    // OnHealthChanged/OnTalismanChanged와 HandleHelathChanged/HandleTalismanChanged를 연결
    BoundAttribute = InPawn -> FindComponentByClass<UAttributeComponent>();
    if(BoundAttribute)
    {
        BoundAttribute -> OnHealthChanged.AddDynamic(this, &APlayerCharacterController::HandleHealthChanged);
        BoundAttribute -> OnTalismanChanged.AddDynamic(this, &APlayerCharacterController::HandleAmuletChanged);
    }
}

void APlayerCharacterController::UnbindFromPawnDelegates()
{
    // OnHealthChanged/OnTalismanChanged와 HandleHelathChanged/HandleTalismanChanged를 연결 해제
	if (BoundAttribute)
	{
		BoundAttribute->OnHealthChanged.RemoveDynamic(this, &APlayerCharacterController::HandleHealthChanged);
        BoundAttribute->OnTalismanChanged.RemoveDynamic(this, &APlayerCharacterController::HandleAmuletChanged);
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


//텍스트 출력 관련 함수들
FTimerHandle& APlayerCharacterController::GetHideHandle(uint8 TextLocation)
{
    switch(TextLocation)
    {
        case 0:
            return HideCenterTextTimerHandle;
        case 1:
            return HideTimeTextTimerHandle;
        default:
            UE_LOG(LogTemp, Display, TEXT("Invalid TextLocation"));
            return HideCenterTextTimerHandle;
    }
}

void APlayerCharacterController::ShowText(uint8 TextLocation)
{
    if(HUDRef)
    {
        GetWorldTimerManager().ClearTimer(GetHideHandle(TextLocation));
        switch(TextLocation)
        {
            case 0:
                HUDRef -> ShowCenterText();
                break;
            case 1:
                HUDRef -> ShowTimeText();
                break;
            default:
                break;
        }
    }
}


void APlayerCharacterController::ShowAutoText(float Seconds, uint8 TextLocation)
{
    if (HUDRef)
    {
        ShowText(TextLocation);
        FTimerHandle& Handle = GetHideHandle(TextLocation);
        GetWorldTimerManager().ClearTimer(Handle);

        FTimerDelegate Del;
        Del.BindUObject(this, &APlayerCharacterController::HideText, TextLocation);

        GetWorldTimerManager().SetTimer(Handle, Del, Seconds, false);
    }
}

void APlayerCharacterController::HideText(uint8 TextLocation)
{
    if(HUDRef)
    {
        switch(TextLocation)
        {
            case 0:
                HUDRef -> HideCenterText();
                break;
            case 1:
                HUDRef -> HideTimeText();
                break;
            default:
                break;
        }
        GetWorldTimerManager().ClearTimer(GetHideHandle(TextLocation));
    }
}

void APlayerCharacterController::UpdateText(const FString& Text, uint8 TextLocation)
{
    if(HUDRef)
    {
        switch(TextLocation)
        {
            case 0:
                HUDRef -> UpdateCenterText(Text);
                break;
            case 1:
                HUDRef -> UpdateTimeText(Text);
                break;
            default:
                break;
        }
    }
}


/**
 * GameState에서 패널티 스택이 변경될 때마다 콜백 함수
 */
void APlayerCharacterController::OnPenaltyStackUpdated(int32 NewStackCount)
{
    ApplyPenaltyEffects(NewStackCount);
}


void APlayerCharacterController::ApplyPenaltyEffects(int32 NewStackCount)
{
    APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(GetPawn());
    if (!PlayerChar) return;
    ApplyMovementPenalty(PlayerChar, NewStackCount);
    ApplyVignettePenalty(PlayerChar, NewStackCount);
    ApplySoundPenalty(NewStackCount);
}


void APlayerCharacterController::ApplySoundPenalty(int32 NewStackCount)
{
    // --- 2스택: 청각 패널티 (주기적인 사운드 재생) ---
    if (NewStackCount >= 2)
    {
        // 타이머가 이미 실행 중인지 확인 (중복 실행 방지)
        if (!GetWorld()->GetTimerManager().IsTimerActive(PenaltySoundTimerHandle))
        {
            // 타이머 시작: WhisperInterval마다 PlayWhisperSound 함수를, 반복(true) 실행
            GetWorld()->GetTimerManager().SetTimer(
                PenaltySoundTimerHandle,
                this,
                &APlayerCharacterController::PlayWhisperSound,
                WhisperInterval,
                true);

            // 즉시 1회 재생
            PlayWhisperSound();
        }
    }
    else
    {
        // 2스택 미만이면 반복 타이머를 즉시 정지
        GetWorld()->GetTimerManager().ClearTimer(PenaltySoundTimerHandle);
    }
}


void APlayerCharacterController::ApplyVignettePenalty(APlayerCharacter* PlayerChar, int32 NewStackCount)
{
    // --- 3스택: 시각 패널티 (비네트 효과) ---
    UCameraComponent* Camera = PlayerChar->GetViewCamera(); // 추가한 Getter로 카메라 가져오기
    if (Camera)
    {
        // PostProcessSettings 값을 덮어씀
        Camera->PostProcessSettings.bOverride_VignetteIntensity = true;

        if (NewStackCount >= 3)
        {
            Camera->PostProcessSettings.VignetteIntensity = 1.0f; // 비네트 최대
        }
        else
        {
            Camera->PostProcessSettings.VignetteIntensity = 0.0f; // 비네트 제거
        }
    }
}


void APlayerCharacterController::ApplyMovementPenalty(APlayerCharacter* PlayerChar, int32 NewStackCount)
{
    // --- 4스택: 이동 속도 패널티 (10~20% 감소) ---
    UCharacterMovementComponent* Movement = PlayerChar->GetCharacterMovement();
    if (Movement)
    {
        if (NewStackCount >= 4)
        {
            Movement->MaxWalkSpeed = DefaultWalkSpeed * 0.8f; // 20% 감소
        }
        else
        {
            Movement->MaxWalkSpeed = DefaultWalkSpeed; // 기본 속도로 복원
        }
    }
}


/**
 * 2스택 패널티 사운드 재생 (타이머가 호출)
 * [수정] 배열에서 랜덤하게 사운드를 선택하여 재생
 */
void APlayerCharacterController::PlayWhisperSound()
{
    // 1. BP에서 사운드 배열이 채워졌는지, 비어있지 않은지 확인
    if (WhisperSounds.Num() > 0)
    {
        // 2. 0부터 (배열 크기 - 1) 사이의 랜덤 인덱스를 구합니다.
        int32 RandomIndex = FMath::RandRange(0, WhisperSounds.Num() - 1);

        // 3. 해당 인덱스의 사운드 에셋을 가져옵니다.
        //    (IsValidIndex 체크는 RandRange가 범위를 보장하므로 생략 가능)
        USoundBase* SoundToPlay = WhisperSounds[RandomIndex];

        // 4. 해당 사운드 에셋이 유효한지(nullptr이 아닌지) 확인하고 재생
        if (SoundToPlay) {
            UGameplayStatics::PlaySound2D(GetWorld(), SoundToPlay);
        }
    }
}