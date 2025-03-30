// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityManager.h"

#include "AbilityRegistryDataAsset.h"

void UAbilityManager::BeginPlay()
{
	Super::BeginPlay();

	if (!AbilityRegistry)
	{
		UE_LOG(LogTemp, Error, TEXT("[UAbilityManager::GetAbilityDataFromAbilityName]: Ability Registry hasn't been set"));
		return;
	}

	AbilityRegistry->InitializeAbilityRegistry();
}

const UAbilityDataAsset* UAbilityManager::GetAbilityDataFromAbilityName(const FGameplayTag& AbilityName) const
{
	if (AbilityRegistry == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("[UAbilityManager::GetAbilityDataFromAbilityName]: Ability Registry hasn't been set"));
		return nullptr;
	}
	
	auto OptionalAbility = AbilityRegistry->GetAbility(AbilityName);

	if (OptionalAbility.IsSet())
	{
		return OptionalAbility.GetValue();
	}

	return nullptr;
}
