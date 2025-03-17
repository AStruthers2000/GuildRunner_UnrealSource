// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GuildRunner/Grid/Utilities/FPathfindingData.h"
#include "TileSelectionManager.generated.h"


class APlayerGridActions;
class ACombatGrid;
class ACombatGridObject;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GUILDRUNNER_API UTileSelectionManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	friend APlayerGridActions;

	FIntPoint GetHoveredTile() const { return HoveredTile; }
	
	FIntPoint GetSelectedTile() const { return SelectedTile; }
	ACombatGridObject* GetSelectedGridObject() const { return SelectedGridObject; }

	/**
	 * @brief Tries to select the passed tile. This is responsible for deselecting the currently selected tile,
	 *	selecting the new tile, incrementing the selection counter if necessary, then notifying the
	 *	OnSelectedTileChanged callback that a new tile was selected
	 * @param Index Index of tile we are trying to select
	 * @param bForceUpdate Whether we want to force an update or not. Forcing an update selects the provided tile
	 *	regardless of if it was already selected or not
	 */
	void TrySelectTile(const FIntPoint& Index, const bool bForceUpdate = false);

	/**
	 * @brief Called once per tick, tracing from the mouse position to highlight the hovered tile
	 */
	void UpdateTileUnderCursor();
	
	/**
	 * @brief Tries to deselect the provided tile, if it is a valid tile index
	 * @param Index Index of tile to be deselected
	 */
	void DeselectTile(const FIntPoint& Index);
	/**
	 * @brief Tries to select the provided tile, if it is a valid tile index
	 * @param Index Index of tile to be selected
	 */
	void SelectTile(const FIntPoint& Index);
	
	/**
	 * @brief Increments the number of times this tile was selected, so that we can cyclically select any object on this
	 *	tile
	 * @param Index Index of tile to increment number of selected times
	 */
	void IncrementSelectedTile(const FIntPoint& Index) const;

	/**
	 * @brief Tries to deselect the provided object, if it isn't null
	 * @param Object Object to be deselected
	 */
	void DeselectObject(ACombatGridObject* Object);
	
	/**
	 * @brief Tries to select the provided object, if it isn't null
	 * @param Object Object to be selected
	 */
	void SelectObject(ACombatGridObject* Object);

	/**
	 * @brief Tries to select the next object on the provided tile. This is responsible for deselecting the currently
	 *	selected object and selecting the new object (if valid)
	 * @param Index Index of tile to try and select an object from
	 */
	void TrySelectObjectOnSelectedTile(const FIntPoint& Index);

private:
	UFUNCTION()
	void OnGridGenerated();

	UFUNCTION()
	void OnTileDataUpdated(const FIntPoint& Index);
	
	UFUNCTION()
	void OnTileSelectionChanged(const FIntPoint& TileIndex);
	
	void SetHoveredTile(const FIntPoint& NewIndex) { HoveredTile = NewIndex; }
	void SetSelectedTile(const FIntPoint& NewIndex) { SelectedTile = NewIndex; }
	void SetSelectedObject(ACombatGridObject* NewGridObject) { SelectedGridObject = NewGridObject; };

	void SetGridReference(ACombatGrid* Grid);
	void SetPlayerGridActionsReference(APlayerGridActions* PlayerActions);
	


	//UFUNCTION()
	//void OnUnitGridIndexChanged(ACombatGridUnit* Unit);

	bool bForcingUpdate = false;

	UPROPERTY()
	ACombatGrid* GridReference;

	UPROPERTY()
	APlayerGridActions* PlayerGridActions;
	
	FIntPoint HoveredTile = FPATHFINDINGDATA_DEFAULT_INDEX;
	FIntPoint SelectedTile = FPATHFINDINGDATA_DEFAULT_INDEX;

	UPROPERTY()
	ACombatGridObject* SelectedGridObject;
};


