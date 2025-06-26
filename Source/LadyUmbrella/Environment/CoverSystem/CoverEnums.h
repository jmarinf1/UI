// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoverEnums.generated.h"

/**
 * Used in Generic Character to determine when and how the cover system is affecting the character.
 */
UENUM(BlueprintType)
enum class ECoveredState : uint8
{
	NoCover UMETA(DisplayName = "NoCover"),
	InCover UMETA(DisplayName = "InCover"),
	EnteringCover UMETA(DisplayName = "EnteringCover"),
	ExitingCover UMETA(DisplayName = "ExitingCover"),
};


/**
 * Used in Generic Character to determine the state of a character while inside a Cover.
 */
UENUM(BlueprintType)
enum class EInsideCoverState : uint8
{
	BaseCovered UMETA(DisplayName = "BaseCovered"),
	Aiming UMETA(DisplayName = "Aiming"),
	MovingBetweenCovers UMETA(DisplayName = "MovingBetweenCovers"),
};
