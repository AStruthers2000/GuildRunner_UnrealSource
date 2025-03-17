// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GuildRunner/Grid/Utilities/ETileType.h"
#include "GuildRunner/Player/GridActions/GridAction.h"
#include "Action_MoveUnitOnGrid.generated.h"

class ACombatGridUnit;
/**
 * 
 */
UCLASS()
class GUILDRUNNER_API AAction_MoveUnitOnGrid : public AGridAction
{
	GENERATED_BODY()

public:
	AAction_MoveUnitOnGrid();
	virtual void ExecuteGridAction(FIntPoint TileIndex) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void OnPathfindingCompleted(const TArray<FIntPoint>& Path);

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float MoveDurationPerTile;

	UPROPERTY()
	ACombatGridUnit* CurrentUnit;

	UFUNCTION()
	void OnUnitFinishedWalking(ACombatGridUnit* Unit);

	
};
