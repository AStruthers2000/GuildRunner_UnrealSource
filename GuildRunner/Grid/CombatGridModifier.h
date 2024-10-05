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
	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GuildRunner|Grid|Modifiers", meta = (AllowPrivateAccess = "true"))
	bool bUpdateMesh;
	
public:	
	// Sets default values for this actor's properties
	ACombatGridModifier();

	TEnumAsByte<ETileType> GetTileType() const { return TileType; }

private:
	void ConfigureMesh();
#if WITH_EDITOR
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent);
#endif
	

};
