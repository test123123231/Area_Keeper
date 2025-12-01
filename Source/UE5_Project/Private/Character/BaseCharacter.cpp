#include "Character/BaseCharacter.h"
#include "Components/AttributeComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false; // 플레이어가 아니므로 틱이 필요 없음

	// 체력을 관리할 AttributeComponent 생성
	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));

}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}

/**
 * 데미지를 AttributeComponent에 전달
 */
void ABaseCharacter::HandleDamage(float DamageAmount)
{
	if (Attributes && IsAlive())
	{
		Attributes->ReceiveDamage(DamageAmount);

		if (!IsAlive())
		{
			Die();
		}
	}
}

/**
 * 캐릭터가 죽었을 때(체력 0) 호출
 */
void ABaseCharacter::Die()
{
	// 게임오버 로직 (GameMode에 알림) 추가 해야 함
	UE_LOG(LogTemp, Warning, TEXT("%s has died. GAME OVER."), *GetName());

	// 캡슐 충돌 비활성화
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

/**
 * AttributeComponent를 통해 생존 여부를 확인합니다.
 */
bool ABaseCharacter::IsAlive()
{
	return Attributes && Attributes->IsAlive();
}

