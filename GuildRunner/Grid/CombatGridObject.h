#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Utilities/FPathfindingData.h"
#include "CombatGridObject.generated.h"

class ACombatGrid;

UCLASS()
class GUILDRUNNER_API ACombatGridObject : public AActor
{
	GENERATED_BODY()

public:
	void SetGrid(ACombatGrid* Grid) { GridReference = Grid; }

	UFUNCTION(BlueprintCallable)
	FIntPoint GetIndexOnGrid() const { return IndexOnGrid; }
	
	UFUNCTION(BlueprintCallable)
	void SetIndexOnGrid(const FIntPoint Index) { IndexOnGrid = Index; }

	bool GetIsBlockingObject() const { return bBlockingObject; }
	
protected:
	UFUNCTION(BlueprintCallable)
	const ACombatGrid* GetGrid() const { return GridReference; }

	void SetBlockingObject(bool BlockingObject) { bBlockingObject = BlockingObject; }

private:
	UPROPERTY()
	ACombatGrid* GridReference;

	UPROPERTY(VisibleAnywhere)
	FIntPoint IndexOnGrid = FPATHFINDINGDATA_DEFAULT_INDEX;

	UPROPERTY(VisibleAnywhere)
	bool bBlockingObject = false;
};