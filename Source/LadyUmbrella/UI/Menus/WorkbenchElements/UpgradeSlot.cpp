// Fill out your copyright notice in the Description page of Project Settings.

#include "UpgradeSlot.h"

#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/ProgressBar.h"
#include "Components/Spacer.h"
#include "Components/TextBlock.h"
#include "LadyUmbrella/Util/Logger.h"
#include "LadyUmbrella/Weapons/Umbrella/UpgradeComponent/UpgradeComponent.h"


void UUpgradeSlot::NativeConstruct()
{
	Super::NativeConstruct();
	UpgradeButton->OnPressed.AddDynamic(this, &UUpgradeSlot::ProgressBarForward);
	UpgradeButton->OnReleased.AddDynamic(this, &UUpgradeSlot::ResetProgressBar);
	
}

void UUpgradeSlot::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	if (bIsHolding)
	{
		HoldTime += InDeltaTime;
		const float Progress = FMath::Clamp(HoldTime / MaxHoldTime, 0.0f, 1.0f);
		if (UpgradeInProcessProgressBar)
		{
			UpgradeInProcessProgressBar->SetPercent(Progress);
		}

		if (Progress >= 1.0f)
		{
			ProgressEndedDelegate.ExecuteIfBound(UpgradeId, CurrentLevel);
			bIsHolding = false;
		}
	}
}

void UUpgradeSlot::SetupUpgradeSlots(const TPair<EUpgradeType, FUpgrade>& UpgradeToShow)
{
	// Create the progress bars to show the current level of the upgrade.
	TextUpgradeName->SetText(UpgradeToShow.Value.GetName());

	// Show the text of the upgrade
	const FText DescriptionOfTheUpgrade = UpgradeToShow.Value.GetSummaryText();
	UTextBlock* DescriptionOfTheUpgradeTextBlock = NewObject<UTextBlock>(this);
	DescriptionOfTheUpgradeTextBlock->SetText(DescriptionOfTheUpgrade);
	FSlateFontInfo FontInfo = DescriptionOfTheUpgradeTextBlock->GetFont();
	FontInfo.Size = 10; 
	DescriptionOfTheUpgradeTextBlock->SetFont(FontInfo);
	HB_Content->AddChild(DescriptionOfTheUpgradeTextBlock);

	// Show the CurrentState of the Upgrade (ProgressBars)
	for (uint8 i = 0; i < UpgradeToShow.Value.GetMaxLevel(); ++i)
	{
		UProgressBar* ProgressBar = NewObject<UProgressBar>(this);
		ProgressBar->SetPercent(UpgradeToShow.Value.GetLevel());
		HB_Content->AddChild(ProgressBar);
		UpgradeLevels.Add(ProgressBar);
	}

	// Show the price of the upgrade
	const FString PriceOfTheUpgrade = FString::FromInt(UpgradeToShow.Value.GetPrice());
	UTextBlock* PriceOfTheUpgradeTextBlock = NewObject<UTextBlock>(this);
	PriceOfTheUpgradeTextBlock->SetText(FText::FromString(PriceOfTheUpgrade));
	HB_Content->AddChild(PriceOfTheUpgradeTextBlock);
/*
	USpacer* Spacer = NewObject<USpacer>(this);
	Spacer->SetSize({50, 50});
	HB_Content->AddChild(Spacer);
*/
}

void UUpgradeSlot::ProgressBarForward()
{	
	bIsHolding = true;
	HoldTime = 0.0f;
	if (UpgradeInProcessProgressBar)
	{
		UpgradeInProcessProgressBar->SetPercent(0.0f);
	}
}

void UUpgradeSlot::ResetProgressBar()
{
	bIsHolding = false;
	HoldTime = 0.0f;
	if (UpgradeInProcessProgressBar)
	{
		UpgradeInProcessProgressBar->SetPercent(0.0f);
	}
}

void UUpgradeSlot::SetPercentToBar(const int32 UpgradeIdentifier)
{
	// Remove. Should be checked from outside
	if (UpgradeIdentifier < UpgradeLevels.Num())
	{
		if (IsValid(UpgradeLevels[UpgradeIdentifier]))
		{
			UpgradeLevels[UpgradeIdentifier]->SetPercent(100.0f);
			CurrentLevel++;
		}	
	}
}