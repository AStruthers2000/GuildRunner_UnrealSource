#pragma once
#include "CoreMinimal.h"
#include "GuildRunner/Grid/Utilities/ETileType.h"
#include "FCombatGridUnitData_Stats.generated.h"


USTRUCT(BlueprintType)
struct FCombatGridUnitData_Stats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ETileType> ValidTileTypes = {ETileType::Normal, ETileType::DoubleCost, ETileType::TripleCost};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanMoveDiagonally = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MovementPoints = 0;
};
