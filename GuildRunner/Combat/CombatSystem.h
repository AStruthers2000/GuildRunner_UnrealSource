// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GuildRunner/Grid/Utilities/FTileData.h"
#include "CombatSystem.generated.h"

class ACombatGridUnit;
class ACombatGrid;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCombatSystemGridUnitIndexChanged, ACombatGridUnit*, Unit);

UCLASS()
class GUILDRUNNER_API ACombatSystem : public AActor
{
	GENERATED_BODY()

public:
	/******************************************************************
	 * Grid Callbacks
	 ******************************************************************/
	UPROPERTY(BlueprintAssignable, Category = "Test")
	FCombatSystemGridUnitIndexChanged OnGridUnitIndexChanged;

	ACombatSystem();

	virtual void BeginPlay() override;

	/**
	 * @brief Registers an object in combat. This registration will include adding this object to the objects on the
	 *	provided tile.
	 * @param Object The object to be registered in combat
	 * @param Index The index the object will spawn on/be registered to initially
	 */
	void AddObjectIntoCombat(ACombatGridObject* Object, const FIntPoint& Index);

	/**
	 * @brief Removes this object from combat and destroys this object
	 * @param Object Object to be destroyed
	 * @param bDestroyUnit Optional variable to determine whether we want to call the object's destructor. Default to true
	 */
	void RemoveObjectFromCombat(ACombatGridObject* Object, const TOptional<bool>& bDestroyUnit = true);

	/**
	 * @brief Tries to remove unit from the provided tile
	 * @param Index Index of tile to attempt to remove unit from
	 * @param bDestroyUnit Optional variable to determine whether we want to call the unit's destructor. Default to true
	 */
	void RemoveUnitFromTile(const FIntPoint& Index, const TOptional<bool>& bDestroyUnit = true);

private:
	/**
	 * @brief Handles registration of the unit when the unit moves from tile it's currently on to the newly provided
	 *	index. Also updates the unit's location when a valid move is being performed.
	 * @param Object Unit being updated
	 * @param Index New tile for unit to go to
	 * @param bForceUpdate Optional parameter to force an update. Units only update if the index passed is different
	 *	then the tile they're already on. Most of the time, this is fine, but when the grid or an individual tile
	 *	regenerates, the index of the tile might still be valid, but it could have moved. For example, lets say there's
	 *	a unit on index (0, 0). If the grid changes size, that index will still be valid, but will most likely have
	 *	changed positions in the world. In this case, the unit thinks it's still standing on index (0, 0), and the grid
	 *	still thinks that this unit is standing on (0, 0), so the tile index hasn't changed. However, there's now a
	 *	desync between the location of the unit and the location of the tile, so we want to force an update.
	 */
	void SetUnitIndexOnGrid(ACombatGridObject* Object, const FIntPoint& Index, const TOptional<bool>& bForceUpdate = false) const;

	/**
	 * @brief Moves the provided unit to the new tile.
	 * @param Unit Unit that will be moved
	 * @param Index Index of the unit's new tile
	 */
	void UpdateUnitLocation(ACombatGridObject* Unit, const FIntPoint& Index) const;

	/**
	 * @brief Helper function for updating the unit properly when a new grid is generated or the tile is updated
	 * @param Unit Unit to be updated according to newly generated grid or updated tile
	 * @param bForceUpdate Whether we want to force an update on this unit. Defaults to false
	 */
	void UpdateUnitOnTileGeneration(ACombatGridObject* const& Unit, const TOptional<bool>& bForceUpdate = false);

	/**
	 * @brief Updates all objects that this system is managing. Possibly destructive operation for ObjectsInCombat
	 */
	UFUNCTION()
	void OnGridGenerated();

	/**
	 * @brief Updates all objects on provided tile. Possibly destructive operation for ObjectsInCombat
	 * @param Index Index of tile that was updated by the grid
	 */
	UFUNCTION()
	void OnTileDataUpdated(const FIntPoint& Index);

	/**
	 * @brief Called once at the beginning of unit movement, right after OnPathfindingComplete, that will pass the unit
	 *	and this unit's target tile for immediate registration purposes
	 * @param Unit Unit that is moving to target tile
	 * @param Index Index of target tile
	 */
	UFUNCTION()
	void OnUnitStartedMovingToTargetTile(ACombatGridUnit* Unit, const FIntPoint& Index);

	/**
	 * @brief Called as soon as the unit starts moving to the new tile. This handles the registration of the unit on
	 *	the appropriate tile. For units to stay synchronized and not end up stepping on the same tile, the registration
	 *	has to happen as soon as the unit starts moving, not when the unit finishes moving
	 * @param Unit Unit to instantly register on new tile
	 * @param Index Index of new tile to register unit on
	 */
	UFUNCTION()
	void OnUnitStartedMovingToNewTile(ACombatGridUnit* Unit, const FIntPoint& Index);

	/**
	 * @brief Called when the unit has fully reached this tile. Can handle events that happen when unit (visually)
	 *	arrives at this tile
	 * @param Unit Unit that reached tile
	 * @param Index Index of tile unit reached
	 */
	UFUNCTION()
	void OnUnitReachedNewTile(ACombatGridUnit* Unit, const FIntPoint& Index);

	/**
	 * @brief Called once at the end of unit movement
	 * @param Unit Unit that finished pathfinding
	 */
	UFUNCTION()
	void OnUnitFinishedPathfinding(ACombatGridUnit* Unit);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	ACombatGrid* ManagedGrid;
	
	UPROPERTY()
	TArray<ACombatGridObject*> ObjectsInCombat;
};


