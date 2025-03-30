// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityRegistryDataAsset.h"

void UAbilityRegistryDataAsset::InitializeAbilityRegistry()
{
	UE_LOG(LogTemp, Display, TEXT("Constructing registry data asset"));

	AbilityNames.Empty();

	for (const auto& [name, _] : AbilitiesMap)
	{
		AbilityNames.Add(name);
		UE_LOG(LogTemp, Display, TEXT("%s has been registered"), *name.ToString());
	}
}

TArray<FGameplayTag> UAbilityRegistryDataAsset::GetAllAbilityNames() const
{
	auto Names = AbilityNames;
	
	if (AbilityNames.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("[UAbilityRegistryDataAsset::GetAllAbilityNames]: Ability names are empty"));
		
		TArray<FGameplayTag> Result;
		for (const auto& [name, _] : AbilitiesMap)
		{
			Result.Add(name);
		}
		
		Names = Result;
	}

	return Names;
}

TOptional<const UAbilityDataAsset*> UAbilityRegistryDataAsset::GetAbility(const FGameplayTag AbilityName) const
{
	if (GetAllAbilityNames().Contains(AbilityName))
	{
		return AbilitiesMap[AbilityName];
	}
	
	return NullOpt;
}

