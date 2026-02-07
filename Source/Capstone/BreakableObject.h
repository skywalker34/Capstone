// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "BreakableObject.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class CAPSTONE_API UBreakableObject : public USceneComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UBreakableObject();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Breakable")
	UGeometryCollectionComponent* TargetCollection;
	//UGeometryCollection* TargetCollection;

	TArray<FTransform> InitialTransforms;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


};
