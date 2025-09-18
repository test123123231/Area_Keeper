#include "Character/BaseCharacter.h"
#include <Components/AttributeComponent.h>
#include "GameFramework/CharacterMovementComponent.h"


ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 360.f, 0.f);

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));
}


void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	Attributes->HealthInit(3.f, 3.f);
}


void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}