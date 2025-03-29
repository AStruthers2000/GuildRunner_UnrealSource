#pragma once
#include "CoreMinimal.h"
#include "EAbilityEffectDuration.generated.h"

UENUM(BlueprintType)
enum class EAbilityEffectDuration : uint8
{
	Instant		UMETA(DisplayName = "Instant"),
	PerTurn		UMETA(DisplayName = "Per Turn"),
	Permanent	UMETA(DisplayName = "Permanent"),
};