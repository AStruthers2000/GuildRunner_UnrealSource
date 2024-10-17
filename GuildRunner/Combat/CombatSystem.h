// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GuildRunner/Grid/Utilities/FTileData.h"
#include "CombatSystem.generated.h"

class ACombatGridUnit;
class ACombatGrid;

UCLASS()
class GUILDRUNNER_API ACombatSystem : public AActor
{
	GENERATED_BODY()
	
public:	
	ACombatSystem();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void AddUnitInCombat(ACombatGridUnit* Unit, FIntPoint Index);
	
	UFUNCTION(BlueprintCallable)
	void RemoveUnitInCombat(ACombatGridUnit* Unit, bool bDestroyUnit = true);

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	ACombatGrid* ManagedGrid;

	void SetUnitIndexOnGrid(ACombatGridUnit* Unit, const FIntPoint& Index, const bool bForceUpdate = false);

	TArray<ACombatGridUnit*> UnitsInCombat;

	UFUNCTION()
	void OnGridGenerated();

	UFUNCTION()
	void OnTileDataUpdated(FIntPoint Index);

};
