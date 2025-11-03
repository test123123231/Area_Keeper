#include "Game/AreaKeeperGameMode.h"
#include "Game/AreaKeeperGameState.h"
#include "HUD/GameOverWidget.h"
#include "HUD/GameClearWidget.h"
#include "HUD/HUDWidget.h"
#include "HUD/QuickSlot.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerController/PlayerCharacterController.h"


AAreaKeeperGameMode::AAreaKeeperGameMode()
{
}


void AAreaKeeperGameMode::BeginPlay()
{
	Super::BeginPlay();

	// AAreaKeeperGameState로 캐스팅하고 변수에 저장합니다.
	GameStateRef = GetWorld() ? Cast<AAreaKeeperGameState>(GetWorld()->GetGameState()) : nullptr;

	if (GameStateRef.IsValid())
	{
		//  "게임 오버 시" 바인딩 로직
		// GameState의 OnGameOver 델리게이트에 HandleGameOver 연결
		GameStateRef->OnGameOver.AddDynamic(this, &AAreaKeeperGameMode::HandleGameOver);

		// "게임 클리어 시" 바인딩 로직
		// GameState의 OnGameClear 델리게이트에 HandleGameClear 연결
		GameStateRef->OnGameClear.AddDynamic(this, &AAreaKeeperGameMode::HandleGameClear);

		UE_LOG(LogTemp, Log, TEXT("AreaKeeperGameMode: GameState 델리게이트 바인딩 성공."));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AreaKeeperGameMode: GameState가 AAreaKeeperGameState가 아닙니다! 캐스팅 실패."));
	}
}


void AAreaKeeperGameMode::HandleGameOver(bool bPlayerDied)
{
	UE_LOG(LogTemp, Log, TEXT("GAME OVER. Player Died: %s"), bPlayerDied ? TEXT("True") : TEXT("False"));

	// 게임오버 UI 생성 로직
	if (GameOverWidgetClass)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
		if (APlayerCharacterController* MyPC = Cast<APlayerCharacterController>(PC))
		{
			if (MyPC->GetHUDWidget())
			{
				MyPC->GetHUDWidget()->RemoveFromParent(); // HUD 제거
			}

			if (MyPC->GetQuickSlotWidget())
			{
				MyPC->GetQuickSlotWidget()->RemoveFromParent(); // 퀵슬롯 제거
			}

			// 이전에 떠 있던 위젯이 있다면 제거 (옵션)
			if (CurrentScreenWidget)
			{
				CurrentScreenWidget->RemoveFromParent();
			}

			// GameOverWidgetClass (WBP_GameOver)로 위젯을 생성
			UGameOverWidget* GameOverWidget = CreateWidget<UGameOverWidget>(PC, GameOverWidgetClass);
			if (GameOverWidget)
			{
				// C++ 위젯의 초기화 함수를 호출하여 "사망 원인"을 전달
				GameOverWidget->InitializeWidget(bPlayerDied);

				// 뷰포트에 추가
				GameOverWidget->AddToViewport();
				CurrentScreenWidget = GameOverWidget; // 변수에 저장

				// 플레이어 입력 모드를 UI 전용으로 변경하고 마우스 커서를 표시
				SetPlayerInputModeToUIOnly();
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AreaKeeperGameMode: GameOverWidgetClass가 설정되지 않았습니다. (BP에서 설정 필요)"));
	}
}


void AAreaKeeperGameMode::HandleGameClear()
{
	UE_LOG(LogTemp, Log, TEXT("GAME CLEAR!"));

	// 게임 클리어 UI 생성 로직
	if (GameClearWidgetClass)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
		if (APlayerCharacterController* MyPC = Cast<APlayerCharacterController>(PC))
		{
			if (MyPC->GetHUDWidget())
			{
				MyPC->GetHUDWidget()->RemoveFromParent(); // HUD 제거
			}

			if (MyPC->GetQuickSlotWidget())
			{
				MyPC->GetQuickSlotWidget()->RemoveFromParent(); // 퀵슬롯 제거
			}

			if (CurrentScreenWidget)
			{
				CurrentScreenWidget->RemoveFromParent();
			}

			// GameClearWidgetClass (WBP_GameClear)로 위젯을 생성합니다.
			UGameClearWidget* GameClearWidget = CreateWidget<UGameClearWidget>(PC, GameClearWidgetClass);
			if (GameClearWidget)
			{
				// GameState에서 클리어 시간, 패널티 스택 등의 정보를 가져와 전달
				GameClearWidget->InitializeWidget(GameStateRef->GetPenaltyStack(), GameStateRef->GetAnomaliesSolved(), GameStateRef->GetChasingHits());

				GameClearWidget->AddToViewport();
				CurrentScreenWidget = GameClearWidget;

				SetPlayerInputModeToUIOnly();
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AreaKeeperGameMode: GameClearWidgetClass가 설정되지 않았습니다. (BP에서 설정 필요)"));
	}
}


void AAreaKeeperGameMode::SetPlayerInputModeToUIOnly()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		FInputModeUIOnly InputModeData;
		//InputModeData.SetWidgetToFocus(CurrentScreenWidget->TakeWidget()); // 생성된 위젯에 포커스
		InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

		PC->SetInputMode(InputModeData);
		PC->bShowMouseCursor = true;
	}
}
