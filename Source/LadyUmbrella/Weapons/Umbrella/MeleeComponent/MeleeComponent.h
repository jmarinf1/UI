// 
// MeleeComponent.h
// 
// Allows the player to use the umbrella to melee attack. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "MeleeComponent.generated.h"

UCLASS()
class LADYUMBRELLA_API UMeleeComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	bool bCanDetectMelee;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float Damage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float LaunchFactor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FVector LaunchAddedVelocity;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float TraceRadius;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TWeakObjectPtr<UParticleSystem> ImpactParticleSystem;

public:
	UMeleeComponent();
	
	UFUNCTION(BlueprintCallable)
	void ProcessCollisionMelee(const FVector& TraceStart, const FVector& ActorRightVector);

	UFUNCTION(BlueprintCallable)
	void SetCanDetectMelee(const bool Value);
};
