// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "EnemyShip.generated.h"

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	Patrol UMETA(DisplayName = "Patrol"),
	Chase  UMETA(DisplayName = "Chase"),
	Attack UMETA(DisplayName = "Attack")
};

UCLASS()
class CAPSTONE_API AEnemyShipPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AEnemyShipPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* ShipMesh;

	// Movement component (same as player)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UFloatingPawnMovement* MovementComp;

	float PitchInput;
	float YawInput;
	float RollInput;
	FString StateName;
	int CurrentWaypointIndex = 0;
	float CurrentSpeed = 3000;
	float FireInterval = 0.5f;
	float FireTimer = 0.0f;

	// --- Movement Settings (copy from your player pawn) ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyAI")
	float PitchSpeed = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyAI")
	float YawSpeed = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyAI")
	float RollSpeed = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyAI")
	float MaxRollAngle = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyAI")
	float MaxPitchTilt = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyAI")
	float TurnSpeed = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyAI")
	EEnemyState CurrentState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyAI")
	TArray<AActor*> Waypoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyAI")
	float ChaseRange = 5000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyAI")
	float AttackRange = 2000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyAI")
	float PatrolSpeed = 600.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyAI")
	float ChaseSpeed = 1200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyAI")
	float MaxAngularSpeed = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyAI")
	float BounceTurnSpeed = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyAI")
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "EnemyAI")
	USceneComponent* Muzzle;

	void Chase(float DeltaTime);
	void Patrol(float DeltaTime);
	void Attack(float DeltaTime);
	void Action(float DeltaTime);
	void NextState();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyAI")
	AActor* Target;

};
