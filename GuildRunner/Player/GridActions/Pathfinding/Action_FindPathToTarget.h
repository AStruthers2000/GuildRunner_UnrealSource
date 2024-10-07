// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GuildRunner/Player/GridActions/GridAction.h"
#include "Action_FindPathToTarget.generated.h"

/**
 * 
 */
UCLASS()
class GUILDRUNNER_API AAction_FindPathToTarget : public AGridAction
{
	GENERATED_BODY()

public:
	AAction_FindPathToTarget();
	virtual void ExecuteGridAction(FIntPoint TileIndex) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bIncludeDiagonals;
};
