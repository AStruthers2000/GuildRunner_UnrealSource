// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GuildRunner/Grid/Utilities/FPathfindingData.h"
#include "TileSelectionManager.generated.h"


class APlayerGridActions;
class ACombatGrid;
class ACombatGridObject;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GUILDRUNNER_API UTileSelectionManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTileSelectionManager();
	friend APlayerGridActions;

	FIntPoint GetHoveredTile() const { return HoveredTile; }
	ACombatGridObject* GetHoveredGridObject() const { return HoveredGridObject; }
	
	FIntPoint GetSelectedTile() const { return SelectedTile; }
	ACombatGridObject* GetSelectedGridObject() const { return SelectedGridObject; }
	
	void TrySelectTile(const FIntPoint& Index, const bool bForceUpdate = false);

	void UpdateTileUnderCursor();
	void DeselectTile(const FIntPoint& Index);
	void SelectTile(const FIntPoint& Index);
	void IncrementSelectedTile(const FIntPoint& Index) const;
	void DeselectObject(ACombatGridObject* Object);
	void SelectObject(ACombatGridObject* Object);
	void TrySelectObjectOnSelectedTile(const FIntPoint& Index, bool bForceUpdate);

private:
	UFUNCTION()
	void OnGridGenerated();

	UFUNCTION()
	void OnTileDataUpdated(const FIntPoint& Index);
	
	void SetHoveredTile(const FIntPoint& NewIndex) { HoveredTile = NewIndex; }
	void SetSelectedTile(const FIntPoint& NewIndex) { SelectedTile = NewIndex; }
	void SetSelectedObject(ACombatGridObject* NewGridObject) { SelectedGridObject = NewGridObject; };

	void SetGridReference(ACombatGrid* Grid) { GridReference = Grid; }
	void SetPlayerGridActionsReference(APlayerGridActions* PlayerActions) { PlayerGridActions = PlayerActions; }

	// ACombatGridObject* GetGridObjectUnderCursor() const;

	//UFUNCTION()
	//void OnUnitGridIndexChanged(ACombatGridUnit* Unit);

	UPROPERTY()
	ACombatGrid* GridReference;

	UPROPERTY()
	APlayerGridActions* PlayerGridActions;
	
	FIntPoint HoveredTile = FPATHFINDINGDATA_DEFAULT_INDEX;
	FIntPoint SelectedTile = FPATHFINDINGDATA_DEFAULT_INDEX;

	UPROPERTY()
	ACombatGridObject* HoveredGridObject;
	UPROPERTY()
	ACombatGridObject* SelectedGridObject;
};
