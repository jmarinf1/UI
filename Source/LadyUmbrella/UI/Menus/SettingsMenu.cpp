// Fill out your copyright notice in the Description page of Project Settings.


#include "SettingsMenu.h"
#include "PauseMenu.h"
#include "LadyUmbrella/UI/Basics/AnimatedButton.h"
#include "LadyUmbrella/Util/Controllers/MainController.h"
#include "LadyUmbrella/Util/HUDs/MainHUD.h"
#include "SettingsOptions/ControlSettings.h"
#include "Components/HorizontalBox.h"

void USettingsMenu::NativeConstruct()
{
	Super::NativeConstruct();

	TArray<UAnimatedButton*> ExtractedButtons;
	for (UWidget* Child : MenuButtonsContainer->GetAllChildren())
	{
		if (UAnimatedButton* AnimatedButton = Cast<UAnimatedButton>(Child))
		{
			ExtractedButtons.Add(AnimatedButton);
		}
	}
	
	SetButtons(ExtractedButtons);
}

void USettingsMenu::Show()
{
	AddToViewport();
	SetNavigationIndex(0);
	GetButtons()[GetNavigationIndex()]->SetKeyboardFocus();
	SetVisibility(ESlateVisibility::Visible);
	ShowSubOptions(GetNavigationIndex());
}

void USettingsMenu::ShowSubOptions(const int Option) const
{
	if (APlayerController* PlayerController = GetOwningPlayer<APlayerController>())
	{
		if (const AMainController* MainController = Cast<AMainController>(PlayerController))
		{
			switch (Option)
			{
				case 0:
					MainController->HandleToggleControlSettingsMenu();
					break;
				case 1:
					MainController->GetMainHUD()->GetControlSettingsInstance()->SetVisibility(ESlateVisibility::Hidden);
					break;
				case 2:
					MainController->GetMainHUD()->GetControlSettingsInstance()->SetVisibility(ESlateVisibility::Hidden);
					break;
				case 3:
					MainController->GetMainHUD()->GetControlSettingsInstance()->SetVisibility(ESlateVisibility::Hidden);
					break;
				case 4:
					MainController->GetMainHUD()->GetControlSettingsInstance()->SetVisibility(ESlateVisibility::Hidden);
					break;
				case 5:
					MainController->GetMainHUD()->GetControlSettingsInstance()->SetVisibility(ESlateVisibility::Hidden);
					break;
				default:
					break;
			}
					
		}
	}
}

FReply USettingsMenu::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
	
	if (InKeyEvent.GetKey() == EKeys::Left || InKeyEvent.GetKey() == EKeys::Gamepad_LeftShoulder)
	{
		MenuNavigation(false, 0, false, true);
		ShowSubOptions(GetNavigationIndex());
	}
	if (InKeyEvent.GetKey() == EKeys::Right || InKeyEvent.GetKey() == EKeys::Gamepad_RightShoulder)
	{
		MenuNavigation(false, 0, true, false);
		ShowSubOptions(GetNavigationIndex());
	}
	if (InKeyEvent.GetKey() == EKeys::Escape || InKeyEvent.GetKey() == EKeys::Gamepad_FaceButton_Right)
	{
		if (APlayerController* PlayerController = GetOwningPlayer())
		{
			if (const AMainController* MainController = Cast<AMainController>(PlayerController))
			{
				MainController->GetMainHUD()->GetControlSettingsInstance()->SetVisibility(ESlateVisibility::Hidden);
				SetVisibility(ESlateVisibility::Hidden);
				MainController->GetMainHUD()->GetPauseMenuInstance()->SetVisibility(ESlateVisibility::Visible);
				const int32 CurrentPauseMenuIndex = MainController->GetMainHUD()->GetPauseMenuInstance()->GetNavigationIndex();
				MainController->GetMainHUD()->GetPauseMenuInstance()->GetButtons()[CurrentPauseMenuIndex]->SetKeyboardFocus();
			}
		}
	}
	return FReply::Handled();
}