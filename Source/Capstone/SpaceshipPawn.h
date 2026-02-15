// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SpaceshipPawn.generated.h"

UCLASS()
class CAPSTONE_API ASpaceshipPawn : public APawn
{
	GENERATED_BODY()

public:
	ASpaceshipPawn();

protected:
	virtual void BeginPlay() override;

	// --- Components ---
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ShipMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
	class UFloatingPawnMovement* MovementComp;

	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* Camera;

	// --- Movement parameters ---

	UPROPERTY(EditAnywhere, Category = "Movement")
	float TurnRate = 80.0f;

	FVector CurrentVelocity;


	// --- Input functions ---
	void Fire();
	void SwitchCamera();
	void Pitch(float Value);
	void Yaw(float Value);
	void Roll(float Value);
	void Flip(float Value);
	void Accelerate(float Value);


	void OnSwitchCameraPressed();
	void OnSwitchCameraReleased();

	UPROPERTY()
	FVector InitialSpringArmLocation;

	UPROPERTY()
	FRotator InitialSpringArmRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
	float MinSpeed = 2000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
	float MaxSpeed = 6000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
	float Acceleration = 5000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
	float CurrentSpeed = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
	float PitchSpeed = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
	float YawSpeed = 60.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
	float MaxRollAngle = 60.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
	float MaxPitchTilt = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
	float RollSpeed = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
	float ReflectionConstant = 0.8f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
	float ReflectionOffset = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Flight")
	USceneComponent* Muzzle;

	float PitchInput = 0.0f;
	float YawInput = 0.0f;
	float RollInput = 0.0f;
	float FlipInput = 0.0f;

	float CurrentPitchAngle = 0.0f;
	float CurrentYawAngle = 0.0f;
	float CurrentRollAngle = 0.0f;

	bool IsHoldingSwitchCamera = false;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	UFUNCTION()
	void OnSpaceshipHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
