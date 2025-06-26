// 
// MainHUD.cpp
// 
// Implementation of the MainHUD class. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#include "MainHUD.h"
#include "LadyUmbrella/UI/Menus/PauseMenu.h"
#include "LadyUmbrella/UI/HUD/PlayerHealth.h"
#include "LadyUmbrella/UI/HUD/AmmoCounter.h"
#include "LadyUmbrella/UI/HUD/Crosshair.h"
#include "Blueprint/UserWidget.h"
#include "LadyUmbrella/UI/Menus/CheatMenu.h"
#include "LadyUmbrella/UI/Menus/SettingsMenu.h"
#include "LadyUmbrella/UI/Menus/SettingsOptions/ControlSettings.h"
#include "LadyUmbrella/UI/HUD/Subtitles.h"

void AMainHUD::BeginPlay()
{
	Super::BeginPlay();
	
	SetPauseMenuInstance(GetPauseMenuWidget());
	SetCheatMenuInstance(GetCheatMenuWidget());
	SetSettingsMenuInstance(GetSettingsMenuWidget());
	SetControlSettingsInstance(GetControlSettingsWidget());
	SetPlayerHealthInstance(GetPlayerHealthWidget());
	SetCrosshairInstance(GetCrosshairWidget());
	SetSubtitleInstance(GetSubtitlesWidget());
	
	GetPlayerHealthInstance()->AddToViewport();
	GetCrosshairInstance()->AddToViewport();
	GetSubtitlesInstance()->AddToViewport();
}

void AMainHUD::OnUmbrellaSpawned(APlayerCharacter* Character)
{
	SetAmmoCounterInstance(GetAmmoCounterWidget());
	GetAmmoCounterInstance()->AddToViewport();
	TogglePlayerHUD();
}

void AMainHUD::TogglePauseMenu() const
{
	if (IsValid(GetPauseMenuInstance()))
	{
		GetPauseMenuInstance()->Show();
	}
}

void AMainHUD::ToggleCheatMenu()
{
	if (IsValid(GetCheatMenuInstance()) && bCheatMenuCanToggle)
	{
		if (GetCheatMenuInstance()->IsVisible())
		{
			GetCheatMenuInstance()->Hide();
		}
		else
		{
			GetCheatMenuInstance()->Show();
		}

		bCheatMenuCanToggle = false;
	}
}

void AMainHUD::EnableToggleCheatMenu()
{
	bCheatMenuCanToggle = true;
}

void AMainHUD::ToggleSettingsMenu() const
{
	if (IsValid(GetSettingsMenuInstance()))
	{
		GetSettingsMenuInstance()->Show();
	}
}

void AMainHUD::ToggleControlSettingsMenu() const
{
	if (IsValid(GetControlSettingsInstance()))
	{
		GetControlSettingsInstance()->Show();
	}
}

void AMainHUD::TogglePlayerHUD() const
{
	if (IsValid(GetPlayerHealthInstance()) && IsValid(GetAmmoCounterInstance()) && IsValid(GetCrosshairInstance()))
	{
		GetPlayerHealthInstance()->Show();
		GetAmmoCounterInstance()->Show();
		GetCrosshairInstance()->Show();
	}
}

void AMainHUD::UnTogglePlayerHUD() const
{
	if (IsValid(GetPlayerHealthInstance()) && IsValid(GetAmmoCounterInstance()) && IsValid(GetCrosshairInstance()))
	{
		GetPlayerHealthInstance()->Hide();
		GetAmmoCounterInstance()->Hide();
		GetCrosshairInstance()->Hide();
	}
}

void AMainHUD::ClearWidgets()
{
	if (GetPauseMenuInstance())
	{
		GetPauseMenuInstance()->RemoveFromParent();
		SetPauseMenuInstance(nullptr);
	}

	if (GetSettingsMenuInstance())
	{
		GetSettingsMenuInstance()->RemoveFromParent();
		SetSettingsMenuInstance(nullptr);
	}

	if (GetControlSettingsInstance())
	{
		GetControlSettingsInstance()->RemoveFromParent();
		SetControlSettingsInstance(nullptr);
	}

	if (GetPlayerHealthInstance())
	{
		GetPlayerHealthInstance()->RemoveFromParent();
		SetPlayerHealthInstance(nullptr);
	}

	if (GetAmmoCounterInstance())
	{
		GetAmmoCounterInstance()->RemoveFromParent();
		SetAmmoCounterInstance(nullptr);
	}

	if (GetCrosshairInstance())
	{
		GetCrosshairInstance()->RemoveFromParent();
		SetCrosshairInstance(nullptr);
	}

	if (GetSubtitlesInstance())
	{
		GetSubtitlesInstance()->RemoveFromParent();
		SetSubtitleInstance(nullptr);
	}
}

void AMainHUD::AddSubtitle(const FString& CharacterName, const FString& Subtitle) const
{
	GetSubtitlesInstance()->AddSubtitle(CharacterName, Subtitle);
}

UPauseMenu* AMainHUD::GetPauseMenuInstance() const 
{
	return PauseMenuInstance;
}

UCheatMenu* AMainHUD::GetCheatMenuInstance() const 
{
	return CheatMenuInstance;
}

TSubclassOf<UCheatMenu> AMainHUD::GetCheatMenuWidget() const
{
	return CheatMenuWidget;
}

UPlayerHealth* AMainHUD::GetPlayerHealthInstance() const
{
	return PlayerHealthInstance;
}

TSubclassOf<UPlayerHealth> AMainHUD::GetPlayerHealthWidget() const
{
	return PlayerHealthWidget;
}

UAmmoCounter* AMainHUD::GetAmmoCounterInstance() const
{
	return AmmoCounterInstance;
}

TSubclassOf<UAmmoCounter> AMainHUD::GetAmmoCounterWidget() const
{
	return AmmoCounterWidget;
}

UCrosshair* AMainHUD::GetCrosshairInstance() const
{
	return CrosshairInstance;
}

TSubclassOf<UCrosshair> AMainHUD::GetCrosshairWidget() const
{
	return CrosshairWidget;
}

TSubclassOf<UPauseMenu> AMainHUD::GetPauseMenuWidget() const
{
	return PauseMenuWidget;
}

USettingsMenu* AMainHUD::GetSettingsMenuInstance() const
{
	return SettingsMenuInstance;
}

TSubclassOf<USettingsMenu>  AMainHUD::GetSettingsMenuWidget() const
{
	return SettingsMenuWidget;
}

UControlSettings* AMainHUD::GetControlSettingsInstance() const
{
	return ControlSettingsMenuInstance;
}

TSubclassOf<UControlSettings>  AMainHUD::GetControlSettingsWidget() const
{
	return ControlSettingsMenuWidget;
}

USubtitles* AMainHUD::GetSubtitlesInstance() const
{
	return SubtitlesInstance;
}

TSubclassOf<USubtitles> AMainHUD::GetSubtitlesWidget() const
{
	return SubtitlesWidget;
}

void AMainHUD::SetPauseMenuInstance(const TSubclassOf<UPauseMenu>& NewPauseMenuWidget)
{
	if (!IsValid(GetPauseMenuInstance()) && IsValid(NewPauseMenuWidget))
	{
		PauseMenuInstance = CreateWidget<UPauseMenu>(GetWorld(), NewPauseMenuWidget);
	}
}

void AMainHUD::SetCheatMenuInstance(const TSubclassOf<UCheatMenu>& NewCheatMenuWidget)
{
	if (!IsValid(GetCheatMenuInstance()) && IsValid(NewCheatMenuWidget))
	{
		CheatMenuInstance = CreateWidget<UCheatMenu>(GetWorld(), NewCheatMenuWidget);
	}
}

void AMainHUD::SetPlayerHealthInstance(const TSubclassOf<UPlayerHealth>& NewPlayerHealthWidget)
{
	if (!IsValid(GetPlayerHealthInstance()) && IsValid(NewPlayerHealthWidget))
	{
		PlayerHealthInstance = CreateWidget<UPlayerHealth>(GetWorld(), NewPlayerHealthWidget);
	}
}

void AMainHUD::SetAmmoCounterInstance(const TSubclassOf<UAmmoCounter>& NewAmmoCounterWidget)
{
	if (!IsValid(GetAmmoCounterInstance()) && IsValid(NewAmmoCounterWidget))
	{
		AmmoCounterInstance = CreateWidget<UAmmoCounter>(GetWorld(), NewAmmoCounterWidget);
	}
}

void AMainHUD::SetCrosshairInstance(const TSubclassOf<UCrosshair>& NewCrosshairWidget)
{
	if (!IsValid(GetCrosshairInstance()) && IsValid(NewCrosshairWidget))
	{
		CrosshairInstance = CreateWidget<UCrosshair>(GetWorld(), NewCrosshairWidget);
	}
}

void AMainHUD::SetSettingsMenuInstance(const TSubclassOf<USettingsMenu>& NewSettingsMenuWidget)
{
	if (!IsValid(GetSettingsMenuInstance()) && IsValid(NewSettingsMenuWidget))
	{
		SettingsMenuInstance = CreateWidget<USettingsMenu>(GetWorld(), NewSettingsMenuWidget);
	}
}

void AMainHUD::SetControlSettingsInstance(const TSubclassOf<UControlSettings>& NewControlSettingsWidget)
{
	if (!IsValid(GetControlSettingsInstance()) && IsValid(NewControlSettingsWidget))
	{
		ControlSettingsMenuInstance = CreateWidget<UControlSettings>(GetWorld(), NewControlSettingsWidget);
	}
}

void AMainHUD::SetSubtitleInstance(const TSubclassOf<USubtitles>& NewSubtitleWidget)
{
	if (!IsValid(GetSubtitlesInstance()) && IsValid(NewSubtitleWidget))
	{
		SubtitlesInstance = CreateWidget<USubtitles>(GetWorld(), NewSubtitleWidget);
	}
}
