//  
// Menu.cpp
// 
// Implementation of the Menu class.
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#include "Menu.h"
#include "LadyUmbrella/UI/Basics/AnimatedButton.h"

void UMenu::MenuNavigation(const bool bIsAnalog, const float AxisY, const bool bMoveForward, const bool bMoveBackward)
{
	int32 NewIndex = GetNavigationIndex();
	if (bIsAnalog) // Joystick
	{
		JoyStickNavigation(AxisY, NewIndex);
	}
	else // Keys
	{
		KeyNavigation(bMoveForward, bMoveBackward, NewIndex);
	}
	if (NewIndex != GetNavigationIndex())
	{
		SetNavigationIndex(NewIndex);
		GetButtons()[GetNavigationIndex()]->SetKeyboardFocus();
	}
	if (FMath::Abs(AxisY) < 0.1f)
	{
		SetCanNavigate(true);
	}
}

void UMenu::JoyStickNavigation(const float AxisY, int32& NewIndex)
{
	if (FMath::Abs(AxisY) > GetJoyStickDeadZone() && GetCanNavigate())
	{
		SetCanNavigate(false);
		if (AxisY < 0) 
		{
			NewIndex = (GetNavigationIndex() + 1) % GetButtons().Num();
		}
		else if (AxisY > 0)
		{
			NewIndex = (GetNavigationIndex() - 1 + GetButtons().Num()) % GetButtons().Num();
		}
	}
}

void UMenu::KeyNavigation(const bool bMoveForward, const bool bMoveBackward, int32& NewIndex)
{
	if (bMoveForward)
	{
		NewIndex = (GetNavigationIndex() + 1) % GetButtons().Num();
	}
	if (bMoveBackward)
	{
		NewIndex = (GetNavigationIndex() - 1 + GetButtons().Num()) % GetButtons().Num();
	}
}

void UMenu::NativeConstruct()
{
	Super::NativeConstruct();
	
	SetJoyStickDeadZone(0.5f);
	SetCanNavigate(true);

}

FReply UMenu::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseMove(InGeometry, InMouseEvent);
	
	for (int32 i = 0; i < GetButtons().Num(); ++i)
	{
		UAnimatedButton* Button = GetButtons()[i];
		if (!Button || !Button->IsVisible())
			continue;

		FGeometry ButtonGeometry = Button->GetCachedGeometry();

		if (ButtonGeometry.IsUnderLocation(InMouseEvent.GetScreenSpacePosition()))
		{
			// Update index
			if (GetNavigationIndex() != i)
			{
				SetNavigationIndex(i);
			}

			Button->SetKeyboardFocus();
			break; // stop after finding one match
		}
	}

	return FReply::Handled();
}