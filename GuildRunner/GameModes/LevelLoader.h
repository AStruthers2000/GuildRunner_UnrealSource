// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelLoader.generated.h"

UCLASS()
class GUILDRUNNER_API ALevelLoader : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelLoader();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void LoadLevel(FName LevelName, bool bForceLoad = false);

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GuildRunner|Level Loading", meta = (AllowPrivateAccess = "true"))
	FName LoadedLevel = "";
};

