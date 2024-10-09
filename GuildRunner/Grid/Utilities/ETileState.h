#pragma once
#include "CoreMinimal.h"
#include "ETileState.generated.h"

UENUM(BlueprintType)
enum ETileState
{
	None,
	Hovered,
	Selected,
	IsNeighbor,
	IsInPath,
	IsDiscovered,
	IsAnalyzed
};
