// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyShip.h"
#include <GameFramework/FloatingPawnMovement.h>
#include <Kismet/KismetMathLibrary.h>

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

	NextState();

	Action(DeltaTime);
}


void AEnemyShipPawn::Chase(float DeltaTime) {
	if (!Target) return;

	FVector MyLocation = GetActorLocation();
	FVector TargetLocation = Target->GetActorLocation();

	//// ===== 新增：前方牆壁偵測 =====
	//FVector Forward = GetActorForwardVector();
	//FHitResult Hit;
	//FCollisionQueryParams Params;
	//Params.AddIgnoredActor(this);

	//bool bHitWall = GetWorld()->LineTraceSingleByChannel(
	//	Hit,
	//	MyLocation,
	//	MyLocation + Forward * TraceDistance,
	//	ECC_WorldStatic,
	//	Params
	//);

	//// ===== 新增：如果撞牆，設定避障目標 =====
	//if (bHitWall)
	//{
	//	IsAvoidingWall = true;

	//	FVector WallNormal = Hit.ImpactNormal;

	//	// 往牆法線方向設一個目標點
	//	AvoidTargetLocation = MyLocation + WallNormal * AvoidDistance;
	//}

	//// ===== 新增：決定現在追誰 =====
	//if (IsAvoidingWall)
	//{
	//	TargetLocation = AvoidTargetLocation;

	//	float DistToAvoid = FVector::Dist(MyLocation, AvoidTargetLocation);

	//	if (DistToAvoid < AvoidArriveDistance)
	//	{
	//		IsAvoidingWall = false;
	//	}
	//}
	//else
	//{
	//	TargetLocation = Target->GetActorLocation();
	//}

	FVector ToTarget = TargetLocation - MyLocation;

	float DistanceToTarget = ToTarget.Length();
	if (ToTarget.IsNearlyZero()) return;

	ToTarget.Normalize();

	FRotator CurrentRot = GetActorRotation();
	FRotator TargetRot = ToTarget.Rotation();

	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(TargetRot, CurrentRot);

	float MaxAngle = MaxAngularSpeed * DeltaTime;

	DeltaRot.Pitch = FMath::Clamp(DeltaRot.Pitch, -MaxAngle, MaxAngle);
	DeltaRot.Yaw = FMath::Clamp(DeltaRot.Yaw, -MaxAngle, MaxAngle);
	DeltaRot.Roll = 0.f;
	SetActorRotation(CurrentRot + DeltaRot);

	float SpeedScale = 1.f;

	if (DistanceToTarget < DesiredDistance + SlowDownRange)
	{
		SpeedScale = FMath::Clamp((DistanceToTarget - DesiredDistance) / SlowDownRange, 0.f, 1.f);
	}

	FVector NewLocation = MyLocation + GetActorForwardVector() * ChaseSpeed * SpeedScale * DeltaTime;
	SetActorLocation(NewLocation);
}

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
	//FRotator SpawnRotation = Muzzle->GetComponentRotation();
	FVector Forward = Muzzle->GetForwardVector();
	float ShootingConeAngleRad = FMath::DegreesToRadians(ShootingConeAngle);
	FVector RandomDir = FMath::VRandCone(Forward, ShootingConeAngleRad);
	FRotator SpawnRotation = RandomDir.Rotation();

	FActorSpawnParameters Params;
	Params.Owner = this;

	FireTimer += DeltaTime;
	if (FireTimer >= FireInterval) {
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
		Chase(DeltaTime);
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


