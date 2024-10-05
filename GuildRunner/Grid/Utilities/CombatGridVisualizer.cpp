// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatGridVisualizer.h"

#include "CombatGridMeshInstance.h"
#include "GuildRunner/Grid/CombatGrid.h"
#include "GuildRunner/Grid/GridShapes/GridShapeUtilities.h"

UCombatGridVisualizer::UCombatGridVisualizer()
{
	PrimaryComponentTick.bCanEverTick = false;
	GridMeshInstance = CreateDefaultSubobject<UCombatGridMeshInstance>(TEXT("Grid Mesh Instance"));
}

void UCombatGridVisualizer::InitializeGridVisual(ACombatGrid* Grid)
{
	GridReference = Grid;
	const auto* ShapeData = GridReference->GetCurrentShapeData();

	GridMeshInstance->InitializeGridMeshInstance(
		ShapeData->FlatMesh,
		ShapeData->FlatBorderMaterial,
		FLinearColor(0, 0, 0, 1),
		ECollisionEnabled::QueryOnly
		);
	
	//Center grid in world
	SetWorldLocation(FVector(0, 0, 0));
	SetOffsetFromGround(GridOffsetFromGround);
}

void UCombatGridVisualizer::SetOffsetFromGround(const float Offset)
{
	GridOffsetFromGround = Offset;
	GridMeshInstance->AdjustInstanceMeshOffsetFromGround(GridOffsetFromGround);
}

void UCombatGridVisualizer::DestroyGridVisual()
{
	if(!GridMeshInstance) return;

	GridMeshInstance->ClearInstances();
}

void UCombatGridVisualizer::UpdateTileVisual(FTileData Data)
{
	GridMeshInstance->RemoveInstance(Data.Index);
	if(UGridShapeUtilities::IsTileTypeWalkable(Data.Type))
	{
		GridMeshInstance->AddInstance(Data.Index, Data.Transform);
	}
}

