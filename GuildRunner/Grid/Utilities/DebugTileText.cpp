// Fill out your copyright notice in the Description page of Project Settings.


#include "DebugTileText.h"

#include "Components/TextRenderComponent.h"
#include "Engine/TextRenderActor.h"
#include "GuildRunner/Grid/CombatGrid.h"
#include "GuildRunner/Grid/CombatGridPathfinding.h"
#include "GuildRunner/Grid/GridShapes/GridShapeUtilities.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ADebugTileText::ADebugTileText()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ADebugTileText::BeginPlay()
{
	Super::BeginPlay();

	GridRef = Cast<ACombatGrid>(UGameplayStatics::GetActorOfClass(this, ACombatGrid::StaticClass()));
	if(!GridRef)
	{
		UE_LOG(LogTemp, Error, TEXT("[ADebugTileText::BeginPlay]:\tFailed to find grid with GetActorOfClass, no grid found in level"));
		return;
	}

	
	//bind to grid events
	GridRef->OnTileDataUpdated.AddDynamic(this, &ADebugTileText::UpdateTextOnTile);
	GridRef->OnCombatGridDestroyed.AddDynamic(this, &ADebugTileText::ClearAllTextActors);
	
	//bind to pathfinding events
	//GridRef->GetGridPathfinding()->OnPathfindingDataUpdated.AddDynamic(this, &ADebugTileText::UpdateTextOnTile);
	GridRef->GetGridPathfinding()->OnPathfindingDataUpdated.AddDynamic(this, &ADebugTileText::ReUpdateAllTextAfterDelay);
	GridRef->GetGridPathfinding()->OnPathfindingDataCleared.AddDynamic(this, &ADebugTileText::UpdateTextOnAllTiles);
}


ATextRenderActor* ADebugTileText::GetTextActor(FIntPoint Index)
{
	ATextRenderActor* Actor = SpawnedTexts.FindRef(Index);
	//if actor already exists
	if(Actor)
	{
		return Actor;
	}

	//create default text actor
	Actor = Cast<ATextRenderActor>(GetWorld()->SpawnActor(ATextRenderActor::StaticClass()));
	SpawnedTexts.Add({Index, Actor});
	Actor->SetActorTickEnabled(false);
	Actor->SetActorEnableCollision(false);
	Actor->GetTextRender()->SetHorizontalAlignment(EHTA_Center);
	Actor->GetTextRender()->SetVerticalAlignment(EVRTA_TextCenter);
	return Actor;
}

void ADebugTileText::DestroyTextActor(const FIntPoint Index)
{
	if(ATextRenderActor* Actor = SpawnedTexts.FindRef(Index))
	{
		Actor->Destroy();
		SpawnedTexts.Remove(Index);
	}
}

void ADebugTileText::ClearAllTextActors()
{
	for(const auto Pair : SpawnedTexts)
	{
		Pair.Value->Destroy();
	}
	SpawnedTexts.Empty();
}

void ADebugTileText::UpdateTextOnTile(const FIntPoint Index)
{
	//if we don't have a grid reference or if we don't want to show tile text
	if(!GridRef || !WantToDisplayAnyText()) return;
	
	const auto* TileData = GridRef->GetGridTiles().Find(Index);
	
	//if the tile doesn't exist or if the tile isn't walkable
	if(!TileData || !UGridShapeUtilities::IsTileTypeWalkable(TileData->Type))
	{
		DestroyTextActor(Index);
	}
	else
	{
		TArray<FString> Text;
		if(bShowTileIndices)
		{
			Text.Add( FString::Printf(TEXT("%d, %d"), Index.X, Index.Y));
		}
		
		if(bShowCostToEnterTile || bShowMinCostToTarget || bShowCostFromStart || bShowSortOrder)
		{
			const auto Tile = GridRef->GetGridPathfinding()->GetPathfindingData().Find(Index);
			if(!Tile) return;
			const FPathfindingData PathfindingData = *Tile;
			if(bShowCostToEnterTile)
			{
				Text.Add(FString::Printf(TEXT("Enter cost: %d"), PathfindingData.CostToEnterTile));
			}
			if(bShowMinCostToTarget)
			{
				if(PathfindingData.MinimumCostToTarget != FPATHFINDINGDATA_DEFAULT_ROUTING_COST)
				{
					Text.Add(FString::Printf(TEXT("Min cost: %d"), PathfindingData.MinimumCostToTarget));
				}
			}
			if(bShowCostFromStart)
			{
				if(PathfindingData.CostFromStart != FPATHFINDINGDATA_DEFAULT_ROUTING_COST)
				{
					Text.Add(FString::Printf(TEXT("Start cost: %d"), PathfindingData.CostFromStart));
				}
			}
			if(bShowSortOrder)
			{
				const auto DiscoveredIndex = GridRef->GetGridPathfinding()->GetDiscoveredTileIndices().Find(Index);
				if(DiscoveredIndex != INDEX_NONE)
				{
					const int32 SortingCost = GridRef->GetGridPathfinding()->GetDiscoveredTileSortingCosts()[DiscoveredIndex];
					Text.Add(FString::Printf(TEXT("Sort order: %d (%d)"), DiscoveredIndex, SortingCost));
				}
			}
		}

		if(Text.Num() == 0)
		{
			return;
		}
		
		auto* TextActor = GetTextActor(Index);
		TextActor->GetTextRender()->SetText(FText::FromString(FString::Join(Text, TEXT("\n"))));

		FTransform TextTransform;
		TextTransform.SetLocation(TileData->Transform.GetLocation() + FVector(0, 0, 5.f));
		TextTransform.SetRotation(FRotator(90, 180, 0).Quaternion());
		TextTransform.SetScale3D(FVector(1));
		TextActor->SetActorTransform(TextTransform);
	}
}

void ADebugTileText::SetShowTileTexts(const bool bTileIndices, const bool bCostToEnterTile, const bool bMinCostToTarget, const bool bCostFromStart, const bool bSortOrder)
{
	bShowTileIndices = bTileIndices;
	bShowCostToEnterTile = bCostToEnterTile;
	bShowMinCostToTarget = bMinCostToTarget;
	bShowCostFromStart = bCostFromStart;
	bShowSortOrder = bSortOrder;

	UpdateTextOnAllTiles();
}

void ADebugTileText::UpdateTextOnAllTiles()
{
	if(WantToDisplayAnyText())
	{
		if(!GridRef) return;

		TArray<FIntPoint> Keys;
		GridRef->GetGridTiles().GetKeys(Keys);
		for(const auto& Key : Keys)
		{
			UpdateTextOnTile(Key);
		}
	}
	else
	{
		ClearAllTextActors();
	}
}

bool ADebugTileText::WantToDisplayAnyText() const
{
	return bShowTileIndices || bShowCostToEnterTile || bShowMinCostToTarget || bShowCostFromStart || bShowSortOrder;
}

void ADebugTileText::ReUpdateAllTextAfterDelay(FIntPoint Index)
{
	FLatentActionInfo ActionInfo;
	ActionInfo.CallbackTarget = this;
	ActionInfo.ExecutionFunction = "UpdateAllTextAfterDelay";
	UKismetSystemLibrary::RetriggerableDelay(this, 0.1f, ActionInfo);
}

void ADebugTileText::UpdateAllTextAfterDelay()
{
	UE_LOG(LogTemp, Display, TEXT("Event triggered after delay"));
	UpdateTextOnAllTiles();
}





