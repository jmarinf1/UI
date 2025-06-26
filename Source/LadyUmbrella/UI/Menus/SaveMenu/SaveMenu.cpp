// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveMenu.h"

#include "LadyUmbrella/UI/Basics/AnimatedButtonSaveMenu.h"
#include "LadyUmbrella/Util/Logger.h"
#include "LadyUmbrella/Util/SaveSystem/SaveGameSlotNames.h"
#include "LadyUmbrella/Util/SaveSystem/LU_GameInstance.h"


void USaveMenu::NativeConstruct()
{
	Super::NativeConstruct();
	
	TArray<UAnimatedButtonSaveMenu*> SaveButtons = {
		AnimatedButtonSaveMenuSlot1,
		AnimatedButtonSaveMenuSlot2,
		AnimatedButtonSaveMenuSlot3
	};

	TArray<FString> SlotNames = {
		SaveGameSlots::SaveSlot1,
		SaveGameSlots::SaveSlot2,
		SaveGameSlots::SaveSlot3
	};

	for (int32 i = 0; i < SaveButtons.Num(); ++i)
	{
		SaveButtons[i]->OnButtonClicked.AddDynamic(this, &USaveMenu::OnSaveButtonClicked);
		SaveButtons[i]->SetSaveSlotNameString(SlotNames[i]);
	}
}

void USaveMenu::OnSaveButtonClicked(FString& SaveSlotNamePressed)
{
	if (!SaveSlotNamePressed.IsEmpty())
	{
		ULU_GameInstance* GameInstance = Cast<ULU_GameInstance>(GetGameInstance());
		if (IsValid(GameInstance))
		{
			GameInstance->LoadGame(SaveSlotNamePressed);
		}
	}
}
