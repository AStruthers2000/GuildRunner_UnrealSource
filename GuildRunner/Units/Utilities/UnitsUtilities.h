// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GuildRunner/Units/EUnitType.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UnitsUtilities.generated.h"

class UDataTable;
struct FCombatGridUnitData;
/**
 * 
 */
UCLASS()
class GUILDRUNNER_API UUnitsUtilities : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	//UFUNCTION(BlueprintCallable)
	static const FCombatGridUnitData* GetDefaultUnitDataFromUnitType(EUnitType UnitType);

	UFUNCTION(BlueprintCallable)
	static FCombatGridUnitData GetDefaultUnitDataFromUnitType_BP(TEnumAsByte<EUnitType> UnitType);

private:
	static const UDataTable* FindGridDataMappingTable();
};
