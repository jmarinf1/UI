// 
// NumericProgressBar.h
// 
// Progress bar with a numeric text value. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "LadyUmbrella/UI/GenericProgressBar.h"
#include "NumericProgressBar.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UNumericProgressBar : public UGenericProgressBar
{
	GENERATED_BODY()

public:

	virtual void UpdateProgressBar(const float Value) override;

	UTextBlock* GetNumericValue() const;
	void SetNumericValue(UTextBlock* NewNumericValue);

private:

	UPROPERTY(BlueprintReadWrite, Category = "ProgressBar", meta = (BindWidget, AllowPrivateAccess = "true"))
	UTextBlock* NumericValue;
	
};