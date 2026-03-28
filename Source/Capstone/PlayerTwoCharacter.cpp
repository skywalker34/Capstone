// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerTwoCharacter.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
APlayerTwoCharacter::APlayerTwoCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	CurrentSpeed = MinSpeed;
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

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
	SpringArm->TargetArmLength = CameraDistance;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	Muzzle = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle"));
	Muzzle->SetupAttachment(ShipMesh);

	InitialSpringArmLocation = SpringArm->GetRelativeLocation();
	InitialSpringArmRotation = SpringArm->GetRelativeRotation();

	AutoPossessPlayer = EAutoReceiveInput::Disabled;

}

// Called when the game starts or when spawned
void APlayerTwoCharacter::BeginPlay()
{
	Super::BeginPlay();

	SpringArm->bUsePawnControlRotation = false;
	Camera->bUsePawnControlRotation = false;
}

// Called every frame
void APlayerTwoCharacter::Tick(float DeltaTime)
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
	//CurrentVelocity = GetActorForwardVector() * CurrentSpeed;

	if (FMath::Abs(RollInput) < 0.01f && FMath::Abs(FlipInput) < 0.01f)
	{
		FRotator CurrentRot = ShipMesh->GetRelativeRotation();

		float TargetRoll = YawInput * MaxRollAngle;
		float NewRoll = FMath::FInterpTo(CurrentRot.Roll, TargetRoll, DeltaTime, RollSpeed * 0.5f);

		float TargetPitch = PitchInput * MaxPitchTilt;
		float NewPitch = FMath::FInterpTo(CurrentRot.Pitch, TargetPitch, DeltaTime, PitchSpeed * 0.5f);

		ShipMesh->SetRelativeRotation(FRotator(NewPitch, CurrentRot.Yaw, NewRoll));
	}

	if (IsHoldingSwitchCamera)
	{
		SwitchCamera();
	}
	else {
		SpringArm->SetRelativeLocation(InitialSpringArmLocation);
		SpringArm->SetRelativeRotation(InitialSpringArmRotation);
	}

	AddMovementInput(GetActorForwardVector(), 1.0f);
}

// Called to bind functionality to input
void APlayerTwoCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("SwitchCamera", IE_Pressed, this, &APlayerTwoCharacter::OnSwitchCameraPressed);
	PlayerInputComponent->BindAction("SwitchCamera", IE_Released, this, &APlayerTwoCharacter::OnSwitchCameraReleased);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APlayerTwoCharacter::Fire);
	PlayerInputComponent->BindAxis("Pitch", this, &APlayerTwoCharacter::Pitch);
	PlayerInputComponent->BindAxis("Yaw", this, &APlayerTwoCharacter::Yaw);
	PlayerInputComponent->BindAxis("Roll", this, &APlayerTwoCharacter::Roll);
	PlayerInputComponent->BindAxis("Flip", this, &APlayerTwoCharacter::Flip);
	PlayerInputComponent->BindAxis("Accelerate", this, &APlayerTwoCharacter::Accelerate);
}

void APlayerTwoCharacter::Accelerate(float Value)
{
	CurrentSpeed += Acceleration * Value * GetWorld()->GetDeltaSeconds();
	CurrentSpeed = CurrentSpeed > MaxSpeed ? MaxSpeed : CurrentSpeed;
}


void APlayerTwoCharacter::SwitchCamera()
{
	FVector Forward = GetActorForwardVector();
	FVector NewCameraLocation = GetActorLocation() + Forward * CameraDistance + FVector(0.f, 0.f, SpringArmOffset);

	SpringArm->TargetArmLength = CameraDistance;
	FRotator LookRotation = (-Forward).Rotation();
	SpringArm->SetWorldRotation(LookRotation);
}

void APlayerTwoCharacter::OnSwitchCameraPressed()
{
	IsHoldingSwitchCamera = true;
}

void APlayerTwoCharacter::OnSwitchCameraReleased()
{
	IsHoldingSwitchCamera = false;
}

void APlayerTwoCharacter::Pitch(float Value)
{
	PitchInput = Value;
	PitchSpeed = RollInput != 0.f ? 30 : 5;
	AddActorLocalRotation(FRotator(Value * PitchSpeed * GetWorld()->GetDeltaSeconds(), 0.f, 0.f));
}

void APlayerTwoCharacter::Yaw(float Value)
{
	YawInput = Value;
	AddActorLocalRotation(FRotator(0.f, Value * YawSpeed * GetWorld()->GetDeltaSeconds(), 0.f));
}

void APlayerTwoCharacter::Roll(float Value)
{
	RollInput = Value;
	if (Value != 0.f) {
		FRotator CurrentRotation = ShipMesh->GetRelativeRotation();
		float NewRoll = FMath::FInterpTo(CurrentRotation.Roll, Value * 90, GetWorld()->GetDeltaSeconds(), RollSpeed);
		ShipMesh->SetRelativeRotation(FRotator(CurrentRotation.Pitch, CurrentRotation.Yaw, NewRoll));
	}
}

void APlayerTwoCharacter::Flip(float Value)
{
	FlipInput = Value;
	if (Value != 0.f) {
		FRotator CurrentRotation = ShipMesh->GetRelativeRotation();
		float NewRoll = FMath::FInterpTo(CurrentRotation.Roll, FlipInput * 180, GetWorld()->GetDeltaSeconds(), RollSpeed);
		ShipMesh->SetRelativeRotation(FRotator(CurrentRotation.Pitch, CurrentRotation.Yaw, NewRoll));
	}

}

void APlayerTwoCharacter::Fire()
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
		GEngine->AddOnScreenDebugMessage(
			-1,            // Unique key to prevent the same message from being added multiple times (use -1 if uniqueness doesn't matter)
			5.0f,          // Duration the message is displayed (in seconds)
			FColor::Red,   // Color of the text
			FString(TEXT("No controller")) // The message itself, wrapped in FString and TEXT() macros
		);
		return;
	}


	FVector TraceStart = CameraLocation;
	FVector TraceEnd = TraceStart + CameraRotation.Vector() * 1000000.f;
	FHitResult HitResult;
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(this);

	bool isHit = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, TraceParams);
	FVector AimPoint = isHit ? HitResult.ImpactPoint : TraceEnd;

	FVector SpawnLocation = Muzzle->GetComponentLocation();
	FVector FireDir = (AimPoint - SpawnLocation).GetSafeNormal();
	FRotator SpawnRotation = FireDir.Rotation();

	//SpawnProjectile(SpawnLocation, SpawnRotation);

	FActorSpawnParameters Params;
	Params.Owner = this;
	GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnLocation, SpawnRotation, Params);
}

