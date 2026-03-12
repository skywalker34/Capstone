// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Turret.generated.h"

UCLASS()
class CAPSTONE_API ATurret : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATurret();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BarrelMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret")
	USceneComponent* Muzzle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret")
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret")
	float Range = 20000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret")
	float BulletSpeed = 30000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret")
	float AimSpeed = 5000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret")
	float TargetOffset = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret")
	float ShootingConeAngle = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret")
	float FireInterval = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret")
	AActor* Target;

	float FireTimer = 0.0f;

	UFUNCTION(BlueprintImplementableEvent)
	void Fire();

	void AimAtTarget(float DeltaTime);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
