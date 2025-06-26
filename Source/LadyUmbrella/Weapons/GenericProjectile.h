// 
// GenericProjectile.h
// 
// Main class for all the projectiles in the game. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GenericProjectile.generated.h"

class UProjectileMovementComponent;
class USphereComponent;

UCLASS()
class LADYUMBRELLA_API AGenericProjectile : public AActor
{
	GENERATED_BODY()

	/****************************/
	/*         GAMEPLAY         */
	/****************************/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General", meta = (AllowPrivateAccess = true))
	float ProjectileDamage;

	/****************************/
	/*         PHYSICS          */
	/****************************/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Physics", meta = (AllowPrivateAccess = true))
	TWeakObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;


	
public:
	AGenericProjectile();

	UFUNCTION()
	virtual void ProcessCollision(  UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	FORCEINLINE float GetProjectileDamage() const { return ProjectileDamage; }
	FORCEINLINE TWeakObjectPtr<UProjectileMovementComponent> GetProjectileMovementComponent() const { return ProjectileMovementComponent; }
	FORCEINLINE TWeakObjectPtr<USphereComponent> GetSphereComponent() const { return SphereComponent; }

	FORCEINLINE void SetProjectileDamage(const float Value) { ProjectileDamage = Value; }
	FORCEINLINE void SetProjectileMovementComponent(const TWeakObjectPtr<UProjectileMovementComponent> Value) { ProjectileMovementComponent = Value; }
	FORCEINLINE void SetSphereComponent(const TWeakObjectPtr<USphereComponent> Value) { SphereComponent = Value; }

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Physics", meta = (AllowPrivateAccess = true))
	TWeakObjectPtr<USphereComponent> SphereComponent;

};
