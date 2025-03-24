// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EAbilityCastingRangeType.h"
#include "EAbilityEffectStatType.h"
#include "FAbilityCost.h"
#include "Engine/DataAsset.h"
#include "AbilityStatsDataAsset.generated.h"

class UAbilityEffectDataAsset;

USTRUCT(BlueprintType)
struct FAbilityCosts
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities|Stats|Costs", meta = (AllowPrivateAccess = "true"))
	TArray<FAbilityCost> Costs;
};
/**
 * 
 */
UCLASS()
class GUILDRUNNER_API UAbilityStatsDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities|Stats", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<EAbilityCastingRangeType> AbilityCastingRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities|Stats", meta = (AllowPrivateAccess = "true", EditCondition = "AbilityCastingRange != EAbilityCastingRangeType::Self"))
	int32 Range;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities|Stats", meta = (AllowPrivateAccess = "true"))
	TMap<EAbilityEffectStatType, FAbilityCosts> AbilityCosts; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities|Stats", meta = (AllowPrivateAccess = "true"))
	TArray<UAbilityEffectDataAsset*> AbilityEffects;
};
