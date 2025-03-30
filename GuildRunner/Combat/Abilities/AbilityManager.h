// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "AbilityManager.generated.h"

class UAbilityDataAsset;
class UAbilityRegistryDataAsset;
class ACombatSystem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GUILDRUNNER_API UAbilityManager : public UActorComponent
{
	GENERATED_BODY()

public:
	friend ACombatSystem;

	virtual void BeginPlay() override;

	/**
	 * @brief Attempts to get information on the ability with the provided name from the global ability registry.
	 * @param AbilityName Internal name of ability that instigator is attempting to find
	 * @return const pointer to an ability data asset if there is a valid ability with the provided name. Else, nullptr.
	 */
	const UAbilityDataAsset* GetAbilityDataFromAbilityName(const FGameplayTag& AbilityName) const;

private:
	UPROPERTY(EditAnywhere)
	UAbilityRegistryDataAsset* AbilityRegistry;
};

