// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EAbilityCastingRangeType.h"
#include "Engine/DataAsset.h"
#include "AbilityStatsDataAsset.generated.h"

class UAbilityEffectDataAsset;

/**
 * 
 */
UCLASS()
class GUILDRUNNER_API UAbilityStatsDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities|Stats", meta = (AllowPrivateAccess = "true"))
	EAbilityCastingRangeType AbilityCastingRangeType;

	// AbilityCastingRange is used for determining the min and max range an ability can be cast at
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities|Stats", meta = (AllowPrivateAccess = "true", EditCondition = "AbilityCastingRangeType != EAbilityCastingRangeType::Self"))
	FIntPoint AbilityCastingRange = {0, 0};

	// Stat modifiers that are part of the AbilityCost variable are SUBTRACTED from the instigator's stats. This means
	// all stat modifiers should be positive (a negative stat modifier in the AbilityCosts data asset will result in
	// the instigator gaining stats when activating this ability. This could be desired behavior, but in general not
	// recommended)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities|Stats", meta = (AllowPrivateAccess = "true"))
	UAbilityEffectDataAsset* AbilityCosts;

	// Stat modifiers that are part of the AbilityEffects variable are ADDED to the target's stats. This means if you
	// want a stat to decrease (aka the target takes damage, loses stamina, etc.) the stat modifier should be negative.
	// If you want a stat to increase (aka the target heals health, gains movement points, etc.) the stat modifier should
	// be positive. If your ability is accidentally healing the target when you expected the target to be taking damage,
	// check the sign of the modifiers.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities|Stats", meta = (AllowPrivateAccess = "true"))
	UAbilityEffectDataAsset* AbilityEffects;
};
