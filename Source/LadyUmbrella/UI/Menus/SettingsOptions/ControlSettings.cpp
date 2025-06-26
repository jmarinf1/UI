// Fill out your copyright notice in the Description page of Project Settings.


#include "LadyUmbrella/UI/Menus/SettingsOptions/ControlSettings.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "LadyUmbrella/UI/Basics/AnimatedButton.h"
#include "LadyUmbrella/Util/SaveSystem/LU_GameInstance.h"
#include "LadyUmbrella/UI/Menus/SettingsMenu.h"
#include "LadyUmbrella/Util/Controllers/MainController.h"
#include "LadyUmbrella/Util/HUDs/MainHUD.h"
#include "LadyUmbrella/UI/Menus/PauseMenu.h"

void UControlSettings::NativeConstruct()
{
	Super::NativeConstruct();
	
	SetButtons({ControlType});
	
	UpdatePlatformName();
	ControlType->GetButton()->OnClicked.AddDynamic(this, &UControlSettings::UpdatePlatformName);
	
}

FReply UControlSettings::NativeOnAnalogValueChanged(const FGeometry& InGeometry, const FAnalogInputEvent& InAnalogEvent)
{
	Super::NativeOnAnalogValueChanged(InGeometry, InAnalogEvent);

	if (InAnalogEvent.GetKey() == EKeys::Gamepad_LeftY)
	{
		MenuNavigation(true, InAnalogEvent.GetAnalogValue(), false, false);
	}
	
	return FReply::Handled();
}

FReply UControlSettings::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
	
	if (InKeyEvent.GetKey() == EKeys::Up || InKeyEvent.GetKey() == EKeys::Gamepad_DPad_Up)
	{
		MenuNavigation(false, 0, false, true);
	}
	if (InKeyEvent.GetKey() == EKeys::Down || InKeyEvent.GetKey() == EKeys::Gamepad_DPad_Down)
	{
		MenuNavigation(false, 0, true, false);
	}
	if (InKeyEvent.GetKey() == EKeys::Left || InKeyEvent.GetKey() == EKeys::Gamepad_LeftShoulder)
	{
		UpdateSettingsOptions(false, true);
	}
	if (InKeyEvent.GetKey() == EKeys::Right || InKeyEvent.GetKey() == EKeys::Gamepad_RightShoulder)
	{
		UpdateSettingsOptions(true, false);
	}
	if (InKeyEvent.GetKey() == EKeys::Escape || InKeyEvent.GetKey() == EKeys::Gamepad_FaceButton_Right)
	{
		ExitSettingsMenu();	
	}
	if (InKeyEvent.GetKey() == EKeys::Enter || InKeyEvent.GetKey() == EKeys::Gamepad_FaceButton_Bottom)
	{
		GetButtons()[GetNavigationIndex()]->GetButton()->OnClicked.Broadcast();
	}

	return FReply::Handled();
}

void UControlSettings::UpdateSettingsOptions(const bool bMoveForward, const bool bMoveBackward) const
{
	if (APlayerController* PlayerController = GetOwningPlayer())
	{
		if (const AMainController* MainController = Cast<AMainController>(PlayerController))
		{
			MainController->GetMainHUD()->GetSettingsMenuInstance()->MenuNavigation(false, 0, bMoveForward, bMoveBackward);
			const int32 SettingsMenuIndex = MainController->GetMainHUD()->GetSettingsMenuInstance()->GetNavigationIndex();
			MainController->GetMainHUD()->GetSettingsMenuInstance()->ShowSubOptions(SettingsMenuIndex);
			
		}
	}
}

void UControlSettings::ExitSettingsMenu()
{
	if (APlayerController* PlayerController = GetOwningPlayer())
	{
		if (const AMainController* MainController = Cast<AMainController>(PlayerController))
		{
			SetVisibility(ESlateVisibility::Hidden);
			MainController->GetMainHUD()->GetSettingsMenuInstance()->SetVisibility(ESlateVisibility::Hidden);
			MainController->GetMainHUD()->GetPauseMenuInstance()->SetVisibility(ESlateVisibility::Visible);
			const int32 CurrentPauseMenuIndex = MainController->GetMainHUD()->GetPauseMenuInstance()->GetNavigationIndex();
			MainController->GetMainHUD()->GetPauseMenuInstance()->GetButtons()[CurrentPauseMenuIndex]->SetKeyboardFocus();
		}
	}
}

void UControlSettings::Show()
{
	AddToViewport();
	SetNavigationIndex(0);
	GetButtons()[GetNavigationIndex()]->SetKeyboardFocus();
	SetVisibility(ESlateVisibility::Visible);
}

void UControlSettings::UpdatePlatformName()
{
	if (ULU_GameInstance* GameInstance = Cast<ULU_GameInstance>(GetGameInstance()))
	{
		GameInstance->SetControlTypeCounter(GameInstance->GetControlTypeCounter() + 1);
		switch (GameInstance->GetControlTypeCounter())
		{
		case 0:
			PlatformName->SetText(FText::FromString("PC"));
			GameInstance->SetPlatformName("PC");
			break;
		case 1:
			PlatformName->SetText(FText::FromString("PLAYSTATION"));
			GameInstance->SetPlatformName("PLAYSTATION");
			break;
		case 2:
			PlatformName->SetText(FText::FromString("XBOX"));
			GameInstance->SetPlatformName("XBOX");
			break;
		case 3:
			GameInstance->SetControlTypeCounter(0);
			PlatformName->SetText(FText::FromString("PC"));
			GameInstance->SetPlatformName("PC");
			break;
		default:
			break;
		}
	}
}