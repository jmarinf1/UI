// Fill out your copyright notice in the Description page of Project Settings.


#include "ChapterSelectorMenu.h"

#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Kismet/GameplayStatics.h"
#include "LadyUmbrella/UI/EncounterMenu.h"
#include "LadyUmbrella/UI/Basics/AnimatedButton.h"



void UChapterSelectorMenu::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (IsValid(WGMenuButtonFirstLevel  ) &&
		IsValid(WGMenuButtonSecondLevel ) &&
		IsValid(WGMenuButtonThirdLevel  ) &&
		IsValid(WGMenuButtonFourthLevel ) &&
		IsValid(WGMenuButtonFifthLevel  ) && 
		IsValid(WGMenuButtonFifthLevel  )
		)
	{
		WGMenuButtonFirstLevel->GetButton()->OnPressed.AddDynamic(this	, &UChapterSelectorMenu::OpenLevel1);
		WGMenuButtonSecondLevel->GetButton()->OnPressed.AddDynamic(this	, &UChapterSelectorMenu::OpenLevel2);
		WGMenuButtonThirdLevel->GetButton()->OnPressed.AddDynamic(this	, &UChapterSelectorMenu::OpenLevel3);
		WGMenuButtonFourthLevel->GetButton()->OnPressed.AddDynamic(this	, &UChapterSelectorMenu::OpenLevel4);
		WGMenuButtonFifthLevel->GetButton()->OnPressed.AddDynamic(this	, &UChapterSelectorMenu::OpenLevel5);

		// The focus of this button has to be controlled from its class, virtual method instead of delegate
		WGMenuButtonFirstLevel->GetButton()->OnHovered.AddDynamic(this, &UChapterSelectorMenu::OnHoverButtonFirstLevel);
		WGMenuButtonSecondLevel->GetButton()->OnHovered.AddDynamic(this, &UChapterSelectorMenu::OnHoverButtonSecondLevel);
		WGMenuButtonThirdLevel->GetButton()->OnHovered.AddDynamic(this, &UChapterSelectorMenu::OnHoverButtonThirdLevel);
		WGMenuButtonFourthLevel->GetButton()->OnHovered.AddDynamic(this, &UChapterSelectorMenu::OnHoverButtonFourthLevel);
		WGMenuButtonFifthLevel->GetButton()->OnHovered.AddDynamic(this, &UChapterSelectorMenu::OnHoverButtonFifthLevel);
		// Need one more button for the last chapter
		WGMenuButtonSixthLevel->GetButton()->OnHovered.AddDynamic(this, &UChapterSelectorMenu::OnHoverButtonSixthLevel);
				
	}
	if (IsValid(WBEncounterMenu))
	{
		WBEncounterMenu->EncounterScrollBox_Chapter1->SetVisibility(ESlateVisibility::Visible);
		WBEncounterMenu->EncounterScrollBox_Chapter2->SetVisibility(ESlateVisibility::Collapsed);
		WBEncounterMenu->EncounterScrollBox_Chapter3->SetVisibility(ESlateVisibility::Collapsed);
		WBEncounterMenu->EncounterScrollBox_Chapter4->SetVisibility(ESlateVisibility::Collapsed);
		WBEncounterMenu->EncounterScrollBox_Chapter5->SetVisibility(ESlateVisibility::Collapsed);
		WBEncounterMenu->EncounterScrollBox_Chapter6->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UChapterSelectorMenu::OpenLevel1()
{
	if (FirstLevelName.Compare(""))
	{
		UGameplayStatics::OpenLevel(this, FirstLevelName);
	}
}

void UChapterSelectorMenu::OpenLevel2()
{
	if (SecondLevelName.Compare(""))
	{
		UGameplayStatics::OpenLevel(this, SecondLevelName);
	}
}

void UChapterSelectorMenu::OpenLevel3()
{
	if (ThirdLevelName.Compare(""))
	{
		UGameplayStatics::OpenLevel(this, ThirdLevelName);
	}
}

void UChapterSelectorMenu::OpenLevel4()
{
	if (FourthLevelName.Compare(""))
	{
		UGameplayStatics::OpenLevel(this, FourthLevelName);
	}
}

void UChapterSelectorMenu::OpenLevel5()
{
	if (FifthLevelName.Compare(""))
	{
		UGameplayStatics::OpenLevel(this, FifthLevelName);
	}
}

void UChapterSelectorMenu::OnHoverButtonFirstLevel()
{
	WBEncounterMenu->EncounterScrollBox_Chapter1->SetVisibility(ESlateVisibility::Visible);
	WBEncounterMenu->EncounterScrollBox_Chapter2->SetVisibility(ESlateVisibility::Collapsed);
	WBEncounterMenu->EncounterScrollBox_Chapter3->SetVisibility(ESlateVisibility::Collapsed);
	WBEncounterMenu->EncounterScrollBox_Chapter4->SetVisibility(ESlateVisibility::Collapsed);
	WBEncounterMenu->EncounterScrollBox_Chapter5->SetVisibility(ESlateVisibility::Collapsed);
	WBEncounterMenu->EncounterScrollBox_Chapter6->SetVisibility(ESlateVisibility::Collapsed);
}

void UChapterSelectorMenu::OnHoverButtonSecondLevel()
{
	WBEncounterMenu->EncounterScrollBox_Chapter1->SetVisibility(ESlateVisibility::Collapsed);
	WBEncounterMenu->EncounterScrollBox_Chapter2->SetVisibility(ESlateVisibility::Visible);
	WBEncounterMenu->EncounterScrollBox_Chapter3->SetVisibility(ESlateVisibility::Collapsed);
	WBEncounterMenu->EncounterScrollBox_Chapter4->SetVisibility(ESlateVisibility::Collapsed);
	WBEncounterMenu->EncounterScrollBox_Chapter5->SetVisibility(ESlateVisibility::Collapsed);
	WBEncounterMenu->EncounterScrollBox_Chapter6->SetVisibility(ESlateVisibility::Collapsed);
}

void UChapterSelectorMenu::OnHoverButtonThirdLevel()
{
	WBEncounterMenu->EncounterScrollBox_Chapter1->SetVisibility(ESlateVisibility::Collapsed);
	WBEncounterMenu->EncounterScrollBox_Chapter2->SetVisibility(ESlateVisibility::Collapsed);
	WBEncounterMenu->EncounterScrollBox_Chapter3->SetVisibility(ESlateVisibility::Visible);
	WBEncounterMenu->EncounterScrollBox_Chapter4->SetVisibility(ESlateVisibility::Collapsed);
	WBEncounterMenu->EncounterScrollBox_Chapter5->SetVisibility(ESlateVisibility::Collapsed);
	WBEncounterMenu->EncounterScrollBox_Chapter6->SetVisibility(ESlateVisibility::Collapsed);
}

void UChapterSelectorMenu::OnHoverButtonFourthLevel()
{
	WBEncounterMenu->EncounterScrollBox_Chapter1->SetVisibility(ESlateVisibility::Collapsed);
	WBEncounterMenu->EncounterScrollBox_Chapter2->SetVisibility(ESlateVisibility::Collapsed);
	WBEncounterMenu->EncounterScrollBox_Chapter3->SetVisibility(ESlateVisibility::Collapsed);
	WBEncounterMenu->EncounterScrollBox_Chapter4->SetVisibility(ESlateVisibility::Visible);
	WBEncounterMenu->EncounterScrollBox_Chapter5->SetVisibility(ESlateVisibility::Collapsed);
	WBEncounterMenu->EncounterScrollBox_Chapter6->SetVisibility(ESlateVisibility::Collapsed);
}

void UChapterSelectorMenu::OnHoverButtonFifthLevel()
{
	WBEncounterMenu->EncounterScrollBox_Chapter1->SetVisibility(ESlateVisibility::Collapsed);
	WBEncounterMenu->EncounterScrollBox_Chapter2->SetVisibility(ESlateVisibility::Collapsed);
	WBEncounterMenu->EncounterScrollBox_Chapter3->SetVisibility(ESlateVisibility::Collapsed);
	WBEncounterMenu->EncounterScrollBox_Chapter4->SetVisibility(ESlateVisibility::Collapsed);
	WBEncounterMenu->EncounterScrollBox_Chapter5->SetVisibility(ESlateVisibility::Visible);
	WBEncounterMenu->EncounterScrollBox_Chapter6->SetVisibility(ESlateVisibility::Collapsed);
}

void UChapterSelectorMenu::OnHoverButtonSixthLevel()
{
	WBEncounterMenu->EncounterScrollBox_Chapter1->SetVisibility(ESlateVisibility::Collapsed);
	WBEncounterMenu->EncounterScrollBox_Chapter2->SetVisibility(ESlateVisibility::Collapsed);
	WBEncounterMenu->EncounterScrollBox_Chapter3->SetVisibility(ESlateVisibility::Collapsed);
	WBEncounterMenu->EncounterScrollBox_Chapter4->SetVisibility(ESlateVisibility::Collapsed);
	WBEncounterMenu->EncounterScrollBox_Chapter5->SetVisibility(ESlateVisibility::Collapsed);
	WBEncounterMenu->EncounterScrollBox_Chapter6->SetVisibility(ESlateVisibility::Visible);
}