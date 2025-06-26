//  
// PauseMenu.cpp
// 
// Implementation of the PauseMenu class.
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#include "PauseMenu.h"
#include "SettingsMenu.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "LadyUmbrella/Util/Controllers/MainController.h"
#include "LadyUmbrella/UI/Basics/AnimatedButton.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "LadyUmbrella/Util/Logger.h"
#include "LadyUmbrella/Util/SaveSystem/LU_GameInstance.h"
#include "Components/VerticalBox.h"

void UPauseMenu::Show()
{
	APlayerController* PlayerController = GetOwningPlayer<APlayerController>();
	PlayerController->SetInputMode(FInputModeUIOnly());
	PlayerController->bShowMouseCursor = true;
	PlayerController->Pause();
	AddToViewport();
	SetNavigationIndex(0);
	GetButtons()[GetNavigationIndex()]->SetKeyboardFocus();
	SetVisibility(ESlateVisibility::Visible);
}

void UPauseMenu::Resume()
{
	if (APlayerController* PlayerController = GetOwningPlayer<APlayerController>())
	{
		PlayerController->SetInputMode(FInputModeGameOnly());
		PlayerController->bShowMouseCursor = false;
		PlayerController->SetPause(false);
		SetVisibility(ESlateVisibility::Hidden);
		if (const AMainController* MainController = Cast<AMainController>(PlayerController))
		{
			MainController->HandleTogglePlayerHUD();
		}
	}
}

void UPauseMenu::Save()
{
	FLogger::WarningLog("Save button clicked");
	ULU_GameInstance* GameInstance = Cast<ULU_GameInstance>( GetGameInstance() );

	if (IsValid(GameInstance))
	{
		GameInstance->SaveGame(GameInstance->GetCurrentSaveSlot());
	}
}

void UPauseMenu::Load()
{
	FLogger::WarningLog("Load button clicked");
}

void UPauseMenu::Settings()
{
	if (APlayerController* PlayerController = GetOwningPlayer<APlayerController>())
	{
		if (const AMainController* MainController = Cast<AMainController>(PlayerController))
		{
			MainController->HandleToggleSettingsMenu();
		}
	}
}

void UPauseMenu::ExitMainMenu()
{
	if (const AMainController* MainController = Cast<AMainController>(GetOwningPlayer()))
	{
		MainController->HandleClearWidgets();
	}
	APlayerController* PlayerController = GetOwningPlayer<APlayerController>();
	PlayerController->SetInputMode(FInputModeUIOnly());
	PlayerController->bShowMouseCursor = true;
	UGameplayStatics::OpenLevel(this, TEXT("L_MainMenu"));
}

void UPauseMenu::ExitDesktop()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, true);
}

void UPauseMenu::NativeConstruct()
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
	
	GetResumeButton()->GetButton()->OnClicked.Clear();
	GetResumeButton()->GetButton()->OnClicked.AddDynamic(this, &UPauseMenu::Resume);

	GetSaveButton()->GetButton()->OnClicked.Clear();
	GetSaveButton()->GetButton()->OnClicked.AddDynamic(this, &UPauseMenu::Save);

	GetLoadButton()->GetButton()->OnClicked.Clear();
	GetLoadButton()->GetButton()->OnClicked.AddDynamic(this, &UPauseMenu::Load);

	SettingsButton->GetButton()->OnClicked.Clear();
	SettingsButton->GetButton()->OnClicked.AddDynamic(this, &UPauseMenu::Settings);
	
	GetExitMainMenuButton()->GetButton()->OnClicked.Clear();
	GetExitMainMenuButton()->GetButton()->OnClicked.AddDynamic(this, &UPauseMenu::ExitMainMenu);

	GetExitDesktopButton()->GetButton()->OnClicked.Clear();
	GetExitDesktopButton()->GetButton()->OnClicked.AddDynamic(this, &UPauseMenu::ExitDesktop);
}

FReply UPauseMenu::NativeOnAnalogValueChanged(const FGeometry& InGeometry, const FAnalogInputEvent& InAnalogEvent)
{
	Super::NativeOnAnalogValueChanged(InGeometry, InAnalogEvent);

	if (InAnalogEvent.GetKey() == EKeys::Gamepad_LeftY)
	{
		MenuNavigation(true, InAnalogEvent.GetAnalogValue(), false, false);
	}
	
	return FReply::Handled();
}

FReply UPauseMenu::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
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
	if (InKeyEvent.GetKey() == EKeys::Escape || InKeyEvent.GetKey() == EKeys::Gamepad_FaceButton_Right)
	{
		Resume();
	}
	if (InKeyEvent.GetKey() == EKeys::Enter || InKeyEvent.GetKey() == EKeys::Gamepad_FaceButton_Bottom)
	{
		GetButtons()[GetNavigationIndex()]->GetButton()->OnClicked.Broadcast();
	}

	return FReply::Handled();
}