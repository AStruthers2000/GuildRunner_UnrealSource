#pragma once

#include "CoreMinimal.h"
#include "GuildRunner/Player/GridActions/GridAction.h"
#include "Action_IncreaseDecreaseTileHeight.generated.h"

/**
 * 
 */
UCLASS()
class GUILDRUNNER_API AAction_IncreaseDecreaseTileHeight : public AGridAction
{
	GENERATED_BODY()

public:
	AAction_IncreaseDecreaseTileHeight();
	virtual void ExecuteGridAction(FIntPoint TileIndex) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bIncreaseHeight;
};
