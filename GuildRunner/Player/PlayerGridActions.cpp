// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerGridActions.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GridActions/GridAction.h"
#include "GuildRunner/Combat/CombatSystem.h"
#include "GuildRunner/Grid/CombatGrid.h"
#include "GuildRunner/Units/CombatGridUnit.h"
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
	if (!GridReference)
	{
		UE_LOG(LogTemp, Error, TEXT("[APlayerGridActions::BeginPlay]:\tNo grid found for player actions"));
	}

	CombatSystemReference = Cast<ACombatSystem>(UGameplayStatics::GetActorOfClass(this, ACombatSystem::StaticClass()));
	if (!CombatSystemReference)
	{
		UE_LOG(LogTemp, Error, TEXT("[APlayerGridActions::BeginPlay]:\tNo combat system found for player actions"));
	}

	if (const auto PC = Cast<APlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
	{
		const auto EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PC->InputComponent);
		const auto Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
		if (Subsystem && EnhancedInputComponent)
		{
			Subsystem->AddMappingContext(CombatGridActionsMappingContext, 0);
			EnhancedInputComponent->BindAction(CombatGridSelectTileAction, ETriggerEvent::Triggered, this,
			                                   &APlayerGridActions::SelectTile);
			EnhancedInputComponent->BindAction(CombatGridDeselectTileAction, ETriggerEvent::Triggered, this,
			                                   &APlayerGridActions::DeselectTile);
		}
	}

	GridReference->OnCombatGridGenerated.AddDynamic(this, &APlayerGridActions::OnGridGenerated);
	GridReference->OnTileDataUpdated.AddDynamic(this, &APlayerGridActions::OnTileDataUpdated);
	CombatSystemReference->OnGridUnitIndexChanged.AddDynamic(this, &APlayerGridActions::OnUnitGridIndexChanged);

	SetSelectedActions(PlayerAction_SelectTile, PlayerAction_SelectTile);
}

void APlayerGridActions::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	UpdateTileUnderCursor();
}

void APlayerGridActions::SetSelectedActions(TSubclassOf<AGridAction> SelectedAction,
                                            TSubclassOf<AGridAction> DeselectedAction)
{
	LeftClickAction = TrySpawnGridAction(LeftClickAction, SelectedAction);
	RightClickAction = TrySpawnGridAction(RightClickAction, DeselectedAction);
	OnSelectedActionsChanged.Broadcast(LeftClickAction, RightClickAction);
}

void APlayerGridActions::TrySelectTileAndUnit(const FIntPoint& Index, const bool bForceUpdate)
{
	//first, try and select the given tile
	//if the indices are different, select this tile
	if (GetSelectedTile() != Index || bForceUpdate)
	{
		GetCombatGridReference()->RemoveStateFromTile(GetSelectedTile(), Selected);
		SetSelectedTile(Index);
		GetCombatGridReference()->AddStateToTile(GetSelectedTile(), Selected);
	}
	else
	{
		GetCombatGridReference()->RemoveStateFromTile(GetSelectedTile(), Selected);
		SetSelectedTile(FPATHFINDINGDATA_DEFAULT_INDEX);
	}

	/*
	//deselect currently selected unit
	if (SelectedUnit)
	{
		SelectedUnit->SetIsSelected(false);
	}
	const auto* Tile = GridReference->GetGridTiles().Find(SelectedTile);
	if (Tile)
	{
		auto* NewUnit = Tile->UnitOnTile
			                ? Tile->UnitOnTile
			                : ((SelectedUnit && SelectedUnit->GetIndexOnGrid() == SelectedTile)
				                   ? SelectedUnit
				                   : nullptr);
		//if the unit is the same as selected, do nothing
		if (SelectedUnit != NewUnit || bForceUpdate)
		{
			//update the new unit and if it exists, set it to selected
			SelectedUnit = NewUnit;
			if (SelectedUnit)
			{
				SelectedUnit->SetIsSelected(true);
			}
		}
	}
	*/
}

void APlayerGridActions::UpdateTileUnderCursor()
{
	if (!GridReference)
	{
		return;
	}

	auto* PossiblyHoveredUnit = GetUnitUnderCursor();
	if (PossiblyHoveredUnit != HoveredUnit)
	{
		//set the old unit as no longer hovered
		if (HoveredUnit)
		{
			HoveredUnit->SetIsHovered(false);
		}

		//update unit reference and set current unit to hovered
		HoveredUnit = PossiblyHoveredUnit;
		if (HoveredUnit)
		{
			HoveredUnit->SetIsHovered(true);
		}
	}

	FIntPoint NewTileIndex;
	if (HoveredUnit)
	{
		NewTileIndex = HoveredUnit->GetIndexOnGrid();
	}
	else
	{
		NewTileIndex = GridReference->GetTileIndexUnderCursor(0);
	}

	if (HoveredTile != NewTileIndex)
	{
		//first remove the state from the old hovered tile
		GridReference->RemoveStateFromTile(HoveredTile, Hovered);

		//now update the new hovered tile
		HoveredTile = NewTileIndex;
		GridReference->AddStateToTile(HoveredTile, Hovered);
	}
}

void APlayerGridActions::SelectTile(const FInputActionValue& Value)
{
	//if(!GridReference) return;
	//GridReference->AddStateToTile(HoveredTile, Selected);

	UpdateTileUnderCursor();
	if (LeftClickAction)
	{
		LeftClickAction->ExecuteGridAction(HoveredTile);
	}
}

void APlayerGridActions::DeselectTile(const FInputActionValue& Value)
{
	//if(!GridReference) return;
	//GridReference->RemoveStateFromTile(HoveredTile, Selected);

	UpdateTileUnderCursor();
	if (RightClickAction)
	{
		RightClickAction->ExecuteGridAction(HoveredTile);
	}
}

ACombatGridUnit* APlayerGridActions::GetUnitUnderCursor()
{
	/*
	const auto* PC = UGameplayStatics::GetPlayerController(this, 0);
	FHitResult OutHit;
	if (PC->GetHitResultUnderCursor(ECC_GameTraceChannel3, false, OutHit))
	{
		if (auto* GridUnit = Cast<ACombatGridUnit>(OutHit.GetActor()))
		{
			return GridUnit;
		}
		return nullptr;
	}

	const auto Index = GridReference->GetTileIndexUnderCursor(0);
	if (const auto* Tile = GridReference->GetGridTiles().Find(Index))
	{
		return Tile->UnitOnTile;
	}
	return nullptr;
	*/
	return nullptr;
}

AGridAction* APlayerGridActions::TrySpawnGridAction(AGridAction*& ActionObject, TSubclassOf<AGridAction> ActionClass)
{
	//if we have a valid player action, we want to destroy it
	if (ActionObject)
	{
		//Destroy(ActionObject);
		ActionObject->EndPlay(EEndPlayReason::Destroyed);
		ActionObject->Destroy();
		ActionObject = nullptr;
	}
	if (ActionClass)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParameters.bDeferConstruction = true;

		if (auto* SpawnedAction = GetWorld()->SpawnActor<AGridAction>(ActionClass, FVector::ZeroVector,
		                                                              FRotator::ZeroRotator, SpawnParameters))
		{
			SpawnedAction->PlayerGridActions = this;
			SpawnedAction->DispatchBeginPlay();
			return SpawnedAction;
		}
	}
	return nullptr;
}

void APlayerGridActions::OnGridGenerated()
{
	if (GridReference->IsIndexValid(SelectedTile))
	{
		TrySelectTileAndUnit(SelectedTile, true);
	}
	else
	{
		TrySelectTileAndUnit(FPATHFINDINGDATA_DEFAULT_INDEX, true);
	}
}

void APlayerGridActions::OnTileDataUpdated(FIntPoint Index)
{
	if (Index == SelectedTile)
	{
		OnGridGenerated();
	}
}

void APlayerGridActions::OnUnitGridIndexChanged(ACombatGridUnit* Unit)
{
	if (Unit == SelectedUnit)
	{
		TrySelectTileAndUnit(Unit->GetIndexOnGrid(), true);
	}
}
