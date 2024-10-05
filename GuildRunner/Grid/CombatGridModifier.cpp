// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatGridModifier.h"

#include "GridShapes/GridShapeUtilities.h"
#include "Utilities/ETileType.h"

// Sets default values
ACombatGridModifier::ACombatGridModifier()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Grid Modifier Mesh"));

	auto* DefaultRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Default Root"));
	RootComponent = DefaultRoot;
	StaticMeshComponent->SetupAttachment(RootComponent);

	ConfigureMesh();
}


void ACombatGridModifier::ConfigureMesh()
{
	const auto* ShapeData = UGridShapeUtilities::GetShapeData(Shape);
	if(!ShapeData) return;
	StaticMeshComponent->SetStaticMesh(UGridShapeUtilities::GetShapeData(Shape)->Mesh);
	StaticMeshComponent->SetMaterial(0, UGridShapeUtilities::GetShapeData(Shape)->FlatMaterial);

	FLinearColor MeshColor = FLinearColor(0, 0, 0, 1);
	switch (TileType)
	{
	case Normal:
		MeshColor = FLinearColor(1, 1, 1, 1);
		break;
	case Obstacle:
		MeshColor = FLinearColor(1, 0, 0, 1);
		break;
	case NoTile:
	default: return;
	}
	StaticMeshComponent->SetVectorParameterValueOnMaterials("Color", FVector(MeshColor.R, MeshColor.G, MeshColor.B));
	StaticMeshComponent->SetScalarParameterValueOnMaterials("IsFilled", 1.0);

	StaticMeshComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);
	SetActorHiddenInGame(true);
}

#if WITH_EDITOR
void ACombatGridModifier::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if(bUpdateMesh)
	{
		ConfigureMesh();
		bUpdateMesh = false;
	}
}
#endif

