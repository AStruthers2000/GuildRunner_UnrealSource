// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AbilityRegistryDataAsset.generated.h"

class UAbilityDataAsset;
/**
 * 
 */
UCLASS()
class GUILDRUNNER_API UAbilityRegistryDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
	TMap<FName, UAbilityDataAsset*> AbilitiesMap;
};
