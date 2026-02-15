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
		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		if (PlayerPawn)
		{
			Target = PlayerPawn;
		}
	}
}

// Called every frame
void ATurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!Target) return;

	FVector ToTarget = Target->GetActorLocation() - GetActorLocation();

	float MinRange = (BaseMesh->GetComponentLocation() - Muzzle->GetComponentLocation()).Size() * 2;

	if (ToTarget.Size() > Range || ToTarget.Size() < MinRange || ToTarget.Z < 0) return;

	//if (ToTarget.Size() > Range) return;

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

	FVector PredictionOffset = TargetVelocity.GetSafeNormal() * TargetOffset * FMath::RandRange(0, 3);

	FVector PredictedLocation = TargetLocation + PredictionOffset + TargetVelocity * time;
	FRotator DesiredRotation = (PredictedLocation - MuzzleLocation).Rotation();

	FRotator NewRotation = FMath::RInterpTo(BaseMesh->GetComponentRotation(), DesiredRotation, DeltaTime, AimSpeed);
	BaseMesh->SetWorldRotation(NewRotation);

	FVector Forward = Muzzle->GetForwardVector();

	//DrawDebugLine(GetWorld(), MuzzleLocation, PredictedLocation, FColor::Red, false, 0.f, 0, 2.f);

	FireTimer += DeltaTime;
	if (FireTimer >= FireInterval)
	{
		Fire();
		FireTimer = 0.0f;
	}
}

void ATurret::Fire()
{
	if (!ProjectileClass || !Muzzle) return;

	FVector SpawnLocation = Muzzle->GetComponentLocation();
	FRotator SpawnRotation = Muzzle->GetComponentRotation();

	FActorSpawnParameters Params;
	Params.Owner = this;

	GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnLocation, SpawnRotation, Params);
}

