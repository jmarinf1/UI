#pragma once

#include "CoreMinimal.h"
#include "FChaptersProgress.generated.h"

USTRUCT(BlueprintType)
struct FChaptersProgress
{
	GENERATED_BODY()
public:

	bool bIsCompleted = false;
	
	bool bLastChapterUnlocked = false;

	bool bLastChapterPlayed = false;

};
