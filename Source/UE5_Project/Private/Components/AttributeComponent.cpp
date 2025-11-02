#include <Components/AttributeComponent.h>
#include "GameFramework/Actor.h"

UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
	//초기화
	OnHealthChanged.Broadcast(Health);
	OnTalismanChanged.Broadcast(Talisman);
}


void UAttributeComponent::ReceiveDamage(float DamageAmount)
{
	Health = FMath::Clamp(Health - DamageAmount, 0.f, MaxHealth);
	OnHealthChanged.Broadcast(Health);

}


float UAttributeComponent::GetHealthPercent()
{
	return Health / MaxHealth;
}


bool UAttributeComponent::IsAlive()
{
	return Health > 0.f;
}


void UAttributeComponent::HealthInit(float NewMaxHealth, float NewHealth)
{
	SetMaxHealth(NewMaxHealth);
	SetHealth(NewHealth);
}


void UAttributeComponent::SetHealth(float NewHealth)
{
	Health = FMath::Clamp(NewHealth, 0.f, MaxHealth);
	OnHealthChanged.Broadcast(NewHealth);
}


void UAttributeComponent::SetMaxHealth(float NewMaxHealth)
{
	MaxHealth = FMath::Max(0.f, NewMaxHealth);
}


void UAttributeComponent::SetTalisman(float NewTalisman)
{
	Talisman = FMath::Clamp(NewTalisman, 0.f, MaxTalisman);
	OnTalismanChanged.Broadcast(NewTalisman);
}


void UAttributeComponent::SetMaxTalisman(float NewMaxTalisman)
{
	MaxTalisman = FMath::Max(0.f, NewMaxTalisman);
}


float UAttributeComponent::GetHelath()
{
	return Health;
}


float UAttributeComponent::GetMaxHelath()
{
	return MaxHealth;
}


float UAttributeComponent::GetTalisman()
{
	return Talisman;
}


float UAttributeComponent::GetMaxTalisman()
{
	return MaxTalisman;
}


void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}


//test용도 (추후 삭제)
#if WITH_EDITOR
void UAttributeComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (!PropertyChangedEvent.Property) return;

	// 값 정리
	MaxHealth = FMath::Max(0.f, MaxHealth);
	Health    = FMath::Clamp(Health, 0.f, MaxHealth);
	MaxTalisman = FMath::Max(0.f, MaxTalisman);
	Talisman = FMath::Clamp(Talisman, 0.f, MaxTalisman);

	const FName Name = PropertyChangedEvent.Property->GetFName();

	if (Name == GET_MEMBER_NAME_CHECKED(UAttributeComponent, Health) ||
		Name == GET_MEMBER_NAME_CHECKED(UAttributeComponent, MaxHealth))
	{
		OnHealthChanged.Broadcast(Health);
	}
	if (Name == GET_MEMBER_NAME_CHECKED(UAttributeComponent, Talisman) ||
		Name == GET_MEMBER_NAME_CHECKED(UAttributeComponent, MaxTalisman))
	{
		OnTalismanChanged.Broadcast(Talisman);
	}
}
#endif