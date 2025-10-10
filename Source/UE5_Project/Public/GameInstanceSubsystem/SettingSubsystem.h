#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SettingSubsystem.generated.h"


class USettingSaveGame;


UCLASS()
class UE5_PROJECT_API USettingSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
    // 서브시스템 초기화 시 호출
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    // --- 블루프린트에서 사용할 함수들 ---

    UFUNCTION(BlueprintCallable, Category = "Settings")
    void SaveSettings();

    UFUNCTION(BlueprintCallable, Category = "Settings")
    void ApplySettings();

    // 블루프린트에서 값을 가져오고 임시로 변경하기 위한 Getter/Setter
    UFUNCTION(BlueprintPure, Category = "Settings")
    float GetMasterVolume() const;

    UFUNCTION(BlueprintPure, Category = "Settings")
    float GetDefaultMasterVolume() const;

    UFUNCTION(BlueprintCallable, Category = "Settings")
    void SetMasterVolume(float NewValue);

    UFUNCTION(BlueprintPure, Category = "Settings")
    float GetMouseSensitivity() const;

    UFUNCTION(BlueprintPure, Category = "Settings")
    float GetDefaultMouseSensitivity() const;

    UFUNCTION(BlueprintCallable, Category = "Settings")
    void SetMouseSensitivity(float NewValue);

    // 기본값으로 되돌리기
    UFUNCTION(BlueprintCallable, Category = "Settings")
    void ResetAllSettingsToDefaults();


private:
    void LoadSettings();

    // 현재 게임에 적용된 설정값을 담고 있는 인스턴스
    UPROPERTY()
    TObjectPtr<USettingSaveGame> CurrentSettings;
};
