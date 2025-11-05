#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHPChanged, float, NewHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTalismanChanged, float, NewTalisman);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE5_PROJECT_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAttributeComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

private:
	// 현재 Health
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Health;
	// 현재 MaxHealth
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxHealth;
	// 현재 talisman
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Talisman;
	// 현재 Maxtalisman
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxTalisman;

public:
	void ReceiveDamage(float DamageAmount);
	float GetHealthPercent();
	bool IsAlive();
	void HealthInit(float MaxHealth, float NewHealth);
	
	void SetHealth(float NewHealth);
	void SetMaxHealth(float NewMaxHealth);
	void SetTalisman(float NewTalisman);
	void SetMaxTalisman(float NewMaxTalisman);
	
	float GetHelath();
	float GetMaxHelath();
	float GetTalisman();
	float GetMaxTalisman();

//event
public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHPChanged OnHealthChanged;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnTalismanChanged OnTalismanChanged;


//test용도 (추후 삭제)
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
