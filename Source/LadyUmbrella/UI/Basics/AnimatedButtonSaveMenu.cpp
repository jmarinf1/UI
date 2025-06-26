// Fill out your copyright notice in the Description page of Project Settings.
#include "AnimatedButtonSaveMenu.h"

#include "Components/Button.h"

void UAnimatedButtonSaveMenu::NativeConstruct()
{
	Super::NativeConstruct();
	Button->OnPressed.AddDynamic(this, &UAnimatedButtonSaveMenu::OnPressedButton);
}

void UAnimatedButtonSaveMenu::OnPressedButton()
{
	OnButtonClicked.Broadcast(SaveSlotName);
}
