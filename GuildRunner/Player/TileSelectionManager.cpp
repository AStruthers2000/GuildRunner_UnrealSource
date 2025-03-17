// Fill out your copyright notice in the Description page of Project Settings.


#include "TileSelectionManager.h"

#include "PlayerGridActions.h"
#include "GuildRunner/Grid/CombatGrid.h"

void UTileSelectionManager::SetGridReference(ACombatGrid* Grid)
{
	GridReference = Grid;
	GridReference->OnCombatGridGenerated.AddDynamic(this, &UTileSelectionManager::OnGridGenerated);
	GridReference->OnTileDataUpdated.AddDynamic(this, &UTileSelectionManager::OnTileDataUpdated);
}

void UTileSelectionManager::SetPlayerGridActionsReference(APlayerGridActions* PlayerActions)
{
	PlayerGridActions = PlayerActions;
	PlayerGridActions->OnSelectedTileChanged.AddDynamic(this, &UTileSelectionManager::OnTileSelectionChanged);
}


void UTileSelectionManager::UpdateTileUnderCursor()
{
	if (!GridReference)
	{
		return;
	}

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

void UTileSelectionManager::TrySelectTile(const FIntPoint& Index, const bool bForceUpdate)
{
	if (Index == FIntPoint(FPATHFINDINGDATA_DEFAULT_INDEX))
	{
		DeselectTile(GetSelectedTile());
	}
	else if (GetSelectedTile() != Index || bForceUpdate)
	{
		DeselectTile(GetSelectedTile());
		SelectTile(Index);
	}
	else
	{
		IncrementSelectedTile(Index);
	}

	bForcingUpdate = bForceUpdate;
	PlayerGridActions->NotifyTileSelectionChanged(GetSelectedTile());
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

void UTileSelectionManager::TrySelectObjectOnSelectedTile(const FIntPoint& Index)
{
	if (!GridReference) return;
	
	auto TileData = GridReference->GetGridTiles().Find(Index);
	if (!TileData)
	{
		DeselectObject(SelectedGridObject);
		return;
	}

	auto* NewUnit = TileData->GetSelectedItem();
	if (NewUnit != SelectedGridObject || bForcingUpdate)
	{
		DeselectObject(SelectedGridObject);
		SelectObject(NewUnit);
	}

	bForcingUpdate = false;
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

void UTileSelectionManager::OnTileSelectionChanged(const FIntPoint& TileIndex)
{
	TrySelectObjectOnSelectedTile(TileIndex);
}

//void UTileSelectionManager::OnUnitGridIndexChanged(ACombatGridUnit* Unit)
//{
//	if (Unit == SelectedUnit)
//	{
//		TrySelectTile(Unit->GetIndexOnGrid(), true);
//	}
//}

