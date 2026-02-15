// Fill out your copyright notice in the Description page of Project Settings.


#include "Missile.h"

// Sets default values
AMissile::AMissile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MissileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MissileMesh"));
	RootComponent = MissileMesh;

	MissileMesh->SetSimulatePhysics(false);
	MissileMesh->SetEnableGravity(false);
}

// Called when the game starts or when spawned
void AMissile::BeginPlay()
{
	Super::BeginPlay();
	APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	Target = PlayerPawn;
	CurrentVelocity = GetActorForwardVector() * MaxSpeed;
}

// Called every frame
void AMissile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateSteering(DeltaTime);
}

void AMissile::UpdateSteering(float DeltaTime)
{
	if (!Target) return;

	FVector MyLocation = GetActorLocation();
	FVector TargetLocation = Target->GetActorLocation();

	FVector ToTarget = TargetLocation - MyLocation;

	if (ToTarget.IsNearlyZero()) return;

	FVector DesiredVelocity = ToTarget.GetSafeNormal() * MaxSpeed;
	FVector Steering = DesiredVelocity - CurrentVelocity;
	Steering = Steering.GetClampedToMaxSize(MaxTurnForce);

	CurrentVelocity += Steering * DeltaTime;
	CurrentVelocity = CurrentVelocity.GetClampedToMaxSize(MaxSpeed);
	FVector NewLocation = MyLocation + CurrentVelocity * DeltaTime;
	SetActorLocation(NewLocation);

	if (!CurrentVelocity.IsNearlyZero())
	{
		SetActorRotation(CurrentVelocity.Rotation());
	}
}
