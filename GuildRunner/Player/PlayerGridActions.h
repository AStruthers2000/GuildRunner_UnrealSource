// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerGridActions.generated.h"

struct FInputActionValue;
class UInputMappingContext;
class UInputAction;
class ACombatGrid;

UCLASS()
class GUILDRUNNER_API APlayerGridActions : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GuildRunner|Input", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* CombatGridActionsMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GuildRunner|Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* CombatGridSelectTileAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GuildRunner|Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* CombatGridDeselectTileAction;
	
public:	
	// Sets default values for this actor's properties
	APlayerGridActions();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

private:
	UPROPERTY()
	ACombatGrid* GridReference;
	FIntPoint HoveredTile = {};

	void UpdateTileUnderCursor();

	void SelectTile(const FInputActionValue& Value);
	void DeselectTile(const FInputActionValue& Value);
};
