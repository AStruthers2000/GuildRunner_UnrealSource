// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GuildRunner/Player/GridActions/GridAction.h"
#include "Action_AddTile.generated.h"

/**
 * 
 */
UCLASS()
class GUILDRUNNER_API AAction_AddTile : public AGridAction
{
	GENERATED_BODY()

public:
	AAction_AddTile();
	virtual void ExecuteGridAction(FIntPoint TileIndex) override;
};
