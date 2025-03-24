// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EAbilityEffectDuration.h"
#include "Engine/DataAsset.h"
#include "AbilityEffectDataAsset.generated.h"

enum class EAbilityEffectStatType : uint8;
struct FAbilityEffect;

USTRUCT(BlueprintType)
struct FAbilityEffectArray
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities|Stats|Effects", meta = (AllowPrivateAccess = "true"))
	TArray<FAbilityEffect> Effects;
};

USTRUCT(BlueprintType)
struct FAbilityEffectMap
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities|Stats|Effects", meta = (AllowPrivateAccess = "true"))
	TMap<EAbilityEffectStatType, FAbilityEffectArray> Effects;
};

/**
 * 
 */
UCLASS()
class GUILDRUNNER_API UAbilityEffectDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities|Stats|Effects", meta = (AllowPrivateAccess = "true"))
	TMap<EAbilityEffectDuration, FAbilityEffectMap> AbilityEffects;
};
