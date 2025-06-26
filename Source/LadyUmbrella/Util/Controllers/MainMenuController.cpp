// 
// MainMenuController.cpp
// 
// Implementation of the MainMenuController class.
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 


#include "MainMenuController.h"
#include "LadyUmbrella/UI/Menus/MainMenu.h"
#include "Blueprint/UserWidget.h"

void AMainMenuController::BeginPlay()
{
	Super::BeginPlay();

	if (MainMenuWidget)
	{
		MainMenuInstance = CreateWidget<UMainMenu>(GetWorld(), MainMenuWidget);
		MainMenuInstance->AddToViewport();
		MainMenuInstance->Show();
	}
}

