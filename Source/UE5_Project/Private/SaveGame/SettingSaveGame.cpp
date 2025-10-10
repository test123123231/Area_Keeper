#include "SaveGame/SettingSaveGame.h"


USettingSaveGame::USettingSaveGame()
{
    // 게임의 기본 설정값 (초기화 시 돌아갈 값)
    MasterVolume = 0.5f;
    MusicVolume = 0.5f;
    MouseSensitivity = 0.5f;

    // 저장 파일 이름과 인덱스
    SaveSlotName = TEXT("GameSettings");
    UserIndex = 0;
}