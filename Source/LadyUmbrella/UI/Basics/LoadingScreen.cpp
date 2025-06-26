// Fill out your copyright notice in the Description page of Project Settings.


#include "LadyUmbrella/UI/Basics/LoadingScreen.h"
#include "Components/TextBlock.h"

void ULoadingScreen::UpdateProgressBar(const float Value)
{
	Super::UpdateProgressBar(Value);
	
	const int32 DisplayedPercent = FMath::RoundToInt(GetPercentageValue() * 100.0f);

	GetPercentageText()->SetText(FText::FromString(FString::Printf(TEXT("%d"), DisplayedPercent)));
}

UTextBlock* ULoadingScreen::GetPercentageText() const
{
	return TextPercentage;
}

void ULoadingScreen::SetPercentageText(UTextBlock* NewText)
{
	TextPercentage = NewText;
}