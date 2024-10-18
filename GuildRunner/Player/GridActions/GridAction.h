// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridAction.generated.h"

class APlayerGridActions;

UCLASS()
class GUILDRUNNER_API AGridAction : public AActor
{
	GENERATED_BODY()

public:
	AGridAction();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void ExecuteGridAction(FIntPoint TileIndex);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", ExposeOnSpawn = "true"))
	APlayerGridActions* PlayerGridActions;
};
