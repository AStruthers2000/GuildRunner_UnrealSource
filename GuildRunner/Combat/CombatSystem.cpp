// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatSystem.h"

#include "Algo/ForEach.h"
#include "GuildRunner/Grid/CombatGrid.h"
#include "GuildRunner/Units/CombatGridUnit.h"
#include "GuildRunner/Units/CombatGridUnitMovement.h"

ACombatSystem::ACombatSystem()
{
	PrimaryActorTick.bCanEverTick = true;

	ManagedGrid = nullptr;
}

void ACombatSystem::BeginPlay()
{
	Super::BeginPlay();

	ManagedGrid->OnCombatGridGenerated.AddDynamic(this, &ACombatSystem::OnGridGenerated);
	ManagedGrid->OnTileDataUpdated.AddDynamic(this, &ACombatSystem::OnTileDataUpdated);
}



void ACombatSystem::AddObjectIntoCombat(ACombatGridObject* Object, const FIntPoint& Index)
{
	ObjectsInCombat.Add(Object);
	
	ManagedGrid->RegisterGridObjectWithTile(Object, Index);

	if (auto* Unit = Cast<ACombatGridUnit>(Object))
	{
		Unit->GetCombatGridUnitMovement()->OnCombatUnitStartedMovingToTargetTile.AddDynamic(this, &ACombatSystem::OnUnitStartedMovingToTargetTile);
		Unit->GetCombatGridUnitMovement()->OnCombatUnitStartedMovingToNewTile.AddDynamic(this, &ACombatSystem::OnUnitStartedMovingToNewTile);
		Unit->GetCombatGridUnitMovement()->OnCombatUnitReachedNewTile.AddDynamic(this, &ACombatSystem::OnUnitReachedNewTile);
		Unit->GetCombatGridUnitMovement()->OnCombatUnitFinishedWalking.AddDynamic(this, &ACombatSystem::OnUnitFinishedPathfinding);
	}
	
	UpdateUnitLocation(Object, Index);
}

void ACombatSystem::RemoveObjectFromCombat(ACombatGridObject* Object, const TOptional<bool>& bDestroyUnit)
{
	ObjectsInCombat.Remove(Object);

	ManagedGrid->UnregisterGridObjectWithTile(Object);

	// if the optional isn't set, or if it is set and the value is set to true, we want to destroy unit
	if (!bDestroyUnit.IsSet() || bDestroyUnit.GetValue())
	{
		Object->Destroy();
	}
}

void ACombatSystem::RemoveUnitFromTile(const FIntPoint& Index, const TOptional<bool>& bDestroyUnit)
{
	auto* Unit = ManagedGrid->TryGetUnitOnTile(Index);
	if (!Unit) return;

	RemoveObjectFromCombat(Unit, bDestroyUnit);
}

void ACombatSystem::SetUnitIndexOnGrid(ACombatGridObject* Object, const FIntPoint& Index, const TOptional<bool>& bForceUpdate) const
{
	if (!Object)
	{
		return;
	}

	// if the unit's index is already the index we want, we don't want to move the unit
	if (Object->GetIndexOnGrid() != Index || bForceUpdate)
	{
		// remove the unit from the tile it's currently on
		ManagedGrid->UnregisterGridObjectWithTile(Object);

		// add unit to new tile
		ManagedGrid->RegisterGridObjectWithTile(Object, Index);

		if (auto* Unit = Cast<ACombatGridUnit>(Object))
		{
			OnGridUnitIndexChanged.Broadcast(Unit);
		}
	}
}

void ACombatSystem::UpdateUnitLocation(ACombatGridObject* Unit, const FIntPoint& Index) const
{
	const auto* TargetTile = ManagedGrid->GetGridTiles().Find(Index);
	const FVector TargetLocation = TargetTile ? TargetTile->Transform.GetLocation() : FVector(-999'999.f);
	Unit->SetActorLocation(TargetLocation);
}

void ACombatSystem::UpdateUnitOnTileGeneration(ACombatGridObject* const& Unit, const TOptional<bool>& bForceUpdate)
{
	// does the tile still exist/is tile still walkable
	if (ManagedGrid->IsTileWalkable(Unit->GetIndexOnGrid()))
	{
		SetUnitIndexOnGrid(Unit, Unit->GetIndexOnGrid(), true);
	}
	else
	{
		RemoveObjectFromCombat(Unit);
	}
}

void ACombatSystem::OnGridGenerated()
{
	auto Temp_ObjectsInCombat = ObjectsInCombat;
	for (const auto& Unit : Temp_ObjectsInCombat)
	{
		UpdateUnitOnTileGeneration(Unit, true);
	}
}

void ACombatSystem::OnTileDataUpdated(const FIntPoint& Index)
{
	auto* Tile = ManagedGrid->GetGridTiles().Find(Index);
	if (!Tile) return;
	
	auto Temp_ObjectsOnTile = Tile->ObjectsOnTile;
	for (const auto& Unit : Temp_ObjectsOnTile)
	{
		UpdateUnitOnTileGeneration(Unit);
	}
}

void ACombatSystem::OnUnitStartedMovingToTargetTile(ACombatGridUnit* Unit, const FIntPoint& Index)
{
	SetUnitIndexOnGrid(Unit, Index);
}

void ACombatSystem::OnUnitStartedMovingToNewTile(ACombatGridUnit* Unit, const FIntPoint& Index)
{
	//auto CurrentIndex = Unit->GetIndexOnGrid();
	//ManagedGrid->DecrementTimesTileIsInPath(CurrentIndex);
	//ManagedGrid->DecrementTimesTileIsInPath(Index);
	//SetUnitIndexOnGrid(Unit, Index);
}

void ACombatSystem::OnUnitReachedNewTile(ACombatGridUnit* Unit, const FIntPoint& Index)
{
	//Could also use this function to add some verification that the unit actually reached the new tile
	//or to check if the unit stepped on some traps, etc.
	// TODO: if something stopped my movement, unregister myself from the target and register myself at current
	/*if (I am no longer moving due to some trap that triggered when I visually arrived at this tile)
	{
		SetUnitIndexOnGrid(Unit, Index);
		Somehow stop unit pathfinding from happening
	}*/
	

	ManagedGrid->DecrementTimesTileIsInPath(Index);
	
	// move unit to new tile
	UpdateUnitLocation(Unit, Index);
}

void ACombatSystem::OnUnitFinishedPathfinding(ACombatGridUnit* Unit)
{
	ManagedGrid->DecrementTimesTileIsInPath(Unit->GetIndexOnGrid());
	ManagedGrid->RemoveStateFromTile(Unit->GetIndexOnGrid(), PathfindingTarget);
}
