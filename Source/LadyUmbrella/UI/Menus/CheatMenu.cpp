
#include "CheatMenu.h"

#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/EditableTextBox.h"
#include "Kismet/GameplayStatics.h"
#include "LadyUmbrella/Util/Logger.h"
#include "LadyUmbrella/Util/Controllers/MainController.h"

void UCheatMenu::Show()
{
	APlayerController* PlayerController = GetOwningPlayer<APlayerController>();

	if (IsValid(PlayerController))
	{
		PlayerController->SetInputMode(FInputModeGameAndUI());
		PlayerController->bShowMouseCursor = true;
		AddToViewport();
		SetVisibility(ESlateVisibility::Visible);
	}
}

void UCheatMenu::Hide()
{
	APlayerController* PlayerController = GetOwningPlayer<APlayerController>();
	
	if (IsValid(PlayerController))
	{
		PlayerController->SetInputMode(FInputModeGameOnly());
		PlayerController->bShowMouseCursor = false;
		SetVisibility(ESlateVisibility::Hidden);
	}
}

void UCheatMenu::Godmode()
{
	AMainController* MainController = Cast<AMainController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (IsValid(MainController))
	{
		FLogger::DebugLog("Godmode Toggled!");
		MainController->Godmode();
	}
}

void UCheatMenu::Noclip()
{
	AMainController* MainController = Cast<AMainController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (IsValid(MainController))
	{
		FLogger::DebugLog("Noclip Toggled!");
		MainController->NoClip();
	}
}

void UCheatMenu::InfiniteAmmo()
{
	AMainController* MainController = Cast<AMainController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (IsValid(MainController))
	{
		FLogger::DebugLog("Infinite Ammo Toggled!");
		MainController->InfiniteAmmo();
	}
}

void UCheatMenu::UnlockGadgets()
{
	FLogger::DebugLog("Unlocked all gadgets!");
}

void UCheatMenu::UnlockUpgrades()
{
	FLogger::DebugLog("Unlocked all upgrades!");
}

void UCheatMenu::SetAmmoCurrent(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (!UE::String::IsNumeric(Text.ToString()))
	{
		FLogger::ErrorLog("Invalid AmmoCurrent Value!");
		return;
	}

	AMainController* MainController = Cast<AMainController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (IsValid(MainController))
	{
		FLogger::DebugLog("Current Ammo Changed!");
		MainController->SetAmmoCurrent(FCString::Atoi(*Text.ToString()));
	}
}

void UCheatMenu::SetAmmoReserve(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (!UE::String::IsNumeric(Text.ToString()))
	{
		FLogger::ErrorLog("Invalid AmmoReserve Value!");
		return;
	}

	AMainController* MainController = Cast<AMainController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (IsValid(MainController))
	{
		FLogger::DebugLog("Current Reserve Changed!");
		MainController->SetAmmoReserve(FCString::Atoi(*Text.ToString()));
	}
}

void UCheatMenu::SetComponents(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (!UE::String::IsNumeric(Text.ToString()))
	{
		FLogger::ErrorLog("Invalid AmmoCurrent Value!");
		return;
	}

	AMainController* MainController = Cast<AMainController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (IsValid(MainController))
	{
		FLogger::DebugLog("Current Reserve Changed!");
		MainController->SetComponents(FCString::Atoi(*Text.ToString()));
	}
}

void UCheatMenu::NativeConstruct()
{
	Super::NativeConstruct();
	
	GetGodmodeButton()->OnClicked.Clear();
	GetGodmodeButton()->OnClicked.AddDynamic(this, &UCheatMenu::Godmode);
	
	GetNoclipButton()->OnClicked.Clear();
	GetNoclipButton()->OnClicked.AddDynamic(this, &UCheatMenu::Noclip);
	
	GetInfiniteAmmoButton()->OnClicked.Clear();
	GetInfiniteAmmoButton()->OnClicked.AddDynamic(this, &UCheatMenu::InfiniteAmmo);
	
	GetUnlockGadgetsButton()->OnClicked.Clear();
	GetUnlockGadgetsButton()->OnClicked.AddDynamic(this, &UCheatMenu::UnlockGadgets);
	
	GetUnlockUpgradesButton()->OnClicked.Clear();
	GetUnlockUpgradesButton()->OnClicked.AddDynamic(this, &UCheatMenu::UnlockUpgrades);

	GetAmmoCurrentTextBox()->OnTextCommitted.AddDynamic(this, &UCheatMenu::SetAmmoCurrent);
	GetAmmoReserveTextBox()->OnTextCommitted.AddDynamic(this, &UCheatMenu::SetAmmoReserve);
	GetComponentsTextBox()->OnTextCommitted.AddDynamic(this, &UCheatMenu::SetComponents);
	
}
