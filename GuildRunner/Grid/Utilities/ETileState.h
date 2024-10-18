#pragma once
#include "CoreMinimal.h"
#include "ETileState.generated.h"

UENUM(BlueprintType)
enum ETileState : uint8
{
	None,
	Hovered,
	Selected,
	IsNeighbor,
	IsInPath,
	IsDiscovered,
	IsAnalyzed,
	IsReachable
};
