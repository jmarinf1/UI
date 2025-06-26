// 
// NumericProgressBar.cpp
// 
// Implementation of the NumericProgressBar class.
// 
// Copyright Zulo Interactive. All Rights Reserved.
//


#include "LadyUmbrella/UI/Basics/NumericProgressBar.h"
#include "Components/TextBlock.h"

void UNumericProgressBar::UpdateProgressBar(const float Value)
{
	Super::UpdateProgressBar(Value);
	
	if (Value <= 0)
	{
		GetNumericValue()->SetText(FText::FromString(FString::Printf(TEXT("%d"), 0)));
	}
	else
	{
		GetNumericValue()->SetText(FText::FromString(FString::Printf(TEXT("%d"), static_cast<int>(Value))));
	}
}

UTextBlock* UNumericProgressBar::GetNumericValue() const
{
	return NumericValue;
}

void UNumericProgressBar::SetNumericValue(UTextBlock* NewNumericValue)
{
	NumericValue = NewNumericValue;
}