// Fill out your copyright notice in the Description page of Project Settings.


#include "GuildRunnerPlayerController.h"

void AGuildRunnerPlayerController::BeginPlay()
{
	Super::BeginPlay();

	//show mouse cursor in game always
	bShowMouseCursor = true;
}
