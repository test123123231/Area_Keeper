// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ReadyZone.generated.h"

UCLASS()
class UE5_PROJECT_API AReadyZone : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AReadyZone();

protected:
	UPROPERTY(VisibleAnywhere)
    class UBoxComponent* ZoneBox;

public:	
	UFUNCTION()
    void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
