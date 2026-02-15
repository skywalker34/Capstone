// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Missile.generated.h"

UCLASS()
class CAPSTONE_API AMissile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMissile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MissileMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Missile")
	float MaxSpeed = 15000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Missile")
	float MaxTurnForce = 8000.f;


public:
	FVector CurrentVelocity;
	AActor* Target;
	void UpdateSteering(float DeltaTime);

	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
