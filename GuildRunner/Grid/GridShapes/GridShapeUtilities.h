// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/DataTable.h"
#include "FGridShapeData.h"
#include "GuildRunner/Grid/Utilities/ETileType.h"
#include "GridShapeUtilities.generated.h"

class UDataTable;
/**
 * 
 */
UCLASS()
class GUILDRUNNER_API UGridShapeUtilities : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	//UFUNCTION(BlueprintCallable)
	static const FGridShapeData* GetShapeData(EGridShape Shape);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool IsTileTypeWalkable(const ETileType TileType);

private:
	static const UDataTable* FindGridDataMappingTable();
	//static UDataTable* GridDataMappingTable;
};
