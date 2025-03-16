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
	 * @brief Registers a unit in combat. This registration will include adding this unit to the objects on the provided
	 *	tile.
	 * @param Unit The unit to be registered in combat
	 * @param Index The index the unit will spawn on/be registered to initially
	 */
	UFUNCTION(BlueprintCallable)
	void AddUnitInCombat(ACombatGridUnit* Unit, FIntPoint Index);

	/**
	 * @brief Removes this unit from combat and destroys this unit
	 * @param Unit Unit to be destroyed
	 * @param bDestroyUnit Optional variable to determine whether we want to call the unit's destructor. Default to true
	 */
	//UFUNCTION(BlueprintCallable)
	void RemoveUnitInCombat(ACombatGridUnit* Unit, const TOptional<bool>& bDestroyUnit = true);

	/**
	 * @brief Tries to remove unit from the provided tile
	 * @param Index Index of tile to attempt to remove unit from
	 * @param bDestroyUnit Optional variable to determine whether we want to call the unit's destructor. Default to true
	 */
	void RemoveUnitFromTile(const FIntPoint& Index, const TOptional<bool>& bDestroyUnit = true);

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	ACombatGrid* ManagedGrid;

	void SetUnitIndexOnGrid(ACombatGridUnit* Unit, const FIntPoint& Index, const bool bForceUpdate = false);
	
	/**
	 * @brief Registers the unit with the grid, which will start the process of associating this unit with the specific
	 *	tile it spawned on.
	 * @param Unit Unit that will be placed onto the grid
	 * @param Index Index of the tile that the unit will specifically be added to
	 * @param bForceUpdate Force an update of the unit's position (probably need to remove this)
	 */
	void PlaceUnitOnGrid(ACombatGridUnit* Unit, const FIntPoint& Index, const bool bForceUpdate = false);

	TArray<ACombatGridUnit*> UnitsInCombat;

	UFUNCTION()
	void OnGridGenerated();

	UFUNCTION()
	void OnTileDataUpdated(FIntPoint Index);

	UFUNCTION()
	void OnUnitReachedNewTile(ACombatGridUnit* Unit, FIntPoint Index);
};
