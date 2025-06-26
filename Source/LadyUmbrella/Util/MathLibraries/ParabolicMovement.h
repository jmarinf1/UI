// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class LADYUMBRELLA_API ParabolicMovement
{
public:	
	static FVector CalculateParabolicLaunchVelocity(FVector Start, FVector End, float Time, float GravityZ);
	static void DrawDebugParabola(UWorld* World, FVector Start, FVector InitialVelocity, float GravityZ, float TotalTime);
};
