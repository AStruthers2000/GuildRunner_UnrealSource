// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatSystem.h"

#include "GuildRunner/Grid/CombatGrid.h"
#include "GuildRunner/Units/CombatGridUnit.h"

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

void ACombatSystem::AddUnitInCombat(ACombatGridUnit* Unit, const FIntPoint Index)
{
	UnitsInCombat.Add(Unit);
	
	ManagedGrid->RegisterGridObjectWithTile(Unit, Index);
	Unit->OnCombatUnitReachedNewTile.AddDynamic(this, &ACombatSystem::ACombatSystem::OnUnitReachedNewTile);
	
	const auto* TargetTile = ManagedGrid->GetGridTiles().Find(Index);
	const FVector TargetLocation = TargetTile ? TargetTile->Transform.GetLocation() : FVector(-999'999.f);
	Unit->SetActorLocation(TargetLocation);
}

void ACombatSystem::RemoveUnitInCombat(ACombatGridUnit* Unit, const TOptional<bool>& bDestroyUnit)
{
	UnitsInCombat.Remove(Unit);

	ManagedGrid->UnregisterGridObjectWithTile(Unit);

	// if the optional isn't set, or if it is set and the value is set to true, we want to destroy unit
	if (!bDestroyUnit.IsSet() || bDestroyUnit.GetValue())
	{
		Unit->Destroy();
	}
}

void ACombatSystem::RemoveUnitFromTile(const FIntPoint& Index, const TOptional<bool>& bDestroyUnit)
{
	auto* Unit = ManagedGrid->TryGetUnitOnTile(Index);
	if (!Unit) return;

	RemoveUnitInCombat(Unit, bDestroyUnit);
}

void ACombatSystem::SetUnitIndexOnGrid(ACombatGridUnit* Unit, const FIntPoint& Index, const bool bForceUpdate)
{
	if (!Unit)
	{
		return;
	}

	/*
	//if the unit's index is already the index we want, we don't want to move the unit
	if (Unit->GetIndexOnGrid() != Index || bForceUpdate)
	{
		//remove the unit from the previous tile
		auto* PreviousTile = ManagedGrid->GetGridTiles().Find(Unit->GetIndexOnGrid());
		if (PreviousTile && PreviousTile->UnitOnTile == Unit)
		{
			//DANGEROUS MODIFICATION OF TILES ON GRID DIRECTLY!!!! THIS IS PROBABLY BAD TO DO
			//BUT WHEN COPYING THE TILE AND THEN UPDATING IT IN THE KEY, THE STATES ARRAY GETS
			//CORRUPTED. TO WORK AROUND THIS, WE DIRECTLY MODIFY THE TILE. DON'T MAKE A HABIT OF THIS!
			ManagedGrid->GetGridTilesRef()->Find(Unit->GetIndexOnGrid())->UnitOnTile = nullptr;
		}

		//add unit to the new tile
		Unit->SetIndexOnGrid(Index);
		if (Index != FIntPoint(FPATHFINDINGDATA_DEFAULT_INDEX))
		{
			auto* NextTile = ManagedGrid->GetGridTiles().Find(Index);
			if (NextTile)
			{
				//DANGEROUS MODIFICATION OF TILES ON GRID DIRECTLY!!!! THIS IS PROBABLY BAD TO DO
				//BUT WHEN COPYING THE TILE AND THEN UPDATING IT IN THE KEY, THE STATES ARRAY GETS
				//CORRUPTED. TO WORK AROUND THIS, WE DIRECTLY MODIFY THE TILE. DON'T MAKE A HABIT OF THIS!
				ManagedGrid->GetGridTilesRef()->Find(Index)->UnitOnTile = Unit;
			}
		}

		OnGridUnitIndexChanged.Broadcast(Unit);
	}

	//move tile to new location
	const auto* TargetTile = ManagedGrid->GetGridTiles().Find(Index);
	const FVector TargetLocation = TargetTile ? TargetTile->Transform.GetLocation() : FVector(-999'999.f);
	Unit->SetActorLocation(TargetLocation);
	*/
}

void ACombatSystem::PlaceUnitOnGrid(ACombatGridUnit* Unit, const FIntPoint& Index, const bool bForceUpdate)
{
	if (!Unit) return;

	
}

void ACombatSystem::OnGridGenerated()
{
	auto Temp_UnitsInCombat = UnitsInCombat;
	for (const auto& Unit : Temp_UnitsInCombat)
	{
		//does the tile still exist/is tile still walkable
		if (ManagedGrid->IsTileWalkable(Unit->GetIndexOnGrid()))
		{
			SetUnitIndexOnGrid(Unit, Unit->GetIndexOnGrid(), true);
		}
		else
		{
			RemoveUnitInCombat(Unit);
		}
	}
}

void ACombatSystem::OnTileDataUpdated(FIntPoint Index)
{
	auto Temp_UnitsInCombat = UnitsInCombat;
	for (const auto& Unit : Temp_UnitsInCombat)
	{
		if (Unit->GetIndexOnGrid() == Index)
		{
			//does the tile still exist/is tile still walkable
			if (ManagedGrid->IsTileWalkable(Unit->GetIndexOnGrid()))
			{
				SetUnitIndexOnGrid(Unit, Unit->GetIndexOnGrid());
				break;
			}
			else
			{
				RemoveUnitInCombat(Unit);
				break;
			}
		}
	}
}

void ACombatSystem::OnUnitReachedNewTile(ACombatGridUnit* Unit, FIntPoint Index)
{
	//Could also use this function to add some verification that the unit actually reached the new tile
	//or to check if the unit stepped on some traps, etc.
	SetUnitIndexOnGrid(Unit, Index);
}
