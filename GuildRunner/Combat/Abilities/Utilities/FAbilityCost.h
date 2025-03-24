#pragma once
#include "CoreMinimal.h"
#include "FAbilityCost.generated.h"

USTRUCT(BlueprintType)
struct FAbilityCost
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Cost = 0;
};