#pragma once
#include "CoreMinimal.h"
#include "EUnitAnimationState.generated.h"

UENUM(BlueprintType)
enum EUnitAnimationState
{
	Idle,
	Walk,
	Attack,
	Hit,
	Death,
	Respawn
};
