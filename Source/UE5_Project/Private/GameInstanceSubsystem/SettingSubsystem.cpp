#include "GameInstanceSubsystem/SettingSubsystem.h"
#include "SaveGame/SettingSaveGame.h"
#include "Kismet/GameplayStatics.h"


void USettingSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    LoadSettings();
    ApplySettings();
}


void USettingSubsystem::LoadSettings()
{
    // 저장된 파일이 있는지 확인하고 불러오기
    if (UGameplayStatics::DoesSaveGameExist(TEXT("GameSettings"), 0))
    {
        CurrentSettings = Cast<USettingSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("GameSettings"), 0));
    }
    else
    {
        // 없다면 기본값으로 새로 생성
        CurrentSettings = Cast<USettingSaveGame>(UGameplayStatics::CreateSaveGameObject(USettingSaveGame::StaticClass()));
    }
}


void USettingSubsystem::SaveSettings()
{
    if (CurrentSettings)
    {
        UGameplayStatics::SaveGameToSlot(CurrentSettings, CurrentSettings->SaveSlotName, CurrentSettings->UserIndex);
    }
}


void USettingSubsystem::ApplySettings()
{
    if (!CurrentSettings) return;

    // TODO: 실제 게임에 설정을 적용하는 로직 구현
    // 예: 사운드 설정 적용 (Sound Mix 필요)
    // UGameplayStatics::SetSoundMixClassOverride(...)

    // 예: 마우스 감도 적용
    // GetWorld()->GetFirstPlayerController()->...

    UE_LOG(LogTemp, Warning, TEXT("Settings Applied: MasterVol=%f, MouseSens=%f"), CurrentSettings->MasterVolume, CurrentSettings->MouseSensitivity);
}

// --- 블루프린트용 Getter/Setter 구현 ---

float USettingSubsystem::GetMasterVolume() const
{
    return CurrentSettings ? CurrentSettings->MasterVolume : 1.0f;
}


float USettingSubsystem::GetDefaultMasterVolume() const
{
    return GetDefault<USettingSaveGame>()->MasterVolume;
}



void USettingSubsystem::SetMasterVolume(float NewValue)
{
    if (CurrentSettings) CurrentSettings->MasterVolume = NewValue;
}


float USettingSubsystem::GetMouseSensitivity() const
{
    return CurrentSettings ? CurrentSettings->MouseSensitivity : 1.0f;
}


float USettingSubsystem::GetDefaultMouseSensitivity() const
{
    return GetDefault<USettingSaveGame>()->MouseSensitivity;
}


void USettingSubsystem::SetMouseSensitivity(float NewValue)
{
    if (CurrentSettings) CurrentSettings->MouseSensitivity = NewValue;
}


void USettingSubsystem::ResetAllSettingsToDefaults()
{
    // 기본값을 가진 새 인스턴스를 만들어 현재 설정에 덮어쓰기
    CurrentSettings = Cast<USettingSaveGame>(UGameplayStatics::CreateSaveGameObject(USettingSaveGame::StaticClass()));
    ApplySettings();
    SaveSettings();
}