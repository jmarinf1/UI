// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimatedButtonEncounters.h"

#include "Components/Button.h"

void UAnimatedButtonEncounters::NativeConstruct()
{
	Super::NativeConstruct();
	Button->OnPressed.AddDynamic(this, &UAnimatedButtonEncounters::OnPressedButton);
}

void UAnimatedButtonEncounters::OnPressedButton()
{
	OnButtonClicked.Broadcast(Id);
}
