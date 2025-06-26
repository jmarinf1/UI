#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FEnemiesEncounter.generated.h"


USTRUCT(BlueprintType)
struct FEnemiesEncounter
{
	GENERATED_BODY()
	
	int32 ButtonId;
	int32 LevelId;
	FName LevelName;
	int32 UniqueId;
	FName EncounterName;
	int32 CheckPointEncounterId;
	
};