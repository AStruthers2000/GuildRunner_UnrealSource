// Fill out your copyright notice in the Description page of Project Settings.


#include "DebugTileText.h"

#include "Components/TextRenderComponent.h"
#include "Engine/TextRenderActor.h"
#include "GuildRunner/Grid/CombatGrid.h"
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
	if(!GridRef || !bShowTileText) return;
	
	const auto* TileData = GridRef->GetGridTiles().Find(Index);
	
	//if the tile doesn't exist or if the tile isn't walkable
	if(!TileData || !UGridShapeUtilities::IsTileTypeWalkable(TileData->Type))
	{
		DestroyTextActor(Index);
	}
	else
	{
		auto* TextActor = GetTextActor(Index);
		const FString Text = FString::Printf(TEXT("%d, %d"), Index.X, Index.Y);
		TextActor->GetTextRender()->SetText(FText::FromString(Text));

		FTransform TextTransform;
		TextTransform.SetLocation(TileData->Transform.GetLocation() + FVector(0, 0, 5.f));
		TextTransform.SetRotation(FRotator(90, 180, 0).Quaternion());
		TextTransform.SetScale3D(FVector(2));
		TextActor->SetActorTransform(TextTransform);
	}
}

void ADebugTileText::SetShowTileIndices(const bool bShowTileIndices)
{
	bShowTileText = bShowTileIndices;
	if(bShowTileIndices)
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







