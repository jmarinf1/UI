// 
// AmmoCounter.cpp
// 
// Implementation of the AmmoCounter class.
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#include "AmmoCounter.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Weapons/Umbrella/Umbrella.h"
#include "Components/TextBlock.h"

void UAmmoCounter::Show()
{
	if (const APlayerCharacter* MainCharacter = Cast<APlayerCharacter>(GetOwningPlayer()->GetCharacter()))
	{
		if (AUmbrella* Umbrella = MainCharacter->GetUmbrella())
		{
			Umbrella->OnCurrentBulletsChanged.BindUObject(this, &UAmmoCounter::UpdateCurrentAmmo);
			Umbrella->OnTotalBulletsChanged.BindUObject(this, &UAmmoCounter::UpdateTotalAmmo);
		}
	}
	
	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UAmmoCounter::Hide()
{
	if (const APlayerCharacter* MainCharacter = Cast<APlayerCharacter>(GetOwningPlayer()->GetCharacter()))
	{
		if (AUmbrella* Umbrella = MainCharacter->GetUmbrella())
		{
			Umbrella->OnCurrentBulletsChanged.Unbind();
			Umbrella->OnTotalBulletsChanged.Unbind();
		}
	}
	
	SetVisibility(ESlateVisibility::Hidden);
}

void UAmmoCounter::UpdateCurrentAmmo(const int NewCurrentAmmo) const 
{
	FNumberFormattingOptions Options;
	
	Options.MinimumIntegralDigits = 2; 
	
	CurrentAmmo->SetText(FText::AsNumber(NewCurrentAmmo, &Options));
}

void UAmmoCounter::UpdateTotalAmmo(const int NewTotalAmmo) const
{
	FNumberFormattingOptions Options;
	
	Options.MinimumIntegralDigits = 2;
	
	TotalAmmo->SetText(FText::AsNumber(NewTotalAmmo, &Options));
}

UTextBlock* UAmmoCounter::GetCurrentAmmo() const
{
	return CurrentAmmo;
}
UTextBlock* UAmmoCounter::GetTotalAmmo() const
{
	return TotalAmmo;
}

void UAmmoCounter::SetCurrentAmmo(UTextBlock* NewCurrentAmmo)
{
	CurrentAmmo = NewCurrentAmmo;
}

void UAmmoCounter::SetTotalAmmo(UTextBlock* NewTotalAmmo)
{
	TotalAmmo = NewTotalAmmo;
}
