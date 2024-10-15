// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GuildRunner/Player/GridActions/GridAction.h"
#include "GuildRunner/Units/EUnitType.h"
#include "Action_AddRemoveUnitFromGrid.generated.h"

class APlayerGridActions;

UCLASS()
class GUILDRUNNER_API AAction_AddRemoveUnitFromGrid : public AGridAction
{
	GENERATED_BODY()
	
public:	
	AAction_AddRemoveUnitFromGrid();
	virtual void ExecuteGridAction(FIntPoint TileIndex) override;
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bIsAddingUnit = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<EUnitType> UnitType;

	void RemoveUnitOnTile(const FIntPoint& TileIndex) const;
};
