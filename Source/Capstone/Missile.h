// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Missile.generated.h"

UENUM(BlueprintType)
enum class EMissileState : uint8
{
	Chase  UMETA(DisplayName = "Chase"),
	Attack UMETA(DisplayName = "Attack")
};

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* MissileMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Missile")
	float MaxSpeed = 15000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Missile")
	float MaxTurnForce = 8000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Missile")
	float ActivateRange = 10000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Missile")
	float AvoidStrength = 12000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Missile")
	float AvoidDistance = 8000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Missile")
	float AvoidRadius = 10000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Missile")
	float ChasingHeightOffset = 2000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Missile")
	float AttackDistance = 2000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Missile")
	EMissileState MissileState = EMissileState::Chase;


public:
	FVector CurrentVelocity;
	AActor* Target;
	FVector TargetLocation;
	float Distance;

	bool IsActivate = false;
	void UpdateSteering(float DeltaTime);
	FVector CalculateAvoidForce();
	void Action();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
