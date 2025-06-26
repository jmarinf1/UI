// 
// MafiaCharacter.h
// 
// Enemy character belonging to the Mafia.
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#pragma once

#include "CoreMinimal.h"
#include "LadyUmbrella/Characters/Enemies/EnemyCharacter.h"
#include "MafiaCharacter.generated.h"

class AGrenade;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGrenadeTokenReleased);

UCLASS()
class LADYUMBRELLA_API AMafiaCharacter : public AEnemyCharacter
{
	GENERATED_BODY()
	
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grenade", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ThrowGrenadeMontage;

	UPROPERTY()
	AGrenade* Grenade;
	
	UPROPERTY(VisibleAnywhere, Category = "Grenade")
	bool bHasGrenadeToken;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grenade", meta = (AllowPrivateAccess = true))
	TSubclassOf<AGrenade> GrenadeBP;

	//Speeds
	UPROPERTY(EditAnywhere, Category = "Speeds",meta=(ToolTip = "Speed when chracter is fleeing",ClampMin="0", UIMin="0", ForceUnits="cm/s")) 
	float SpeedOnFleeing;

	UPROPERTY(EditAnywhere, Category = "Speeds",meta=(ToolTip = "Speed when character is close to player",ClampMin="0", UIMin="0", ForceUnits="cm/s")) 
	float SpeedOnCloseCombat;
	
	UPROPERTY(EditAnywhere, Category = "Speeds",meta=(ToolTip = "Speed when character is enter arena called by event",ClampMin="0", UIMin="0", ForceUnits="cm/s")) 
	float SpeedOnEnterArena;

	UPROPERTY(EditAnywhere, Category = "Speeds",meta=(ToolTip = "Speed when character is patroling",ClampMin="0", UIMin="0", ForceUnits="cm/s")) 
	float SpeedOnPatroling;

	UPROPERTY(EditAnywhere, Category = "Speeds",meta=(ToolTip = "Speed when character is moving towards one Cover",ClampMin="0", UIMin="0", ForceUnits="cm/s")) 
	float SpeedOnMoveToCover;

	UPROPERTY()
	FTimerHandle ResetCollisionTimerHandler;

	UFUNCTION()
	void OnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	UFUNCTION()
	void ResetCollision();

protected:
	UPROPERTY(EditAnywhere, Category = "Grenade")
	FVector GrenadeEndPosition;

	UPROPERTY(EditAnywhere, Category = "Grenade")
	float GrenadeEndLocationZOffset;

	UPROPERTY(EditAnywhere, Category = "Grenade")
	float GrenadeEndLocationForwardOffset;

	UPROPERTY(EditAnywhere, Category = "Grenade")
	float GrenadeEndLocationLateralOffset;

	UPROPERTY(EditAnywhere, Category = "Grenade")
	float GrenadeMinLateralOffset;

	UPROPERTY(EditAnywhere, Category = "Grenade")
	float GrenadeArc;
public:
	AMafiaCharacter();
	
	static FOnGrenadeTokenReleased OnGrenadeTokenReleased;

	UFUNCTION()
	void OnThrowGrenadeEnd(UAnimMontage* Montage, bool bInterrupted);
	
	UFUNCTION()
	void ThrowGrenade();

	void SetHasGrenadeToken(bool hasGrenadeToken);

	bool GetHasGrenadeToken() const;

	//speeds getters
	float GetSpeedOnFleeing() const { return SpeedOnFleeing; }
	float GetSpeedOnCloseCombat() const { return SpeedOnCloseCombat; }
	float GetSpeedOnEnterArena() const { return SpeedOnEnterArena; }
	float GetSpeedOnPatroling() const { return SpeedOnPatroling; }
	float GetSpeedOnMoveToCover() const { return SpeedOnMoveToCover; }
};
