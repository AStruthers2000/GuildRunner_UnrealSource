// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DebugTileText.generated.h"

class ACombatGrid;
class ATextRenderActor;

UCLASS()
class GUILDRUNNER_API ADebugTileText : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADebugTileText();

	UFUNCTION(BlueprintCallable)
	ATextRenderActor* GetTextActor(FIntPoint Index);

	UFUNCTION(BlueprintCallable)
	void DestroyTextActor(FIntPoint Index);

	UFUNCTION(BlueprintCallable)
	void ClearAllTextActors();

	UFUNCTION(BlueprintCallable)
	void UpdateTextOnTile(FIntPoint Index);

	UFUNCTION(BlueprintCallable)
	void SetShowTileIndices(bool bShowTileIndices);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetShowTileText() const { return bShowTileText; }

protected:
	virtual void BeginPlay() override;


private:
	TMap<FIntPoint, ATextRenderActor*> SpawnedTexts;
	UPROPERTY()
	ACombatGrid* GridRef = nullptr;

	bool bShowTileText = false;
};
