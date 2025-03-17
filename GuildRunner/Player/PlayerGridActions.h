// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GuildRunner/Grid/Utilities/FPathfindingData.h"
#include "PlayerGridActions.generated.h"

class UTileSelectionManager;
class ACombatGridObject;
class ACombatGridUnit;
class ACombatSystem;
class AGridAction;
struct FInputActionValue;
class UInputMappingContext;
class UInputAction;
class ACombatGrid;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPlayerGridActionDelegate, AGridAction*, LeftClickAction, AGridAction*,
                                             RightClickAction);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerGridActionTileDelegate, const FIntPoint&, Index);

UCLASS()
class GUILDRUNNER_API APlayerGridActions : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GuildRunner|Input", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* CombatGridActionsMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GuildRunner|Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* CombatGridLeftClickAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GuildRunner|Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* CombatGridRightClickAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GuildRunner|Input", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AGridAction> PlayerAction_LeftClickOnTile;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GuildRunner|Input", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AGridAction> PlayerAction_RightClickOnTile;

public:
	/******************************************************************
	 * Action Callbacks
	 ******************************************************************/
	UPROPERTY(BlueprintAssignable, Category = "Test")
	FPlayerGridActionDelegate OnSelectedActionsChanged;

	UPROPERTY(BlueprintAssignable, Category = "Test")
	FPlayerGridActionTileDelegate OnSelectedTileChanged;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
public:
	/******************************************************************
	 * Helpful getters and setters
	 ******************************************************************/
	APlayerGridActions();
	
	ACombatGrid* GetCombatGridReference() const { return GridReference; }
	ACombatSystem* GetCombatSystemReference() const { return CombatSystemReference; }

	UFUNCTION(BlueprintCallable)
	void SetSelectedActions(TSubclassOf<AGridAction> SelectedAction, TSubclassOf<AGridAction> DeselectedAction);

	void SelectTile(const FIntPoint& Index) const;
	FIntPoint GetSelectedTile() const;

	void NotifyTileSelectionChanged(const FIntPoint& Index) const;

private:
	UPROPERTY()
	ACombatGrid* GridReference;
	
	UPROPERTY()
	ACombatSystem* CombatSystemReference;

	UPROPERTY()
	UTileSelectionManager* TileSelectionManager;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	AGridAction* LeftClickAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	AGridAction* RightClickAction;
	
	void ExecuteLeftClickActionOnHoveredTile(const FInputActionValue& Value);
	void ExecuteRightClickActionOnHoveredTile(const FInputActionValue& Value);
	
	AGridAction* TrySpawnGridAction(AGridAction*& ActionObject, TSubclassOf<AGridAction> ActionClass);
};
