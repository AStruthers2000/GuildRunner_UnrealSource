#pragma once
#include "CoreMinimal.h"
#include "EAbilityEffectStatType.generated.h"

UENUM(BlueprintType)
enum class EAbilityEffectStatType : uint8
{
	NoStatType	UMETA(DisplayName = "No Stat Type"),
	Health		UMETA(DisplayName = "Health"),
	Mana		UMETA(DisplayName = "Mana"),
	Stamina		UMETA(DisplayName = "Stamina"),
	Movement	UMETA(DisplayName = "Movement Points"),
};