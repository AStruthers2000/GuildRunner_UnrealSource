#pragma once
#include "CoreMinimal.h"
#include "GuildRunner/Grid/Utilities/ETileType.h"
#include "FCombatGridUnitData_Stats.generated.h"


USTRUCT(BlueprintType)
struct FCombatGridUnitData_Stats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TEnumAsByte<ETileType>> ValidTileTypes = {Normal, DoubleCost, TripleCost};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanMoveDiagonally = false;
};

