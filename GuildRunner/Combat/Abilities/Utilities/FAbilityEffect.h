#pragma once
#include "CoreMinimal.h"
#include "FAbilityEffect.generated.h"

USTRUCT(BlueprintType)
struct FAbilityEffect
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities|Stats|Effects", meta = (AllowPrivateAccess = "true"))
	int32 StatModifier = 0;
};