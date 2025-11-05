#pragma once


/**
 * 게임의 현재 상태 (타이머, 페이즈)
 */
UENUM(BlueprintType)
enum class EAreaKeeperPlayState : uint8
{
	// 플레이 준비 구역에 있으며, 60초 준비 타이머 대기 중
	EPS_WaitingToStart UMETA(DisplayName = "Waiting to Start"),

	// 메인 게임 플레이 중 (이상현상 스폰 활성화)
	EPS_InProgress UMETA(DisplayName = "In Progress"),

	// 게임 클리어 또는 게임 오버
	EPS_GameFinished UMETA(DisplayName = "Game Finished")
};