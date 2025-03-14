// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GuildRunner/Grid/Utilities/ETileType.h"
#include "GuildRunner/Player/GridActions/Grid/Action_SelectTile.h"
#include "Action_SelectTileAndReachable.generated.h"

/**
 * 
 */
UCLASS()
class GUILDRUNNER_API AAction_SelectTileAndReachable : public AAction_SelectTile
{
	GENERATED_BODY()

public:
	virtual void ExecuteGridAction(FIntPoint TileIndex) override;
	
	UFUNCTION()
	void GenerateReachables();

	void Tick(float DeltaSeconds) override;

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

	UFUNCTION()
	void OnPathfindingCompleted(const TArray<FIntPoint>& Path);

	bool bAddingStateToTile = false;
	TArray<FIntPoint> TilesToUpdateState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float m_FrameDelayForDrawingReachables = 0.333f;

	TArray<TEnumAsByte<ETileType>> GetValidWalkingTiles() const;
};