// Fill out your copyright notice in the Description page of Project Settings.


#include "TileSelectionManager.h"

#include "GroomVisualizationData.h"
#include "PlayerGridActions.h"
#include "GuildRunner/Grid/CombatGrid.h"
#include "Kismet/GameplayStatics.h"

UTileSelectionManager::UTileSelectionManager()
{
}

void UTileSelectionManager::UpdateTileUnderCursor()
{
	if (!GridReference)
	{
		return;
	}

	// auto* ObjectUnderCursor = GetGridObjectUnderCursor();
	// if (ObjectUnderCursor != HoveredGridObject)
	// {
	// 	//set the old unit as no longer hovered
	// 	if (HoveredGridObject)
	// 	{
	// 		HoveredGridObject->SetIsHovered(false);
	// 	}
	//
	// 	//update unit reference and set current unit to hovered
	// 	HoveredGridObject = ObjectUnderCursor;
	// 	if (HoveredGridObject)
	// 	{
	// 		HoveredGridObject->SetIsHovered(true);
	// 	}
	// }

	// FIntPoint NewTileIndex;
	// if (HoveredGridObject)
	// {
	// 	// units are sometimes taller than the tile they're standing on (if we are pointing the mouse at the "head" of a
	// 	// unit, the cursor might be intersecting with the tile "behind" the unit, not the tile the unit is on
	// 	// effectively, if the mouse raycast hits a unit before it hits the ground, we want to hover on the tile the
	// 	// unit is on, not the tile that the mouse raycast would actually hit
	// 	NewTileIndex = HoveredGridObject->GetIndexOnGrid();
	// }
	// else
	// {
	// 	NewTileIndex = GridReference->GetTileIndexUnderCursor(0);
	// }

	FIntPoint NewTileIndex = GridReference->GetTileIndexUnderCursor(0);
	if (HoveredTile != NewTileIndex)
	{
		//first remove the state from the old hovered tile
		GridReference->RemoveStateFromTile(HoveredTile, Hovered);

		//now update the new hovered tile
		HoveredTile = NewTileIndex;
		GridReference->AddStateToTile(HoveredTile, Hovered);
	}
}


void UTileSelectionManager::DeselectTile(const FIntPoint& Index)
{
	GridReference->RemoveStateFromTile(Index, Selected);
	SetSelectedTile(FPATHFINDINGDATA_DEFAULT_INDEX);
}

void UTileSelectionManager::SelectTile(const FIntPoint& Index)
{
	GridReference->AddStateToTile(Index, Selected);
	SetSelectedTile(Index);
}

void UTileSelectionManager::IncrementSelectedTile(const FIntPoint& Index) const
{
	GridReference->IncrementTimesTileHasBeenSelected(Index);
}

void UTileSelectionManager::DeselectObject(ACombatGridObject* Object)
{
	if (Object) Object->SetIsSelected(false);
	SetSelectedObject(nullptr);
}

void UTileSelectionManager::SelectObject(ACombatGridObject* Object)
{
	if (Object) Object->SetIsSelected(true);
	SetSelectedObject(Object);
}

void UTileSelectionManager::TrySelectObjectOnSelectedTile(const FIntPoint& Index, const bool bForceUpdate)
{
	auto TileData = GridReference->GetGridTiles().Find(Index);
	if (!TileData) return;

	auto* NewUnit = TileData->GetSelectedItem();
	if (NewUnit != SelectedGridObject || bForceUpdate)
	{
		DeselectObject(SelectedGridObject);
		SelectObject(NewUnit);
	}
}

void UTileSelectionManager::TrySelectTile(const FIntPoint& Index, const bool bForceUpdate)
{
	bool bShouldNotifyChanged = false;
	if (Index == FIntPoint(FPATHFINDINGDATA_DEFAULT_INDEX))
	{
		DeselectTile(GetSelectedTile());
		bShouldNotifyChanged = true;
	}
	else if (GetSelectedTile() != Index || bForceUpdate)
	{
		DeselectTile(GetSelectedTile());
		SelectTile(Index);
		bShouldNotifyChanged = true;
	}
	else
	{
		IncrementSelectedTile(Index);
	}
	
	if (bShouldNotifyChanged) PlayerGridActions->NotifyTileSelectionChanged(GetSelectedTile());
	
	TrySelectObjectOnSelectedTile(GetSelectedTile(), bForceUpdate);
}

void UTileSelectionManager::OnGridGenerated()
{
	if (GridReference->IsIndexValid(SelectedTile))
	{
		TrySelectTile(SelectedTile, true);
	}
	else
	{
		TrySelectTile(FPATHFINDINGDATA_DEFAULT_INDEX, true);
	}
}

void UTileSelectionManager::OnTileDataUpdated(const FIntPoint& Index)
{
	if (Index == SelectedTile)
	{
		OnGridGenerated();
	}
}

//void UTileSelectionManager::OnUnitGridIndexChanged(ACombatGridUnit* Unit)
//{
//	if (Unit == SelectedUnit)
//	{
//		TrySelectTile(Unit->GetIndexOnGrid(), true);
//	}
//}

// ACombatGridObject* UTileSelectionManager::GetGridObjectUnderCursor() const
// {
// 	const auto* PC = UGameplayStatics::GetPlayerController(GridReference, 0);
// 	
// 	FHitResult OutHit;
// 	if (PC->GetHitResultUnderCursor(ECC_GameTraceChannel3, false, OutHit))
// 	{
// 		if (auto* HitActor = Cast<ACombatGridObject>(OutHit.GetActor()))
// 		{
// 			return HitActor;
// 		}
// 	}
//
// 	const auto Index = GridReference->GetTileIndexUnderCursor(0);
// 	if (auto* Tile = GridReference->GetGridTiles().Find(Index))
// 	{
// 		return Tile->GetSelectedItem();
// 	}
// 	
// 	return nullptr;
// }
