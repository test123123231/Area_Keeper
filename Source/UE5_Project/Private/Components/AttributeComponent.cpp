#include <Components/AttributeComponent.h>


UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
}


void UAttributeComponent::ReceiveDamage(float DamageAmount)
{
	Health = FMath::Clamp(Health - DamageAmount, 0.f, MaxHealth);

}


float UAttributeComponent::GetHealthPercent()
{
	return Health / MaxHealth;
}


bool UAttributeComponent::IsAlive()
{
	return Health > 0.f;
}

void UAttributeComponent::HealthInit(float MaxHealth, float NewHealth)
{
	SetMaxHealth(MaxHealth);
	SetHealth(NewHealth);
}


void UAttributeComponent::SetHealth(float NewHealth)
{
	Health = FMath::Clamp(NewHealth, 0.f, MaxHealth);
}


void UAttributeComponent::SetMaxHealth(float NewMaxHealth)
{
	MaxHealth = FMath::Max(0.f, NewMaxHealth);
}


void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}