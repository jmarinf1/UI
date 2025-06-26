//  
// MainMenu.cpp
// 
// Implementation of the MainMenu class.
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#include "MainMenu.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "LadyUmbrella/UI/Basics/AnimatedButton.h"
#include "Components/Button.h"
#include "LadyUmbrella/Util/SaveSystem/SaveGameSlotNames.h"
#include "LadyUmbrella/Util/SaveSystem/LU_GameInstance.h"
#include "Components/VerticalBox.h"

void UMainMenu::NativeConstruct()
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
	
	GetNewGameButton()->GetButton()->OnClicked.AddDynamic(this, &UMainMenu::NewGame);
	GetContinueButton()->GetButton()->OnClicked.AddDynamic(this, &UMainMenu::ContinueGame);
	GetExitGameButton()->GetButton()->OnClicked.AddDynamic(this, &UMainMenu::ExitDesktop);
}

void UMainMenu::ContinueGame()
{
	// me falta poner que cuando un checkpoint se hitee, se cambie el nombre del lastlevelreached. Hay que gestionarlo con el tema del cambio niveles
	// create slots menu widget. Set this to show false, set Slots Menu to visible and manage logic from there
	ULU_GameInstance* GameInstance = Cast<ULU_GameInstance>( GetGameInstance() );

	if (IsValid(GameInstance))
	{
		GameInstance->OpenLevelFromLoadGame();
		//GameInstance->LoadGame( GameInstance->GetCurrentSaveSlot() );
	}
}

void UMainMenu::Show()
{
	APlayerController* PlayerController = GetOwningPlayer<APlayerController>();
	PlayerController->SetInputMode(FInputModeUIOnly());
	PlayerController->bShowMouseCursor = true;
	SetNavigationIndex(0);
	GetButtons()[GetNavigationIndex()]->SetKeyboardFocus();
}

void UMainMenu::NewGame()
{
	ULU_GameInstance* GameInstance = Cast<ULU_GameInstance>( GetGameInstance() );

	if (IsValid(GameInstance))
	{
		GameInstance->SetCurrentSaveSlot(SaveGameSlots::SaveSlot1);
	}
	
	APlayerController* PlayerController = GetOwningPlayer<APlayerController>();
	PlayerController->SetInputMode(FInputModeGameOnly());
	PlayerController->bShowMouseCursor = false;
	RemoveFromParent();
	UGameplayStatics::OpenLevel(this, TEXT("L_ConstructionBuilding"));
}

void UMainMenu::ExitDesktop()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, true);
}

FReply UMainMenu::NativeOnAnalogValueChanged(const FGeometry& InGeometry, const FAnalogInputEvent& InAnalogEvent)
{
	Super::NativeOnAnalogValueChanged(InGeometry, InAnalogEvent);

	if (InAnalogEvent.GetKey() == EKeys::Gamepad_LeftY)
	{
		MenuNavigation(true, InAnalogEvent.GetAnalogValue(), false, false);
	}
	
	return FReply::Handled();
}

FReply UMainMenu::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
	
	if (InKeyEvent.GetKey() == EKeys::Up || InKeyEvent.GetKey() == EKeys::Gamepad_DPad_Up)
	{
		MenuNavigation(false, 0, false, true);
	}
	else if (InKeyEvent.GetKey() == EKeys::Down || InKeyEvent.GetKey() == EKeys::Gamepad_DPad_Down)
	{
		MenuNavigation(false, 0, true, false);
	}

	if (InKeyEvent.GetKey() == EKeys::Enter || InKeyEvent.GetKey() == EKeys::Gamepad_FaceButton_Bottom)
	{
		GetButtons()[GetNavigationIndex()]->GetButton()->OnClicked.Broadcast();
	}

	return FReply::Handled();
}