// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AgencyCharacter.h"
#include "ShieldAgencyCharacter.generated.h"

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API AShieldAgencyCharacter : public AAgencyCharacter
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "weapon", meta = (AllowPrivateAccess = "true"))
	AAgencyShield* Shield;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AAgencyShield> ShieldSubclass;

	UPROPERTY(EditAnywhere, Category = "Speeds",meta=(ToolTip = "Speed when chracter is moving to pladyer",ClampMin="0", UIMin="0", ForceUnits="cm/s")) 
	float SpeedAproachPlayer;

	UPROPERTY(EditAnywhere, Category = "Speeds",meta=(ToolTip = "Speed when chracter is being shooted by Player",ClampMin="0", UIMin="0", ForceUnits="cm/s")) 
	float SpeedOnBeingAttacked;
	
	UPROPERTY(EditAnywhere, Category = "Speeds",meta=(ToolTip = "Speed when character is finding Player",ClampMin="0", UIMin="0", ForceUnits="cm/s")) 
	float SpeedOnFindPlayer;
	
public:
	// Delegate que se dispara cuando el escudo se destruye
	// UPROPERTY(BlueprintAssignable, Category = "Shield")
	// FOnShieldDestroyedSignature OnShieldDestroyed;
	
	UFUNCTION()
	void HandleShieldDestroyed();

	virtual AActor* GetShield() const override;

	virtual void BeginPlay() override;

	//speed getters
	float GetSpeedOnBeingAttacked() const { return SpeedOnBeingAttacked; };
	float GetSpeedOnFindPlayer() const { return SpeedOnFindPlayer; };
	float GetSpeedOnAproachPlayer() const { return SpeedAproachPlayer; };
private:
	AShieldAgencyCharacter();
};
