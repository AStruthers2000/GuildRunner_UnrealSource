// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridShapes/FGridShapeData.h"
#include "Utilities/ETileType.h"
#include "CombatGridModifier.generated.h"


UCLASS()
class GUILDRUNNER_API ACombatGridModifier : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GuildRunner|Grid|Modifiers", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMeshComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GuildRunner|Grid|Modifiers", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<EGridShape> Shape = Square;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GuildRunner|Grid|Modifiers", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<ETileType> TileType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GuildRunner|Grid|Modifiers|Colors", meta = (AllowPrivateAccess = "true"))
	FLinearColor NormalObstacleColor = FLinearColor(1, 1, 1, 1);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GuildRunner|Grid|Modifiers|Colors", meta = (AllowPrivateAccess = "true"))
	FLinearColor BlockingObstacleColor = FLinearColor(1, 0, 0, 1);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GuildRunner|Grid|Modifiers|Colors", meta = (AllowPrivateAccess = "true"))
	FLinearColor DoubleObstacleColor;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GuildRunner|Grid|Modifiers|Colors", meta = (AllowPrivateAccess = "true"))
	FLinearColor TripleObstacleColor;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GuildRunner|Grid|Modifiers|Colors", meta = (AllowPrivateAccess = "true"))
	FLinearColor FlyingOnlyObstacleColor = FLinearColor(0, 1, 1, 1);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GuildRunner|Grid|Modifiers", meta = (AllowPrivateAccess = "true"))
	bool bUseTileHeight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GuildRunner|Grid|Modifiers", meta = (AllowPrivateAccess = "true"))
	bool bUpdateMesh;
	
public:	
	// Sets default values for this actor's properties
	ACombatGridModifier();

	bool GetUseTileHeight() const { return bUseTileHeight; }
	TEnumAsByte<ETileType> GetTileType() const { return TileType; }

private:
	void ConfigureMesh();
#if WITH_EDITOR
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent);
#endif
	

};
