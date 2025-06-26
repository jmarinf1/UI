// 
// GenericWeapon.h
// 
// Class from which all weapons inherit from. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LadyUmbrella/Weapons/Umbrella/BulletType.h"
#include "GenericWeapon.generated.h"

class UCapsuleComponent;
class UNiagaraSystem;
class UNiagaraComponent;
class USkeletalMeshComponent;

UCLASS()
class LADYUMBRELLA_API AGenericWeapon : public AActor
{
	GENERATED_BODY()

	/****************************/
	/*         GAMEPLAY         */
	/****************************/
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay", meta = (AllowPrivateAccess = true))
	int AmmoCurrent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay", meta = (AllowPrivateAccess = true))
	int AmmoCurrentMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay", meta = (AllowPrivateAccess = true))
	int AmmoReserve;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay", meta = (AllowPrivateAccess = true))
	int AmmoReserveMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay", meta = (AllowPrivateAccess = true))
	bool bReloading;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay", meta = (AllowPrivateAccess = true))
	float WeaponCadence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay", meta = (AllowPrivateAccess = true))
	float MaxWeaponCadence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay", meta = (AllowPrivateAccess = true))
	float MinWeaponCadence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay", meta = (AllowPrivateAccess = true))
	float WeaponDamage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay", meta = (AllowPrivateAccess = true))
	float WeaponRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = true))
	EBulletType WeaponBulletType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = true))
	float ElectricBulletCooldown;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = true))
	bool bIsElectricDardActivated;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = true))
	bool bIsElectricDardShooted;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LifeSpan", meta = (AllowPrivateAccess = true))
	float LifeSpan;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "LifeSpan", meta = (AllowPrivateAccess = true))
	float MassKg;
	/****************************/
	/*         GRAPHICS         */
	/****************************/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics", meta = (AllowPrivateAccess = true))
	TWeakObjectPtr<UNiagaraSystem> BulletNiagaraSystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics", meta = (AllowPrivateAccess = true))
	TWeakObjectPtr<UNiagaraComponent> BulletParticles;

	UPROPERTY(EditDefaultsOnly, Category = "Graphics")
	UParticleSystem* ElectricEffect;

	UPROPERTY()
	UParticleSystemComponent* ElectricEffectComponent;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category = "Graphics",meta = (AllowPrivateAccess = true))
	UCapsuleComponent* Capsule;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category = "Graphics",meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* SkeletalMeshComponent;
	
	//UPROPERTY(EditAnywhere)
	//USkeletalMesh* Mesh;
	/****************************/
	/*          SOUNDS          */
	/****************************/
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds", meta = (AllowPrivateAccess = true))
	USoundBase* FireSoundBase;
	
public:	
	AGenericWeapon();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual bool Fire();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual bool Reload(bool ConsumeAmmo);

	FORCEINLINE bool IsElectricBulletActivated() const { return bIsElectricDardActivated; }
	FORCEINLINE void SetElectricBulletActivated(bool Activated) { bIsElectricDardActivated = Activated; }
	FORCEINLINE bool IsElectricBulletShooted() const { return bIsElectricDardShooted; }
	FORCEINLINE void SetElectricBulletShooted(bool Shooted) { bIsElectricDardShooted = Shooted; }
	
	FORCEINLINE int GetAmmoCurrent() const { return AmmoCurrent; }
	FORCEINLINE int GetAmmoCurrentMax() const { return AmmoCurrentMax; }
	FORCEINLINE int GetAmmoReserve() const { return AmmoReserve; }
	FORCEINLINE int GetAmmoReserveMax() const { return AmmoReserveMax; }
	float GetRandomWeaponCadence();
	FORCEINLINE float GetWeaponCadence() const { return WeaponCadence; }
	FORCEINLINE float GetMinWeaponCadence() const { return MinWeaponCadence; }
	FORCEINLINE float GetMaxWeaponCadence() const { return MaxWeaponCadence; }
	FORCEINLINE float GetWeaponDamage() const { return WeaponDamage; }
	FORCEINLINE float GetWeaponRange() const { return WeaponRange; }
	FORCEINLINE EBulletType GetWeaponBulletType() const { return WeaponBulletType; }
	FORCEINLINE float GetElectricBulletCooldown() const { return ElectricBulletCooldown; }
	FORCEINLINE	TWeakObjectPtr<UNiagaraSystem> GetBulletNiagaraSystem() const { return BulletNiagaraSystem; }
	FORCEINLINE TWeakObjectPtr<UNiagaraComponent> GetBulletParticles() const { return BulletParticles; }
	FORCEINLINE TWeakObjectPtr<USkeletalMeshComponent> GetSkeletalMeshComponent() const { return SkeletalMeshComponent; }

	FORCEINLINE void SetAmmoCurrent(const int Value) { AmmoCurrent = Value; }
	FORCEINLINE void SetAmmoCurrentMax(const int Value) { AmmoCurrentMax = Value; }
	FORCEINLINE void SetAmmoReserve(const int Value) { AmmoReserve = Value; }
	FORCEINLINE void SetAmmoReserveMax(const int Value) { AmmoReserveMax = Value; }
	FORCEINLINE void SetWeaponCadence(const float Value) { WeaponCadence = Value; }
	FORCEINLINE void SetWeaponDamage(const float Value) { WeaponDamage = Value; }
	FORCEINLINE void SetWeaponRange(const float Value) { WeaponRange = Value; }
	FORCEINLINE void SetWeaponBulletType(const EBulletType Value) { WeaponBulletType = Value;}
	FORCEINLINE	void SetBulletNiagaraSystem(const TWeakObjectPtr<UNiagaraSystem> Value) { BulletNiagaraSystem = Value; }
	FORCEINLINE void SetBulletParticles(const TWeakObjectPtr<UNiagaraComponent> Value) { BulletParticles = Value; }
	//FORCEINLINE void SetSkeletalMeshComponent(const TWeakObjectPtr<USkeletalMeshComponent> Value) { SkeletalMeshComponent = Value; }

	void SimulatePhysicsBeforeDestroy();

	void SpawnElectricEmmiter();
	void StopElectricEmmiter();
protected:	
	virtual void BeginPlay() override;
	virtual void Tick(const float DeltaTime) override;

private:
	bool CanReload() const;
};
