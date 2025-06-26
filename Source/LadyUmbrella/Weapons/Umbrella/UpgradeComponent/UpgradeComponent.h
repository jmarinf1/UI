// 
// UpgradeComponent.h
// 
// Information about the possible umbrella upgrades go here. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#pragma once

#include "LadyUmbrella/Weapons/Umbrella/UpgradeComponent/Upgrade.h"
#include "LadyUmbrella/Weapons/Umbrella/UpgradeComponent/UpgradeType.h"

#include "UpgradeComponent.generated.h"

UCLASS()
class LADYUMBRELLA_API UUpgradeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UUpgradeComponent();
	
	FUpgrade* Get(const EUpgradeType Type);
	const TMap<EUpgradeType, FUpgrade>& GetMap() const;

	TMap<EUpgradeType, FUpgrade> Upgrades;
};
