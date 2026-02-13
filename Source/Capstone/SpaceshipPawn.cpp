// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceshipPawn.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/StaticMeshComponent.h"

ASpaceshipPawn::ASpaceshipPawn()
{
	CurrentSpeed = MinSpeed;
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
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(ShipMesh);
	SpringArm->TargetArmLength = 300.0f;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	Muzzle = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle"));
	Muzzle->SetupAttachment(ShipMesh);

	InitialSpringArmLocation = SpringArm->GetRelativeLocation();
	InitialSpringArmRotation = SpringArm->GetRelativeRotation();

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void ASpaceshipPawn::BeginPlay()
{
	Super::BeginPlay();
	SpringArm->bUsePawnControlRotation = false;
	Camera->bUsePawnControlRotation = false;
}

void ASpaceshipPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CurrentSpeed = FMath::Max(CurrentSpeed, MinSpeed);

	FVector ForwardDir = GetActorForwardVector();
	FVector RightDir = GetActorRightVector();
	FVector UpDir = GetActorUpVector();

	RightDir.Z = 0.f;
	RightDir.Normalize();

	FVector MoveDir = ForwardDir + RightDir * YawInput;
	MoveDir = MoveDir.GetClampedToMaxSize(1.0f);
	MovementComp->MaxSpeed = CurrentSpeed;

	if (FMath::Abs(RollInput) < 0.01f && FMath::Abs(FlipInput) < 0.01f)
	{
		FRotator CurrentRot = ShipMesh->GetRelativeRotation();

		float TargetRoll = YawInput * MaxRollAngle;
		float NewRoll = FMath::FInterpTo(CurrentRot.Roll, TargetRoll, DeltaTime, RollSpeed * 0.5f);

		float TargetPitch = PitchInput * MaxPitchTilt;
		float NewPitch = FMath::FInterpTo(CurrentRot.Pitch, TargetPitch, DeltaTime, PitchSpeed * 0.5f);

		ShipMesh->SetRelativeRotation(FRotator(NewPitch, CurrentRot.Yaw, NewRoll));
	}

	if (bIsHoldingSwitchCamera)
	{
		SwitchCamera();
	}
	else {
		SpringArm->SetRelativeLocation(InitialSpringArmLocation);
		SpringArm->SetRelativeRotation(InitialSpringArmRotation);
	}

	AddMovementInput(GetActorForwardVector(), 1.0f);
}

void ASpaceshipPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	PlayerInputComponent->BindAction("SwitchCamera", IE_Pressed, this, &ASpaceshipPawn::OnSwitchCameraPressed);
	PlayerInputComponent->BindAction("SwitchCamera", IE_Released, this, &ASpaceshipPawn::OnSwitchCameraReleased);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASpaceshipPawn::Fire);
	PlayerInputComponent->BindAxis("Pitch", this, &ASpaceshipPawn::Pitch);
	PlayerInputComponent->BindAxis("Yaw", this, &ASpaceshipPawn::Yaw);
	PlayerInputComponent->BindAxis("Roll", this, &ASpaceshipPawn::Roll);
	PlayerInputComponent->BindAxis("Flip", this, &ASpaceshipPawn::Flip);
	PlayerInputComponent->BindAxis("Accelerate", this, &ASpaceshipPawn::Accelerate);

}

void ASpaceshipPawn::Accelerate(float Value)
{
	CurrentSpeed += Acceleration * Value * GetWorld()->GetDeltaSeconds();
	CurrentSpeed = CurrentSpeed > MaxSpeed ? MaxSpeed : CurrentSpeed;
}


void ASpaceshipPawn::SwitchCamera()
{
	FVector Forward = GetActorForwardVector();
	FVector NewCameraLocation = GetActorLocation() + Forward * 300.f + FVector(0.f, 0.f, 100.f);

	SpringArm->TargetArmLength = 300.f;
	FRotator LookRotation = (-Forward).Rotation();
	SpringArm->SetWorldRotation(LookRotation);
}

void ASpaceshipPawn::OnSwitchCameraPressed()
{
	bIsHoldingSwitchCamera = true;
}

void ASpaceshipPawn::OnSwitchCameraReleased()
{
	bIsHoldingSwitchCamera = false;
}

void ASpaceshipPawn::Pitch(float Value)
{
	PitchInput = Value;
	AddActorLocalRotation(FRotator(Value * PitchSpeed * GetWorld()->GetDeltaSeconds(), 0.f, 0.f));
}

void ASpaceshipPawn::Yaw(float Value)
{
	YawInput = Value;
	AddActorLocalRotation(FRotator(0.f, Value * YawSpeed * GetWorld()->GetDeltaSeconds(), 0.f));
}

void ASpaceshipPawn::Roll(float Value)
{
	RollInput = Value;
	if (Value != 0.f) {
		FRotator CurrentRotation = ShipMesh->GetRelativeRotation();
		float NewRoll = FMath::FInterpTo(CurrentRotation.Roll, Value * 90, GetWorld()->GetDeltaSeconds(), RollSpeed);
		ShipMesh->SetRelativeRotation(FRotator(CurrentRotation.Pitch, CurrentRotation.Yaw, NewRoll));
	}
}

void ASpaceshipPawn::Flip(float Value)
{
	FlipInput = Value;
	if (Value != 0.f) {
		FRotator CurrentRotation = ShipMesh->GetRelativeRotation();
		float NewRoll = FMath::FInterpTo(CurrentRotation.Roll, FlipInput * 180, GetWorld()->GetDeltaSeconds(), RollSpeed);
		ShipMesh->SetRelativeRotation(FRotator(CurrentRotation.Pitch, CurrentRotation.Yaw, NewRoll));
	}

}

void ASpaceshipPawn::Fire()
{
	if (!ProjectileClass || !Muzzle) return;

	FVector CameraLocation;
	FRotator CameraRotation;
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->GetPlayerViewPoint(CameraLocation, CameraRotation);
	}
	else
	{
		return;
	}

	FVector TraceStart = CameraLocation;
	FVector TraceEnd = TraceStart + CameraRotation.Vector() * 10000.f;
	FHitResult HitResult;
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(this);

	bool isHit = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, TraceParams);
	FVector AimPoint = isHit ? HitResult.ImpactPoint : TraceEnd;

	FVector SpawnLocation = Muzzle->GetComponentLocation();
	FVector FireDir = (AimPoint - SpawnLocation).GetSafeNormal();
	FRotator SpawnRotation = FireDir.Rotation();

	FActorSpawnParameters Params;
	Params.Owner = this;
	GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnLocation, SpawnRotation, Params);
}
