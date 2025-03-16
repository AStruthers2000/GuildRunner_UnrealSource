#include "Action_AddRemoveUnitFromGrid.h"

#include "GuildRunner/Combat/CombatSystem.h"
#include "GuildRunner/Grid/CombatGrid.h"
#include "GuildRunner/Player/PlayerGridActions.h"
#include "GuildRunner/Units/CombatGridUnit.h"

AAction_AddRemoveUnitFromGrid::AAction_AddRemoveUnitFromGrid()
{
}



void AAction_AddRemoveUnitFromGrid::ExecuteGridAction(FIntPoint TileIndex)
{
	Super::ExecuteGridAction(TileIndex);
	if (!PlayerGridActions)
	{
		return;
	}
	
	if (bIsAddingUnit)
	{
		AddUnitOnTile(TileIndex);
	}
	else
	{
		RemoveUnitOnTile(TileIndex);
	}
}

void AAction_AddRemoveUnitFromGrid::AddUnitOnTile(const FIntPoint& TileIndex) const
{
	bool bIsWalkable = PlayerGridActions->GetCombatGridReference()->IsTileWalkable(TileIndex);
	bool bIsOccupied = PlayerGridActions->GetCombatGridReference()->IsTileOccupiedByBlockingObject(TileIndex);
	
	if (bIsWalkable && !bIsOccupied)
	{
		if (auto* SpawnedUnit = SpawnUnit())
		{
			PlayerGridActions->GetCombatSystemReference()->AddUnitInCombat(SpawnedUnit, TileIndex);
		}
	}
	else
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Failed to spawn unit on this tile. Reason: IsWalkable: %hs, IsOccupied: %hs"),
			bIsWalkable ? "true" : "false",
			bIsOccupied ? "true" : "false");
	}
}

void AAction_AddRemoveUnitFromGrid::RemoveUnitOnTile(const FIntPoint& TileIndex) const
{
	PlayerGridActions->GetCombatSystemReference()->RemoveUnitFromTile(TileIndex);
}


ACombatGridUnit* AAction_AddRemoveUnitFromGrid::SpawnUnit() const
{
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParameters.bDeferConstruction = true;

	ACombatGridUnit* SpawnedCombatUnit = nullptr;

	if (UnitType != NoUnitSelected && UnitTypeMapping.Contains(UnitType))
	{
		const auto ClassToSpawn = UnitTypeMapping[UnitType];
		SpawnedCombatUnit = GetWorld()->SpawnActor<ACombatGridUnit>(ClassToSpawn, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParameters);
		if (SpawnedCombatUnit)
		{
			SpawnedCombatUnit->SetUnitType(UnitType);
			SpawnedCombatUnit->DispatchBeginPlay();
		}
	}

	else
	{
		UE_LOG(LogTemp, Error,
			   TEXT(
				   "[AAction_AddRemoveUnitFromGrid::ExecuteGridAction]:\tNo mapping exists for adding unit of type %s"
			   ), *UEnum::GetValueAsString(UnitType));
	}

	return SpawnedCombatUnit;
}
