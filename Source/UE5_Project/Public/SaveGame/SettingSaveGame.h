#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SettingSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class UE5_PROJECT_API USettingSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
    // 기본값을 설정하는 생성자
    USettingSaveGame();

    // -- 저장할 설정 값들 --

    UPROPERTY(VisibleAnywhere, Category = "Settings|Sound")
    float MasterVolume;

    UPROPERTY(VisibleAnywhere, Category = "Settings|Sound")
    float MusicVolume;

    UPROPERTY(VisibleAnywhere, Category = "Settings|Mouse")
    float MouseSensitivity;

    // -- 저장 시스템을 위한 정보 --

    UPROPERTY()
    FString SaveSlotName;

    UPROPERTY()
    uint32 UserIndex;
	
};
