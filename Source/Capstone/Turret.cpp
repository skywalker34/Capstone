// Fill out your copyright notice in the Description page of Project Settings.


#include "Turret.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
ATurret::ATurret()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	RootComponent = BaseMesh;

	BarrelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BarrelMesh"));
	BarrelMesh->SetupAttachment(BaseMesh);

	Muzzle = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle"));
	Muzzle->SetupAttachment(BarrelMesh);
}

// Called when the game starts or when spawned
void ATurret::BeginPlay()
{
	Super::BeginPlay();

	if (!Target)
	{
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), SpaceshipClass, FoundActors);

		if (FoundActors.Num() > 0)
		{
			Target = Cast<APawn>(FoundActors[0]);
		}
	}
}

// Called every frame
void ATurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!Target)
	{
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), SpaceshipClass, FoundActors);

		if (FoundActors.Num() > 0)
		{
			Target = Cast<APawn>(FoundActors[0]);
		}
	}

	if (!Target) return;

	FVector ToTarget = Target->GetActorLocation() - GetActorLocation();

	float MinRange = (BaseMesh->GetComponentLocation() - Muzzle->GetComponentLocation()).Size() * 2;

	if (ToTarget.Size() > Range || ToTarget.Size() < MinRange || ToTarget.Z < 0) return;


	AimAtTarget(DeltaTime);
}

void ATurret::AimAtTarget(float DeltaTime)
{
	if (!Target) return;

	FVector TargetVelocity = Target->GetVelocity();
	FVector relativeVelocity = TargetVelocity - BulletSpeed * Muzzle->GetForwardVector();
	FVector TargetLocation = Target->GetActorLocation();
	FVector MuzzleLocation = Muzzle->GetComponentLocation();
	FVector S = TargetLocation - MuzzleLocation;
	float time = S.Size() / relativeVelocity.Size();

	FVector PredictedLocation = TargetVelocity.IsNearlyZero() ? TargetLocation : TargetLocation + TargetVelocity * time;

	FRotator DesiredRotation = (PredictedLocation - MuzzleLocation).Rotation();

	FRotator NewRotation = FMath::RInterpTo(BaseMesh->GetComponentRotation(), DesiredRotation, DeltaTime, AimSpeed);
	BaseMesh->SetWorldRotation(NewRotation);

	FVector Forward = Muzzle->GetForwardVector();


	FireTimer += DeltaTime;
	if (FireTimer >= FireInterval)
	{
		Fire();
		FireTimer = 0.0f;
	}
}

