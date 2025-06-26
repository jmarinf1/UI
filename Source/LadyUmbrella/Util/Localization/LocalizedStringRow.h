
#pragma once

#include "ActionCode.h"
#include "LocalizedStringRow.generated.h"

USTRUCT()
struct FLocalizedStringRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName = "Action Code")
	EActionCode ActionCode;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName = "English (United States)")
	FString EN_US;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName = "German")
	FString DE_DE;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName = "Spanish (Spain)")
	FString ES_ES;

	FLocalizedStringRow()
	{
		ActionCode = EActionCode::OTHER;

		EN_US.Empty();
		DE_DE.Empty();
		ES_ES.Empty();
	}
};