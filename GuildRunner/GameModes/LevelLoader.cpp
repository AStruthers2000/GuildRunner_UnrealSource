// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelLoader.h"

#include "Engine/LevelStreaming.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ALevelLoader::ALevelLoader()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALevelLoader::BeginPlay()
{
	Super::BeginPlay();

	LoadLevel(LoadedLevel, true);
}

// Called every frame
void ALevelLoader::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALevelLoader::LoadLevel(const FName LevelName, const bool bForceLoad)
{
	//if the currently loaded level is trying to be loaded again, return early
	if(!bForceLoad && LevelName == LoadedLevel) return;
	
	//if there is a currently loaded level
	if(auto* UnloadedLevel = UGameplayStatics::GetStreamingLevel(this, LoadedLevel))
	{
		UnloadedLevel->SetShouldBeVisible(false);
		UnloadedLevel->SetShouldBeLoaded(false);
	}

	//try to load level from given name
	if(auto* Level = UGameplayStatics::GetStreamingLevel(this, LevelName))
	{
		Level->SetShouldBeLoaded(true);
		Level->SetShouldBeVisible(true);
		LoadedLevel = LevelName;
	}
	else if(LevelName != FName("None"))
	{
		if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("[ALevelLoader::LoadLevel]:\tInvalid level name during loading: %s"), *LevelName.ToString()));
	}
}
