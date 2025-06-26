// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FSaveStruct.h"
#include "FSaveStructPlayer.generated.h"

USTRUCT(BlueprintType, meta=(DisplayName="FSaveStructPlayer"))
struct FSaveStructPlayer : public FSaveStructData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Health = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 UmbrellaInventoryAmmo = 32;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 UmbrellaCurrentMagAmmo = 8;

	// Shooting upgrades
	uint8 UpgradeFireRate				= 0;
	uint8 UpgradeHandling				= 0;
	uint8 UpgradeAmmoCapacity			= 0;
	uint8 UpgradeSpeedReloadSpeed		= 0;
	// Shield upgrades
	uint8 UpgradeShieldEndurance		= 0;
	uint8 UpgradeShieldFasterMovement   = 0;
	uint8 UpgradeShieldBounciness		= 0;
	TArray<uint8> Upgrades;
};