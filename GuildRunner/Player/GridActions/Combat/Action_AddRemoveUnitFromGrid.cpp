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

	UE_LOG(LogTemp, Warning, TEXT("Action_AddRemoveUnitFromGrid::ExecuteGridAction:\tSorry, can't add units to the grid at this time\n"));

	if (bIsAddingUnit)
	{
		if (PlayerGridActions->GetCombatGridReference()->IsTileWalkable(TileIndex))
		{
			
		}
	}
	/*
	if (bIsAddingUnit)
	{
		if (PlayerGridActions->GetCombatGridReference()->IsTileWalkable(TileIndex))
		{
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnParameters.bDeferConstruction = true;

			RemoveUnitOnTile(TileIndex);

			if (UnitType != NoUnitSelected && UnitTypeMapping.Contains(UnitType))
			{
				const auto ClassToSpawn = UnitTypeMapping[UnitType];
				if (auto* SpawnedCombatUnit = GetWorld()->SpawnActor<ACombatGridUnit>(
					ClassToSpawn, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParameters))
				{
					SpawnedCombatUnit->SetUnitType(UnitType);
					SpawnedCombatUnit->DispatchBeginPlay();
					PlayerGridActions->GetCombatSystemReference()->AddUnitInCombat(SpawnedCombatUnit, TileIndex);
				}
			}

			else
			{
				UE_LOG(LogTemp, Error,
				       TEXT(
					       "[AAction_AddRemoveUnitFromGrid::ExecuteGridAction]:\tNo mapping exists for adding unit of type %s"
				       ), *UEnum::GetValueAsString(UnitType));
			}
		}
	}
	else
	{
		RemoveUnitOnTile(TileIndex);
	}
	*/
}

void AAction_AddRemoveUnitFromGrid::RemoveUnitOnTile(const FIntPoint& TileIndex) const
{
	//const auto* ClickedTile = PlayerGridActions->GetCombatGridReference()->GetGridTiles().Find(TileIndex);
	//if (ClickedTile && ClickedTile->UnitOnTile)
	//{
	//	PlayerGridActions->GetCombatSystemReference()->RemoveUnitInCombat(ClickedTile->UnitOnTile);
	//}
}
