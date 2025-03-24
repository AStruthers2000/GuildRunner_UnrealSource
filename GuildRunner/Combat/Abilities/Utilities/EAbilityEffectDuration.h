#pragma once
#include "CoreMinimal.h"
#include "EAbilityEffectDuration.generated.h"

UENUM(BlueprintType)
enum class EAbilityEffectDuration : uint8
{
	Instant,
	PerTurn,
	Permanent
};