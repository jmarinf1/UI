// 
// UpgradeComponent.cpp
// 
// Implementation of the UmbrellaComponent class. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#include "UpgradeComponent.h"

UUpgradeComponent::UUpgradeComponent()
{

	// Shotgun upgrades. 
	Upgrades.Emplace(EUpgradeType::AmmoCapacity, FUpgrade("Ammo Capacity","Increase the ammo capacity", 3, 100));
	Upgrades.Emplace(EUpgradeType::Handling, FUpgrade("Handling","Increase the handling of the upgrade", 2, 100));
	Upgrades.Emplace(EUpgradeType::FireRate, FUpgrade("Fire Rate","Increase the fire rate of the shotgun", 3, 100));
	Upgrades.Emplace(EUpgradeType::ReloadSpeed, FUpgrade("Reload Speed","Increase the reload speed", 3, 100));

	// Shield upgrades. 
	Upgrades.Emplace(EUpgradeType::Bounciness, FUpgrade("Bounciness","Increase the bounciness", 2, 100));
	Upgrades.Emplace(EUpgradeType::Endurance, FUpgrade("Endurance","Increase the health of the shield", 3, 100));
	Upgrades.Emplace(EUpgradeType::ShieldMotion, FUpgrade("Shield Motion","Increase the shield agility", 2, 100));
	
}

FUpgrade* UUpgradeComponent::Get(const EUpgradeType Type)
{
	return Upgrades.Find(Type);
}

const TMap<EUpgradeType, FUpgrade>& UUpgradeComponent::GetMap() const
{
	return Upgrades;
}
