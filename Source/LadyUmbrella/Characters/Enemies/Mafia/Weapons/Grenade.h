// 
// Grenade.h
// 
// A grenade is a projectile that explodes after a short amount of time.
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#pragma once

#include "Components/SphereComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Grenade.generated.h"

class APlayerCharacter;
class UNiagaraSystem;
class USoundBase;
class UAudioComponent;
class UProjectileMovementComponent;

UCLASS()
class LADYUMBRELLA_API AGrenade : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Graphics, meta = (AllowPrivateAccess = "true"))
	UNiagaraSystem* NS_Explosion;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Physics, meta = (AllowPrivateAccess = "true"))
	UAudioComponent* ExplosionAudioComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Graphics, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Physics, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovementComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Physics, meta = (AllowPrivateAccess = "true"))
	USphereComponent* SphereComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Physics, meta = (AllowPrivateAccess = "true"))
	UCurveFloat* DamageFalloff;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Damage, meta = (AllowPrivateAccess = "true"))
	float ExplosionRange;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Damage, meta = (AllowPrivateAccess = "true"))
	float MaxDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Damage, meta = (AllowPrivateAccess = "true"))
	float MinDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Restitution, meta = (AllowPrivateAccess = "true"))
	float GrenadeRestitution;
	
	UPROPERTY()
	FTimerHandle UnusedHandle;

	UPROPERTY()
	TArray<AActor*> DamagedActors;
	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	UFUNCTION()
	void Explode();

	virtual void BeginPlay() override;

	bool ShouldDamageActor(const AActor* HitActor) const;
	bool ShouldDamagePlayer(const APlayerCharacter* PlayerCharacter) const;
	
public:
	AGrenade();
	
	UFUNCTION(BlueprintCallable)
	void Throw(const FVector Impulse);

	FORCEINLINE UNiagaraSystem* GetExplosionEffect() const { return NS_Explosion; }
	FORCEINLINE UStaticMeshComponent* GetStaticMeshComponent() const { return StaticMeshComponent; }
	FORCEINLINE UProjectileMovementComponent* GetProjectileMovementComponent() const { return ProjectileMovementComponent; }
	FORCEINLINE USphereComponent* GetSphereComponent() const { return SphereComponent; }
};
