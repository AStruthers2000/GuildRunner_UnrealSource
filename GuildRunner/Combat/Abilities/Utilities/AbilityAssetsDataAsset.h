// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AbilityAssetsDataAsset.generated.h"

class UImage;
class AAbilityVisualization;
/**
 * 
 */
UCLASS()
class GUILDRUNNER_API UAbilityAssetsDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities|Assets", meta = (AllowPrivateAccess = "true"))
	FName AbilityDisplayName;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities|Assets", meta = (AllowPrivateAccess = "true"))
	//UImage* AbilityIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities|Assets", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AAbilityVisualization> AbilityVisualizationClass;
};
