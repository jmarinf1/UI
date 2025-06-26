// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LadyUmbrella/Characters/Enemies/EnemyCharacter.h"
#include "AgencyCharacter.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShieldDestroyedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnElectricBulletTokenRequest, bool, bIsRequesting, AAgencyCharacter*, AgencyCharacter);

class AAgencyShield;

UCLASS()
class LADYUMBRELLA_API AAgencyCharacter : public AEnemyCharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data", meta = (AllowPrivateAccess = "true"))
	int NumRafaguesRemaining;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", meta = (AllowPrivateAccess = "true"))
	int MinNumRafagues;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", meta = (AllowPrivateAccess = "true"))
	int MaxNumRafagues;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data", meta = (AllowPrivateAccess = "true"))
	bool bHasElectricBullet;

	//Speeds
	UPROPERTY(EditAnywhere, Category = "Speeds",meta=(ToolTip = "Speed when chracter is moving crounched between Covers",ClampMin="0", UIMin="0", ForceUnits="cm/s")) 
	float SpeedOnMoveBetweenCovers;

	UPROPERTY(EditAnywhere, Category = "Speeds",meta=(ToolTip = "Speed when chracter is close to player",ClampMin="0", UIMin="0", ForceUnits="cm/s")) 
	float SpeedOnCloseCombat;
	
	UPROPERTY(EditAnywhere, Category = "Speeds",meta=(ToolTip = "Speed when character is moving around their own cover",ClampMin="0", UIMin="0", ForceUnits="cm/s")) 
	float SpeedOnMoveAroundCover;
	
protected:
	//virtual void BeginPlay() override;
	AAgencyCharacter();
public:
	
	static FOnElectricBulletTokenRequest OnElectricBulletTokenRequest;
	
	FORCEINLINE int GetRemainingRafagues() { return NumRafaguesRemaining;}
	FORCEINLINE void SetRemainingRafagues(int NewRafaguesRemaining) { NumRafaguesRemaining = NewRafaguesRemaining; }
	void SetRandomNumRafagues();

	//Speeds getters
	float GetSpeedOnMoveBetweenCovers() const { return SpeedOnMoveBetweenCovers;}
	float GetSpeedOnCloseCombat() const { return SpeedOnCloseCombat;}
	float GetSpeedOnMoveAroundCover() const { return SpeedOnMoveAroundCover;}

	FORCEINLINE void SetHasElectricBullet(bool bNewHasElectricBullet) { bHasElectricBullet = bNewHasElectricBullet; }
	FORCEINLINE bool GetHasElectricBullet() const { return bHasElectricBullet; }
};
