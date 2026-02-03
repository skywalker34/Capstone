// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyShip.h"
#include <GameFramework/FloatingPawnMovement.h>

AEnemyShipPawn::AEnemyShipPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	ShipMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
	RootComponent = ShipMesh;
	ShipMesh->SetEnableGravity(false);

	ShipMesh->SetSimulatePhysics(false);
	ShipMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ShipMesh->SetCollisionObjectType(ECC_Pawn);
	ShipMesh->SetCollisionResponseToAllChannels(ECR_Block);
	ShipMesh->SetNotifyRigidBodyCollision(true);

	MovementComp = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComp"));
	MovementComp->MaxSpeed = 1500.f;

	Muzzle = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle"));
	Muzzle->SetupAttachment(ShipMesh);
}

void AEnemyShipPawn::BeginPlay()
{
	Super::BeginPlay();

	APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	if (PlayerPawn)
	{
		Target = PlayerPawn;
	}

	if (Waypoints.Num() > 0)
	{
		CurrentWaypointIndex = 0;
	}
}

void AEnemyShipPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!Target) return;

	Chase(DeltaTime);

	NextState();

	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Cyan, FString::Printf(TEXT("Current State: %s"), *StateName));
	Action(DeltaTime);
}


void AEnemyShipPawn::Chase(float DeltaTime)
{
	FVector ToTarget = Target->GetActorLocation() - GetActorLocation();
	float Distance = ToTarget.Size();
	ToTarget.Normalize();

	FRotator CurrentRot = GetActorRotation();
	FRotator DesiredRot = ToTarget.Rotation();

	float MaxTurnThisFrame = MaxTurnRate * DeltaTime;
	float NewYaw = FMath::FixedTurn(CurrentRot.Yaw, DesiredRot.Yaw, MaxTurnThisFrame);
	float NewPitch = FMath::FixedTurn(CurrentRot.Pitch, DesiredRot.Pitch, MaxTurnThisFrame);
	SetActorRotation(FRotator(NewPitch, NewYaw, 0.f));

	float RightDot = FVector::DotProduct(GetActorRightVector(), ToTarget);
	float TargetRoll = FMath::Clamp(RightDot * MaxRollAngle, -MaxRollAngle, MaxRollAngle);

	FRotator MeshRotation = ShipMesh->GetRelativeRotation();
	MeshRotation.Roll = FMath::FInterpTo(MeshRotation.Roll, TargetRoll, DeltaTime, RollSpeed);
	ShipMesh->SetRelativeRotation(MeshRotation);

	const float IdealDistance = 3000.f;
	const float DistanceTolerance = 1000.f;

	float DesiredSpeed = PatrolSpeed;
	if (Distance < IdealDistance - DistanceTolerance)
		DesiredSpeed = PatrolSpeed;
	else if (Distance > IdealDistance + DistanceTolerance)
		DesiredSpeed = ChaseSpeed;

	CurrentSpeed = FMath::FInterpTo(CurrentSpeed, DesiredSpeed, DeltaTime, 0.1f);
	FVector NewLocation = GetActorLocation() + GetActorForwardVector() * CurrentSpeed * DeltaTime;
	SetActorLocation(NewLocation, true);


	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Cyan, FString::Printf(TEXT("Current Speed: %f"), CurrentSpeed));

}

//void AEnemyShipPawn::Chase(float DeltaTime) { 
//	FVector ToTarget = Target->GetActorLocation() - GetActorLocation(); 
//	ToTarget.Normalize(); FRotator TargetRotation = ToTarget.Rotation();
//	SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, TurnSpeed));
//	float RightDot = FVector::DotProduct(GetActorRightVector(), ToTarget);
//	float TargetRoll = RightDot * MaxRollAngle;
//	FRotator MeshRotation = ShipMesh->GetRelativeRotation();
//	MeshRotation.Roll = FMath::FInterpTo(MeshRotation.Roll, TargetRoll, DeltaTime, RollSpeed);
//	ShipMesh->SetRelativeRotation(MeshRotation);
//	CurrentSpeed = (Target->GetActorLocation() - GetActorLocation()).Size() < 1000 ? (CurrentSpeed - 100) : ChaseSpeed;
//	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Cyan, FString::Printf(TEXT("Current Speed: %f"), CurrentSpeed));
//	FVector NewLocation = GetActorLocation() + GetActorForwardVector() * CurrentSpeed * DeltaTime; 
//  SetActorLocation(NewLocation, true);
//}

void AEnemyShipPawn::Patrol(float DeltaTime)
{
	if (Waypoints.Num() == 0) return;

	AActor* CurrentWP = Waypoints[CurrentWaypointIndex];
	if (!CurrentWP) return;

	FVector ToTarget = CurrentWP->GetActorLocation() - GetActorLocation();
	float DistanceToWaypoint = ToTarget.Length();

	ToTarget.Normalize();

	FRotator TargetRotation = ToTarget.Rotation();
	SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, TurnSpeed));

	float RightDot = FVector::DotProduct(GetActorRightVector(), ToTarget);
	float TargetRoll = RightDot * MaxRollAngle;
	FRotator MeshRotation = ShipMesh->GetRelativeRotation();
	MeshRotation.Roll = FMath::FInterpTo(MeshRotation.Roll, TargetRoll, DeltaTime, RollSpeed);
	ShipMesh->SetRelativeRotation(MeshRotation);

	FVector NewLocation = GetActorLocation() + GetActorForwardVector() * PatrolSpeed * DeltaTime;
	SetActorLocation(NewLocation);

	if (DistanceToWaypoint < 100.f)
	{
		CurrentWaypointIndex++;

		if (CurrentWaypointIndex >= Waypoints.Num())
		{
			CurrentWaypointIndex = 0;
		}
	}
}

void AEnemyShipPawn::Attack(float DeltaTime)
{

	if (!ProjectileClass || !Muzzle) return;

	FVector SpawnLocation = Muzzle->GetComponentLocation();
	FRotator SpawnRotation = Muzzle->GetComponentRotation();
	FActorSpawnParameters Params;
	Params.Owner = this;

	FireTimer += DeltaTime;
	if (FireTimer >= FireInterval) {
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Cyan, FString::Printf(TEXT("FIRE")));
		GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnLocation, SpawnRotation, Params);
		FireTimer = 0.0f;
	}
}

void AEnemyShipPawn::Action(float DeltaTime)
{
	switch (CurrentState)
	{
	case EEnemyState::Patrol:
		StateName = "Patrol";
		Patrol(DeltaTime);
		break;

	case EEnemyState::Chase:
		StateName = "Chase";
		Chase(DeltaTime);
		break;

	case EEnemyState::Attack:
		StateName = "Attack";
		Attack(DeltaTime);
		break;
	}
}

void AEnemyShipPawn::NextState()
{
	float TargetDistance = FVector::Dist(Target->GetActorLocation(), GetActorLocation());

	switch (CurrentState)
	{
	case EEnemyState::Patrol:
		if (TargetDistance < ChaseRange)
			CurrentState = EEnemyState::Chase;
		CurrentSpeed = ChaseSpeed;
		break;

	case EEnemyState::Chase:
		if (TargetDistance > ChaseRange * 1.2f) {
			CurrentState = EEnemyState::Patrol;
		}
		if (TargetDistance < AttackRange)
			CurrentState = EEnemyState::Attack;

		break;
	case EEnemyState::Attack:
		if (TargetDistance > AttackRange * 1.2f)
			CurrentState = EEnemyState::Chase;
		break;
	}

}


