// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatGridVisualizer.h"

#include "CombatGridMeshInstance.h"
#include "GuildRunner/Grid/CombatGrid.h"
#include "GuildRunner/Grid/GridShapes/GridShapeUtilities.h"
#include "Kismet/GameplayStatics.h"

UCombatGridVisualizer::UCombatGridVisualizer()
{
	PrimaryComponentTick.bCanEverTick = false;
	VisualGridMeshInstance = CreateDefaultSubobject<UCombatGridMeshInstance>(TEXT("Visual Grid Mesh Instance"));
	TacticalGridMeshInstance = CreateDefaultSubobject<UCombatGridMeshInstance>(TEXT("Tactical Grid Mesh Instance"));
}

void UCombatGridVisualizer::InitializeGridVisual(ACombatGrid* Grid)
{
	GridReference = Grid;
	const auto* ShapeData = GridReference->GetCurrentShapeData();

	VisualGridMeshInstance->InitializeGridMeshInstance(
		ShapeData->FlatMesh,
		ShapeData->FlatMaterial,
		false,
		ECollisionEnabled::QueryOnly
	);

	TacticalGridMeshInstance->InitializeGridMeshInstance(
		ShapeData->Mesh,
		ShapeData->MeshMaterial,
		true,
		ECollisionEnabled::QueryOnly
	);

	//Center grid in world
	SetWorldLocation(FVector(0, 0, 0));
	SetOffsetFromGround(GridOffsetFromGround);

	SetGridLowestZ();
	SetIsTacticalGridShowing(bIsTacticalGridShowing);
}

void UCombatGridVisualizer::SetOffsetFromGround(const float Offset)
{
	GridOffsetFromGround = Offset;
	VisualGridMeshInstance->AdjustInstanceMeshOffsetFromGround(GridOffsetFromGround);
}

void UCombatGridVisualizer::DestroyGridVisual()
{
	if (VisualGridMeshInstance)
	{
		VisualGridMeshInstance->ClearInstances();
	}

	if (TacticalGridMeshInstance)
	{
		TacticalGridMeshInstance->ClearInstances();
	}

	SetGridLowestZ();
}

void UCombatGridVisualizer::UpdateTileVisual(const FTileData Data)
{
	VisualGridMeshInstance->RemoveInstance(Data.Index);
	if (UGridShapeUtilities::IsTileTypeWalkable(Data.Type))
	{
		VisualGridMeshInstance->AddInstance(Data.Index, Data.Type, Data.Transform, Data.States);
	}

	UpdateTileVisual_Tactical(Data);
}

void UCombatGridVisualizer::SetIsTacticalGridShowing(const bool bIsTactical)
{
	bIsTacticalGridShowing = bIsTactical;

	//TacticalGridMeshInstance->SetHiddenInGame(!bIsTacticalGridShowing);
	TacticalGridMeshInstance->HideInstancedStaticMeshInGame(!bIsTacticalGridShowing);
	TArray<AActor*> NonTacticalActors;
	UGameplayStatics::GetAllActorsWithTag(this, "NotTactical", NonTacticalActors);
	for (const auto& NonTacticalActor : NonTacticalActors)
	{
		NonTacticalActor->SetActorHiddenInGame(bIsTacticalGridShowing);
	}

	if (bIsTactical && bNeedToRegenerateTacticalOnNextEnable)
	{
		bNeedToRegenerateTacticalOnNextEnable = false;
		for (const auto& Pair : GridReference->GetGridTiles())
		{
			UpdateTileVisual_Tactical(Pair.Value);
		}
	}
}

void UCombatGridVisualizer::SetGridLowestZ(const float Z)
{
	GridLowestZ = Z;
	bNeedToRegenerateTacticalOnNextEnable = true;
}

void UCombatGridVisualizer::UpdateTileVisual_Tactical(const FTileData& Data)
{
	if (Data.Transform.GetLocation().Z < GridLowestZ)
	{
		SetGridLowestZ(Data.Transform.GetLocation().Z);
	}

	if (bIsTacticalGridShowing)
	{
		TacticalGridMeshInstance->RemoveInstance(Data.Index);
		if (Data.Type != NoTile)
		{
			FTransform TacticalTileTransform;
			TacticalTileTransform.SetLocation(Data.Transform.GetLocation());
			TacticalTileTransform.SetRotation(Data.Transform.GetRotation());

			const float ShapeMeshHeight = GridReference->GetCurrentShapeData()->MeshSize.Z;
			const float ModifiedZScale = ((Data.Transform.GetLocation().Z - GridLowestZ) + ShapeMeshHeight) /
				ShapeMeshHeight;
			TacticalTileTransform.SetScale3D(FVector(Data.Transform.GetScale3D().X, Data.Transform.GetScale3D().Y,
			                                         ModifiedZScale));
			TacticalGridMeshInstance->AddInstance(Data.Index, Data.Type, TacticalTileTransform, Data.States);
		}
	}
	else
	{
		bNeedToRegenerateTacticalOnNextEnable = true;
	}
}
