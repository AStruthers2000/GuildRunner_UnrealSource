#pragma once
#include "CoreMinimal.h"
#include "EAbilityEffectStatType.generated.h"

UENUM(BlueprintType)
enum class EAbilityEffectStatType : uint8
{
	Health,
	Mana,
	Movement
};