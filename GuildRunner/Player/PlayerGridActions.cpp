// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerGridActions.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GuildRunner/Grid/CombatGrid.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APlayerGridActions::APlayerGridActions()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GridReference = nullptr;
}

void APlayerGridActions::BeginPlay()
{
	Super::BeginPlay();
	GridReference = Cast<ACombatGrid>(UGameplayStatics::GetActorOfClass(this, ACombatGrid::StaticClass()));
	if(!GridReference)
	{
		UE_LOG(LogTemp, Error, TEXT("[APlayerGridActions::BeginPlay]:\tNo grid found for player actions"));
	}

	if(const auto PC = Cast<APlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
	{
		const auto EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PC->InputComponent);
		const auto Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
		if(Subsystem && EnhancedInputComponent)
		{
			Subsystem->AddMappingContext(CombatGridActionsMappingContext, 0);
			EnhancedInputComponent->BindAction(CombatGridSelectTileAction, ETriggerEvent::Triggered, this, &APlayerGridActions::SelectTile);
			EnhancedInputComponent->BindAction(CombatGridDeselectTileAction, ETriggerEvent::Triggered, this, &APlayerGridActions::DeselectTile);
		}
	}
}

void APlayerGridActions::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	UpdateTileUnderCursor();
}

void APlayerGridActions::UpdateTileUnderCursor()
{
	if(!GridReference) return;
	if(HoveredTile != GridReference->GetTileIndexUnderCursor(0))
	{
		//first remove the state from the old hovered tile
		GridReference->RemoveStateFromTile(HoveredTile, Hovered);

		//now update the new hovered tile
		HoveredTile = GridReference->GetTileIndexUnderCursor(0);
		GridReference->AddStateToTile(HoveredTile, Hovered);
	}
}

void APlayerGridActions::SelectTile(const FInputActionValue& Value)
{
	if(!GridReference) return;
	GridReference->AddStateToTile(HoveredTile, Selected);
}

void APlayerGridActions::DeselectTile(const FInputActionValue& Value)
{
	if(!GridReference) return;
	GridReference->RemoveStateFromTile(HoveredTile, Selected);
}


