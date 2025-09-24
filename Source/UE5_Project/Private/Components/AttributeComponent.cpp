#include <Components/AttributeComponent.h>
#include "GameFramework/Actor.h"

UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	OnHealthChanged.Broadcast(MaxHealth);
	OnAmuletChanged.Broadcast(MaxAmulet);
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

float UAttributeComponent::GetMaxHelath()
{
	return MaxHealth;
}

void UAttributeComponent::SetAmulet(float NewAmulet)
{
	Amulet = FMath::Clamp(NewAmulet, 0.f, MaxAmulet);
	OnAmuletChanged.Broadcast(NewAmulet);
}

void UAttributeComponent::SetMaxAmulet(float NewMaxAmulet)
{
	MaxAmulet = FMath::Max(0.f, NewMaxAmulet);
}

float UAttributeComponent::GetMaxAmulet()
{
	return MaxAmulet;
}



void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}


//for test
#if WITH_EDITOR
void UAttributeComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (!PropertyChangedEvent.Property) return;

	// 값 정리
	MaxHealth = FMath::Max(0.f, MaxHealth);
	Health    = FMath::Clamp(Health, 0.f, MaxHealth);
	MaxAmulet = FMath::Max(0.f, MaxAmulet);
	Amulet    = FMath::Clamp(Amulet, 0.f, MaxAmulet);

	const FName Name = PropertyChangedEvent.Property->GetFName();

	if (Name == GET_MEMBER_NAME_CHECKED(UAttributeComponent, Health) ||
		Name == GET_MEMBER_NAME_CHECKED(UAttributeComponent, MaxHealth))
	{
		OnHealthChanged.Broadcast(Health);
	}
	if (Name == GET_MEMBER_NAME_CHECKED(UAttributeComponent, Amulet) ||
		Name == GET_MEMBER_NAME_CHECKED(UAttributeComponent, MaxAmulet))
	{
		OnAmuletChanged.Broadcast(Amulet);
	}
}
#endif