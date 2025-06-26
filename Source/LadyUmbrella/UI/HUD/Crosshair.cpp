// 
// Crosshair.cpp
// 
// Implementation of the Crosshair class.
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 


#include "Crosshair.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Weapons/Umbrella/Umbrella.h"

void UCrosshair::Show()
{
	if (APlayerCharacter* MainCharacter = Cast<APlayerCharacter>(GetOwningPlayer()->GetCharacter()))
	{
		if (IsValid(MainCharacter) && IsValid(MainCharacter->GetUmbrella()))
		{
			MainCharacter->GetUmbrella()->OnUmbrellaStateChanged.BindUObject(this, &UCrosshair::UpdateCrosshairState);
		}
	}
}

void UCrosshair::Hide()
{
	if (APlayerCharacter* MainCharacter = Cast<APlayerCharacter>(GetOwningPlayer()->GetCharacter()))
	{
		if (IsValid(MainCharacter) && IsValid(MainCharacter->GetUmbrella()))
		{
			MainCharacter->GetUmbrella()->OnUmbrellaStateChanged.Unbind();
		}
	}
	
	SetVisibility(ESlateVisibility::Hidden);
}

void UCrosshair::UpdateCrosshairState(const bool IsAiming, const bool IsShieldOpen, const bool IsOnEnemy)
{
	if (IsAiming && !IsShieldOpen)
	{
		SetVisibility(ESlateVisibility::Visible);

		if (IsOnEnemy)
		{
			ChangeColorRed();
		}
		else
		{
			ResetColor();
		}
	}
	else
	{
		SetVisibility(ESlateVisibility::Hidden);
		ResetColor();
	}
}


