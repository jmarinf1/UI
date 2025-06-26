// 
// ElectricProjectile.h
// 
// Umbrella gadget that stuns enemies and activates electric puzzles. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#pragma once

#include "CoreMinimal.h"
#include "LadyUmbrella/Weapons/GenericProjectile.h"

#include "ElectricProjectile.generated.h"

UCLASS()
class LADYUMBRELLA_API AElectricProjectile : public AGenericProjectile
{
	GENERATED_BODY()
public:
	
	AElectricProjectile();
	
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(EditAnywhere,Category="Projectile")
	float DamageToUmbrella;
	
protected:
	virtual void BeginPlay() override;
	virtual void ProcessCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                              int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

};
