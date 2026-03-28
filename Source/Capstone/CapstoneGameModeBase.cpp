#include "CapstoneGameModeBase.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerStart.h"
#include "EnemyShip.h"
#include <Kismet/GameplayStatics.h>


ACapstoneGameModeBase::ACapstoneGameModeBase()
{

	DefaultPawnClass = nullptr;
}

void ACapstoneGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	//SpawnEnemy();
}

void ACapstoneGameModeBase::SpawnEnemy()
{
	TArray<AActor*> Waypoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), Waypoints);

	AActor* FirstWaypoint = nullptr;
	for (AActor* Actor : Waypoints)
	{
		if (Actor->GetClass()->GetName().Contains("BP_Waypoint"))
		{
			FirstWaypoint = Actor;
			break;
		}
	}

	if (!FirstWaypoint)
	{
		UE_LOG(LogTemp, Error, TEXT("No Waypoints Found!"));
		return;
	}

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC && PC->GetPawn() != nullptr && EnemyShipClass)
	{
		APawn* PlayerPawn = PC->GetPawn();

		FVector EnemySpawnLocation = FirstWaypoint->GetActorLocation();
		EnemySpawnLocation.Z = PlayerPawn->GetActorLocation().Z;
		FRotator EnemySpawnRotation = (PlayerPawn->GetActorLocation() - EnemySpawnLocation).Rotation();

		AEnemyShipPawn* EnemyPawn = Cast<AEnemyShipPawn>(
			GetWorld()->SpawnActor<AEnemyShipPawn>(EnemyShipClass, EnemySpawnLocation, EnemySpawnRotation)
		);
		if (EnemyPawn)
		{
			EnemyPawn->Target = PlayerPawn;
			UE_LOG(LogTemp, Warning, TEXT("Enemy spawned and target set"));
		}
	}
}
