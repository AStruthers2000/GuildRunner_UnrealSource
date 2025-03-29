#pragma once
#include "CoreMinimal.h"
#include "EAbilityCastingRangeType.generated.h"

UENUM(BlueprintType)
enum class EAbilityCastingRangeType : uint8
{
	Self		UMETA(DisplayName = "Self Only"),
	Melee		UMETA(DisplayName = "Melee Distance"),
	Ranged		UMETA(DisplayName = "Ranged Distance"),
};