// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GuildRunner/Grid/Utilities/ETileType.h"
#include "GuildRunner/Player/GridActions/GridAction.h"
#include "Action_FindPathToTarget.generated.h"

/**
 * 
 */
UCLASS()
class GUILDRUNNER_API AAction_FindPathToTarget : public AGridAction
{
	GENERATED_BODY()

public:
	AAction_FindPathToTarget();
	virtual void ExecuteGridAction(FIntPoint TileIndex) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void OnPathfindingCompleted(TArray<FIntPoint> Path);

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bIncludeDiagonals;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bFlyingUnit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 MaxPathLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float DelayBetweenIterations = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float MaxMsPerFrame = 1.f;

	TArray<TEnumAsByte<ETileType>> GetValidWalkingTiles() const;

	UFUNCTION()
	void OnSelectedTileChanged(FIntPoint Index);

	TArray<FIntPoint> ReachableTiles;
};
