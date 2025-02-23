// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GuildRunner/Player/GridActions/GridAction.h"
#include "Action_DeselectTile.generated.h"

class APlayerGridActions;

UCLASS()
class GUILDRUNNER_API AAction_DeselectTile : public AGridAction
{
	GENERATED_BODY()

public:
	AAction_DeselectTile();
	virtual void ExecuteGridAction(FIntPoint TileIndex) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
