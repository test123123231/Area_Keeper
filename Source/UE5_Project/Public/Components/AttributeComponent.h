#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHPChanged, float, NewHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAmuletChanged, float, NewAmulet);

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
	float Health = 3.0f;
	// 현재 MaxHealth
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxHealth = 3.0f;
	// 현재 Amulet
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Amulet = 5.0f;
	// 현재 MaxAmulet
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxAmulet = 5.0f;

public:
	void ReceiveDamage(float DamageAmount);
	float GetHealthPercent();
	bool IsAlive();
	void HealthInit(float MaxHealth, float NewHealth);
	void SetHealth(float NewHealth);
	void SetMaxHealth(float NewMaxHealth);
	float GetHelath();
	float GetAmulet();
	void SetAmulet(float NewAmulet);
	void SetMaxAmulet(float NewMaxAmulet);
	float GetMaxHelath();
	float GetMaxAmulet();

//event
public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHPChanged OnHealthChanged;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnAmuletChanged OnAmuletChanged;


//test용도 (추후 삭제)
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
