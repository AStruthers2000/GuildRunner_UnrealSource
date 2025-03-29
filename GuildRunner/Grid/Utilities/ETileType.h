#pragma once
#include "CoreMinimal.h"
#include "ETileType.generated.h"

UENUM(BlueprintType)
enum class ETileType : uint8
{
	NoTile,
	Normal,
	Obstacle,
	DoubleCost,
	TripleCost,
	FlyingUnitsOnly
};
