// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GuildRunner/Grid/CombatGridObject.h"
#include "GuildRunner/Player/GridActions/GridAction.h"
#include "GuildRunner/Units/EUnitType.h"
#include "Action_AddRemoveUnitFromGrid.generated.h"

class ACombatGridUnit;
class APlayerGridActions;

UCLASS()
class GUILDRUNNER_API AAction_AddRemoveUnitFromGrid : public AGridAction
{
	GENERATED_BODY()

public:
	AAction_AddRemoveUnitFromGrid();
	
	virtual void ExecuteGridAction(FIntPoint TileIndex) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bIsAddingUnit = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<EUnitType> UnitType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TMap<TEnumAsByte<EUnitType>, TSubclassOf<ACombatGridUnit>> UnitTypeMapping;

	

	/**
	 * @brief Tries to add unit to the clicked tile. Can fail if the tile isn't walkable or if there's a blocking unit
	 *	already on that tile.
	 * @param TileIndex Index of tile where unit addition will be attempted
	 */
	void AddUnitOnTile(const FIntPoint& TileIndex) const;

	/**
	 * @brief Tries to remove unit from the clicked tile. There should only be one unit per tile, but if that ever
	 *	changes, this function will need to be updated.
	 * @param TileIndex Index of tile to remove unit from. There should only be one unit per tile.
	 */
	void RemoveUnitOnTile(const FIntPoint& TileIndex) const;
	
	/**
	 * @brief Attempts to spawn unit on the clicked tile
	 * 
	 */
	ACombatGridUnit* SpawnUnit() const;
};

