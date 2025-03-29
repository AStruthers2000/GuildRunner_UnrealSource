// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EAbilityEffectStatType.h"
#include "FAbilityStatModifier.h"
#include "Engine/DataAsset.h"
#include "AbilityEffectDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class GUILDRUNNER_API UAbilityEffectDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

	// Stat modifiers that are part of the AbilityCost variable are SUBTRACTED from the instigator's stats. Therefore,
	// costs should in general be POSITIVE.
	//
	// Stat modifiers that are part of the AbilityEffects variable are ADDED to the target's stats.
	// * If you want a stat to decrease (aka the target takes damage, loses stamina, etc.) the stat modifier should be
	// NEGATIVE.
	// * If you want a stat to increase (aka the target heals health, gains movement points, etc.) the stat modifier
	// should be POSITIVE.
	//
	// The Instant duration effect is applied instantly.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities|Stats|Effects", meta = (AllowPrivateAccess = "true"))
	TMap<EAbilityEffectStatType, FAbilityStatModifier> InstantEffects;

	// Stat modifiers that are part of the AbilityCost variable are SUBTRACTED from the instigator's stats. Therefore,
	// costs should in general be POSITIVE.
	//
	// Stat modifiers that are part of the AbilityEffects variable are ADDED to the target's stats.
	// * If you want a stat to decrease (aka the target takes damage, loses stamina, etc.) the stat modifier should be
	// NEGATIVE.
	// * If you want a stat to increase (aka the target heals health, gains movement points, etc.) the stat modifier
	// should be POSITIVE.
	//
	// The Per Turn duration effect is applied once at the beginning of the receiver's (either instigator if this is a
	// cost or target if this is an effect) turn, for a number of turns equal to the EffectTurnCount variable in the
	// FAbilityStatModifier
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities|Stats|Effects", meta = (AllowPrivateAccess = "true"))
	TMap<EAbilityEffectStatType, FAbilityStatModifier> PerTurnEffects;

	// Stat modifiers that are part of the AbilityCost variable are SUBTRACTED from the instigator's stats. Therefore,
	// costs should in general be POSITIVE.
	//
	// Stat modifiers that are part of the AbilityEffects variable are ADDED to the target's stats.
	// * If you want a stat to decrease (aka the target takes damage, loses stamina, etc.) the stat modifier should be
	// NEGATIVE.
	// * If you want a stat to increase (aka the target heals health, gains movement points, etc.) the stat modifier
	// should be POSITIVE.
	//
	// The Permanent duration effect is applied instantly to the receiver (either instigator if this is a cost or target
	// if this is an effect) and will permanently alter the base stat until the end of this combat. This doesn't
	// permanently alter the hero's general base stats.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities|Stats|Effects", meta = (AllowPrivateAccess = "true"))
	TMap<EAbilityEffectStatType, FAbilityStatModifier> PermanentEffects;
};
