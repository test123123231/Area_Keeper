#pragma once

// 이상현상의 2가지 기본 유형
UENUM(BlueprintType)
enum class EAnomalyType : uint8
{
	// 미해결 시 사라지고 패널티 스택
	AT_Disappearing UMETA(DisplayName = "Disappearing"),

	// 미해결 시 쫓아오는 개체 스폰
	AT_Chasing      UMETA(DisplayName = "Chasing")
};

// '소지' UI에 표시될 이상현상의 범주 (예시)
UENUM(BlueprintType)
enum class EAnomalyCategory : uint8
{
	AC_None         UMETA(DisplayName = "None"),
	AC_Visual       UMETA(DisplayName = "Visual Anomaly"),
	AC_Auditory     UMETA(DisplayName = "Auditory Anomaly"),
	AC_Object       UMETA(DisplayName = "Object Anomaly"),
	AC_Environmental UMETA(DisplayName = "Environmental Anomaly")
};


// 쫓아오는 이상현상의 AI 상태
UENUM(BlueprintType)
enum class EAnomalyState : uint8
{
	// 플레이어를 감지하지 못하고 가만히 있는 상태
	EAS_Idle UMETA(DisplayName = "Idle"),

	// 플레이어를 감지하고 쫓아가는 상태
	EAS_Chasing UMETA(DisplayName = "Chasing")
};


/**
 * 도구의 종류를 정의
 * BP_Tool 블루프린트에서 이 타입을 설정하여 다양한 도구를 만듬
 */
UENUM(BlueprintType)
enum class EToolType : uint8
{
	// (예시) BP_CameraTool은 ETT_Camera로 설정
	ETT_None		UMETA(DisplayName = "None"),
	ETT_Bat 		UMETA(DisplayName = "Bat Tool"),
	ETT_Salt		UMETA(DisplayName = "Salt Tool"),
	ETT_EMFReader	UMETA(DisplayName = "EMF Reader Tool")
	// ... 필요한 만큼 도구 추가 ...
};
