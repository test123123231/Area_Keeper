#include "HUD/TalismanWidget.h"
#include "Components/Button.h"
#include "Character/PlayerCharacter.h"


void UTalismanWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 플레이어 캐릭터 참조 캐시 (기술서의 EventConstruct 로직)
	// GetOwningPlayerPawn()은 이 위젯을 생성한 컨트롤러가 소유한 폰(Pawn)을 반환
	PlayerCharacterRef = Cast<APlayerCharacter>(GetOwningPlayer()->GetPawn());

	// C++ 함수와 BP 버튼의 OnClicked 이벤트를 바인딩(연결)
	if (Button_VisualSense)
	{
		Button_VisualSense->OnClicked.AddDynamic(this, &UTalismanWidget::OnVisualSenseClicked);
	}
	if (Button_AuditorySense)
	{
		Button_AuditorySense->OnClicked.AddDynamic(this, &UTalismanWidget::OnAuditorySenseClicked);
	}
	if (Button_Object)
	{
		Button_Object->OnClicked.AddDynamic(this, &UTalismanWidget::OnObjectClicked);
	}
	if (Button_Environment)
	{
		Button_Environment->OnClicked.AddDynamic(this, &UTalismanWidget::OnEnvironmentClicked);
	}
	if (Button_GoBack)
	{
		Button_GoBack->OnClicked.AddDynamic(this, &UTalismanWidget::OnGoBackClicked);
	}
}

/**
 * 모든 버튼 클릭 이벤트를 처리하는 중앙 함수
 */
void UTalismanWidget::HandleSelection(EAnomalyCategory SelectedCategory)
{
	// 캐시된 플레이어 참조가 유효한지 확인
	if (PlayerCharacterRef)
	{
		// APlayerCharacter에 정의된 함수 FinishTalismanRitual 호출
		PlayerCharacterRef->FinishTalismanRitual(SelectedCategory);
	}
	else
	{
		// NativeConstruct에서 캐시 실패 시 오류 로그
		UE_LOG(LogTemp, Error, TEXT("TalismanWidget: PlayerCharacterRef가 유효하지 않습니다!"));
	}
}

//--- 각 버튼이 클릭되었을 때 헬퍼 함수를 호출 ---

void UTalismanWidget::OnVisualSenseClicked()
{
	// '시각' 범주 전달
	HandleSelection(EAnomalyCategory::AC_Visual);
}

void UTalismanWidget::OnAuditorySenseClicked()
{
	// '청각' 범주 전달
	HandleSelection(EAnomalyCategory::AC_Auditory);
}

void UTalismanWidget::OnObjectClicked()
{
	// '물체' 범주 전달
	HandleSelection(EAnomalyCategory::AC_Object);
}

void UTalismanWidget::OnEnvironmentClicked()
{
	// '환경' 범주 전달
	HandleSelection(EAnomalyCategory::AC_Environmental);
}

void UTalismanWidget::OnGoBackClicked()
{
	// '소지' 취소 (None 범주 전달)
	HandleSelection(EAnomalyCategory::AC_None);
}
