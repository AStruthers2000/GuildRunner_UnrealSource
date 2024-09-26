// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GuildRunnerPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class GUILDRUNNER_API AGuildRunnerPlayerController : public APlayerController
{
	GENERATED_BODY()

	virtual void BeginPlay() override;
	
};
