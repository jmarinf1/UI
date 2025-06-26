// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FPlayerDataDriven.generated.h"

USTRUCT(BlueprintType)
struct FPlayerDataDriven : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MaxHealth = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RegenerationCoolDown = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RegenerationValue = 0;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RegenerationSpeed = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MinMoveSpeed = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxMoveSpeed = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float AimHandlingSpeed = 0;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ShieldHandlingSpeed = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float OnCoverSpeed = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float OnTalkingSpeed = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxJumpHeight = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxJumpDistance = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float JumpCorrection = 0;
	

};
