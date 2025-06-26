// 
// EUpgradeType.h
// 
// Enum containing the available upgrades for the Umbrella. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#pragma once

UENUM()
enum class EUpgradeType : uint8
{
	// Shotgun upgrades.
	AmmoCapacity = 0 UMETA(DisplayName = "Ammo Capacity"),
	Handling     = 1 UMETA(DisplayName = "Weapon Handling"),
	FireRate     = 2 UMETA(DisplayName = "Fire Rate"),
	ReloadSpeed  = 3 UMETA(DisplayName = "Reload Speed"),

	// Shield upgrades.
	Bounciness   = 4 UMETA(DisplayName = "Bounciness"),
	Endurance    = 5 UMETA(DisplayName = "Endurance"),
	ShieldMotion = 6 UMETA(DisplayName = "Shield Motion"),
};
