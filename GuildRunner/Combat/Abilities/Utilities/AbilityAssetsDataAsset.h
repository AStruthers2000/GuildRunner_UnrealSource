// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AbilityAssetsDataAsset.generated.h"

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities|Assets", meta = (AllowPrivateAccess = "true"))
	UTexture2D* Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities|Assets", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AAbilityVisualization> AbilityVisualizationClass;
};
