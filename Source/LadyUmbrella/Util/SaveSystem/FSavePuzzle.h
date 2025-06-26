#pragma once

#include "CoreMinimal.h"
#include "FSaveStruct.h"
#include "FSavePuzzle.generated.h"

USTRUCT(BlueprintType, meta=(DisplayName="FSavePuzzle"))
struct FSavePuzzle : public FSaveStructData
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsSolved = false;
};
