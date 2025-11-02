#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

class UAttributeComponent;

/**
 * APlayerCharacter를 위한 기본 부모 클래스
 * 체력에 필요한 AttributeComponent와
 * 생사(IsAlive), 피격(HandleDamage), 죽음(Die) 처리만 담당
 */
UCLASS()
class UE5_PROJECT_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseCharacter();

	/**
	 * 이 캐릭터가 생존 상태인지 확인
	 * UAttributeComponent가 유효하고 체력이 0보다 크면 true
	 */
	UFUNCTION(BlueprintPure, Category = "Attributes")
	bool IsAlive();

	/**
	 * 데미지를 처리
	 * DamageAmount 받은 데미지 양
	 */
	virtual void HandleDamage(float DamageAmount);

protected:
	virtual void BeginPlay() override;

	/**
	 * 캐릭터가 죽었을 때 호출
	 * GameMode에 게임오버를 알림
	 */
	virtual void Die();

	/**
	 * AttributeComponent (체력 및 지방 관리용)
	 * 초기 체력은 이 컴포넌트의 생성자에서 설정합니다.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAttributeComponent* Attributes;

public:
	// AttributeComponent에 쉽게 접근할 수 있도록 Getter 추가
	FORCEINLINE UAttributeComponent* GetAttributes() const { return Attributes; }
};

