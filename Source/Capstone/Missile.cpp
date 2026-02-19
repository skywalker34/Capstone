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
	Action();

	float TargetDistance = (Target->GetActorLocation() - GetActorLocation()).Length();
	if (!IsActivate) {
		IsActivate = TargetDistance < ActivateRange && Target->GetActorLocation().Z > GetActorLocation().Z;
	}
	else {
		UpdateSteering(DeltaTime);
	}
}

void AMissile::UpdateSteering(float DeltaTime)
{
	if (!Target) return;

	FVector MyLocation = GetActorLocation();
	FVector ToTarget = TargetLocation - MyLocation;

	if (ToTarget.IsNearlyZero()) return;

	FVector DesiredVelocity = ToTarget.GetSafeNormal() * MaxSpeed;
	FVector Steering = DesiredVelocity - CurrentVelocity;
	FVector AvoidForce = CalculateAvoidForce();
	FVector TotalSteering = Steering + AvoidForce;
	TotalSteering = TotalSteering.GetClampedToMaxSize(MaxTurnForce);

	CurrentVelocity += TotalSteering * DeltaTime;
	CurrentVelocity = CurrentVelocity.GetClampedToMaxSize(MaxSpeed);
	FVector NewLocation = MyLocation + CurrentVelocity * DeltaTime;
	SetActorLocation(NewLocation, true);

	if (!CurrentVelocity.IsNearlyZero())
	{
		SetActorRotation(CurrentVelocity.Rotation());
	}
}

FVector AMissile::CalculateAvoidForce()
{
	FHitResult Hit;

	FVector Start = GetActorLocation();
	FVector End = Start + GetActorForwardVector() * AvoidDistance;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool IsHit = GetWorld()->SweepSingleByChannel(
		Hit,
		Start,
		End,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(AvoidRadius),
		Params
	);

	if (IsHit && Hit.GetActor() && Hit.GetActor()->ActorHasTag("Enemy"))
	{
		FVector SideDir = FVector::CrossProduct(
			GetActorForwardVector(),
			Hit.ImpactNormal
		).GetSafeNormal();

		return SideDir * AvoidStrength;
	}

	return FVector::ZeroVector;
}

void AMissile::Action()
{
	FVector PlayerLocation = Target->GetActorLocation();
	TargetLocation = PlayerLocation + FVector(0, 0, ChasingHeightOffset);
	Distance = (PlayerLocation - GetActorLocation()).Length();
	switch (MissileState)
	{
	case EMissileState::Chase:
	{
		float DescendingDistance = AttackDistance * 2;
		if (Distance < DescendingDistance && Distance > AttackDistance) {
			float OffsetScale = (Distance - AttackDistance) / AttackDistance;
			TargetLocation = PlayerLocation + FVector(0, 0, ChasingHeightOffset * OffsetScale);
		}
		if (Distance < AttackDistance)
		{
			MissileState = EMissileState::Attack;
		}
		break;
	}

	case EMissileState::Attack:
		TargetLocation = PlayerLocation;
		break;
	}
}
