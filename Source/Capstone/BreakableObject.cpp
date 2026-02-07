// Fill out your copyright notice in the Description page of Project Settings.


#include "BreakableObject.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "PhysicsProxy/GeometryCollectionPhysicsProxy.h"


// Sets default values for this component's properties
UBreakableObject::UBreakableObject()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UBreakableObject::BeginPlay()
{
	Super::BeginPlay();

	if (TargetCollection)
	{
		InitialTransforms = TargetCollection->GetLocalRestTransforms();
	}

}


// Called every frame
void UBreakableObject::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (TargetCollection && InitialTransforms.Num() > 0)
	{
		FGeometryDynamicCollection* Collection = TargetCollection->GetDynamicCollection();

		if (Collection)
		{
			// 直接存取 TransformArray
			//const TManagedArray<FTransform>& Transforms = Collection->GetTransform(0);

			GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Cyan, TEXT("Piece 0 Pos: ") + Collection->GetTransform(0).ToString());
			//if (Transforms.Num() > 0)
			//{
			//	FTransform T0 = Transforms[0];
			//	FString PosStr = T0.GetLocation().ToString();
			//	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Cyan, TEXT("Piece 0 Pos: ") + PosStr);
			//}
		}
	}
}

