// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CapstoneGameModeBase.generated.h"

/**
 *
 */
UCLASS()
class CAPSTONE_API ACapstoneGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	ACapstoneGameModeBase();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	TSubclassOf<class APawn> EnemyShipClass;
};
