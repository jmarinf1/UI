// 
// Umbrella.h
// 
// Main tool/weapon the player will use. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "Components/ArrowComponent.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Characters/HealthComponent.h"
#include "LadyUmbrella/Interfaces/HealthInterface.h"
#include "LadyUmbrella/Interfaces/UmbrellaUpgradeInterface.h"
#include "LadyUmbrella/Weapons/Umbrella/GadgetType.h"

#include "Umbrella.generated.h"

class UTimelineComponent;
class UMeleeComponent;
class UAnimSequence;
class UBoxComponent;
class UCableComponent;
class USkeletalMeshComponent;
class USoundCue;
class UWidgetComponent;
class AElectricProjectile;

DECLARE_DELEGATE_OneParam(FOnCurrentBulletsChanged, int);
DECLARE_DELEGATE_OneParam(FOnTotalBulletsChanged, int);
DECLARE_DELEGATE_ThreeParams(FOnUmbrellaStateChanged, bool, bool , bool ); /*IsAiming, IsOpen, IsOnEnemy*/
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIsCoveredByUmbrellaChanged, bool, NewState);

UCLASS()
class LADYUMBRELLA_API AUmbrella : public AActor, public IHealthInterface, public IUmbrellaUpgradeInterface
{
	GENERATED_BODY()

#pragma region COMPONENTS
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UMeleeComponent* MeleeComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UWidgetComponent* EnergyBarComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UCableComponent* CableComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UArrowComponent* Arrow;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UHealthComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere, Category = "Graphics")
	UNiagaraComponent* ElectricEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics", meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* SkeletalMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics", meta = (AllowPrivateAccess = true))
	UBoxComponent* ProtectionCollisionComponent;

	UPROPERTY(EditAnywhere)
	UUpgradeComponent* UpgradeComponent;

#pragma endregion

#pragma region PARAMETERS
private:
	/****************************/
	/*          ATTACK          */
	/****************************/
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = true))
	int AmmoCurrentMax;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = true))
	int AmmoReserveMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = true))
	float BulletDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = true))
	float BulletRange;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = true))
	UCurveFloat* DistanceDamageModifier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = "true"))
	float ReloadTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = "true"))
	float ShootingCadenceTime;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = true))
	float NotAimingBulletDispersion;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = true))
	float AimingBulletDispersion;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = true))
	float BulletPatternSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = true))
	bool bDrawDebugBulletDispersion;

	/****************************/
	/*         GADGETS          */
	/****************************/

	UPROPERTY(EditAnywhere, Category = "Gadget")
	TSubclassOf<AElectricProjectile> ElectricProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gadget", meta = (AllowPrivateAccess = true))
	float HookRange;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gadget",meta = (AllowPrivateAccess = true))
	float ElectricCooldownDuration = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gadget",meta = (AllowPrivateAccess = true))
	float GrappleCooldownDuration = 5.0f;

	/****************************/
	/*         GRAPHICS         */
	/****************************/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics", meta = (AllowPrivateAccess = true))
	UAnimSequence* CloseAnimSequence;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics", meta = (AllowPrivateAccess = true))
	UAnimSequence* OpenAnimSequence;

	UPROPERTY(EditAnywhere, Category = "Graphics")
	UParticleSystem* ImpactParticleSystem;

	UPROPERTY(EditAnywhere, Category = "Graphics")
	UNiagaraSystem* ElectricNiagaraSystemAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics", meta = (AllowPrivateAccess = true))
	UMaterial* MaterialClosed;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics", meta = (AllowPrivateAccess = true))
	UMaterialInstance* MaterialOpened;

	/****************************/
	/*          SOUND           */
	/****************************/
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SFX", meta = (AllowPrivateAccess = "true"))
	USoundCue* SCShoot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFX", meta = (AllowPrivateAccess = "true"))
	float VolumeMultiplier;

	/****************************/
	/*            UI            */
	/****************************/

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = true))
	TSubclassOf<UNumericProgressBar> UmbrellaHealthWidget;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = true))
	float WidgetTraceRadius;

	
#pragma endregion 

#pragma region VARIABLES
private:
	/****************************/
	/*       GENERAL STATE      */
	/****************************/
	
	/**
	 * Reference to the Player Pawn that holds the umbrella.
	 */
	UPROPERTY()
	APlayerCharacter* Player;
	
	/**
	 * Reference to the player's camera component.
	 */
	UPROPERTY()
	UCameraComponent* CameraComponent;

	UPROPERTY()
	bool bInfiniteAmmo;
	
	/****************************/
	/*          ATTACK          */
	/****************************/

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = "true"))
	FTimerHandle ReloadTimerHandle;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = "true"))
	bool bOnEnemy;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = "true"))
	bool bIsShooting;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = true))
	int AmmoCurrent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = true))
	int AmmoReserve;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = true))
	EGadgetType GadgetType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = true))
	int MaxNrBulletPerShot;

	UPROPERTY(VisibleAnywhere)
	TArray<FVector> SpawnPattern;

	/****************************/
	/*          DEFEND          */
	/****************************/

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Defend", meta = (AllowPrivateAccess = true))
	bool bOpen;

	/****************************/
	/*         GADGETS          */
	/****************************/

	UPROPERTY()
	FTimerHandle GrappleCooldownTimerHandle;
	
	UPROPERTY()
	FTimerHandle ElectricCooldownTimerHandle;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gadget", meta = (AllowPrivateAccess = true))
	bool bCanShootGrapple;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gadget", meta = (AllowPrivateAccess = true))
	bool bIsGrapplingCharacter;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gadget", meta = (AllowPrivateAccess = true))
	bool bCanShootElectric;

#pragma endregion

#pragma region  DELEGATES
public:
	FOnCurrentBulletsChanged OnCurrentBulletsChanged;
	FOnTotalBulletsChanged OnTotalBulletsChanged;
	FOnUmbrellaStateChanged OnUmbrellaStateChanged;
	FOnIsCoveredByUmbrellaChanged OnIsCoveredByUmbrellaChanged;

#pragma endregion 

#pragma region FUNCTIONS

#pragma region BASIC FUNCTIONS
public:
	AUmbrella();
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
#pragma endregion

#pragma region PRIVATE FUNCTIONS
private:
	UFUNCTION()
	void RecalculateBulletSpreadPositions();

	UFUNCTION()
	FORCEINLINE void ResetElectricCooldown() { bCanShootElectric = true; };

	UFUNCTION()
	FORCEINLINE void ResetGrappleCooldown() { bCanShootGrapple = true; };

	UFUNCTION()
	FORCEINLINE void ResetShooting() { bIsShooting = false; };
	
	UFUNCTION()
	bool ShootingUmbrellaGrapple();

	UFUNCTION()
	TArray<FHitResult> PerformSphereTrace(float TraceRadius, float TraceLength);

	UFUNCTION()
	void UmbrellaHealthChangedUpdate(int32 NewHealth, bool bLostHealth);
	
#pragma endregion

#pragma region PUBLIC FUNCTIONS
public:
	UFUNCTION()
	FORCEINLINE void SetVisibility(bool Visibility) const { SkeletalMeshComponent->SetVisibility(Visibility); };

	UFUNCTION()
	void Open();
	
	UFUNCTION()
	void Close();
	
	UFUNCTION(BlueprintCallable, Category = "Attacks")
	void ToggleOpenClose();
	
	/**
	 * Performs the logic when the player tries to shoot. If no bullets are stored, this will trigger an automatic reload.
	 * @return True if the umbrella managed to shoot bullets.
	 */
	UFUNCTION(BlueprintCallable, Category = "Attacks")
	bool TryToShoot();
	
	UFUNCTION(BlueprintCallable, Category = "Attacks")
	bool GadgetShoot();
	
	UFUNCTION(BlueprintCallable, Category = "Attacks")
	bool OnElectricShoot();
	
	UFUNCTION(BlueprintCallable, Category = "Attacks")
	void CycleBulletType();
	
	UFUNCTION(BlueprintCallable, Category = "Attacks")
	void SetBulletType(EGadgetType TargetGadget);

	UFUNCTION()
	void ResetGrapple();

	UFUNCTION()
	void SetCableEndLocation(const FVector& EndPoint) const;

	UFUNCTION()
	void StartReloading();
	
	UFUNCTION()
	void ReloadOneShell();
	
	UFUNCTION()
	void AutoReload();

	FORCEINLINE bool IsAmmoReserveAtMaxValue() const { return AmmoReserve >= AmmoReserveMax; };
	FORCEINLINE bool IsAiming() const { return Player->IsAiming(); };
	FORCEINLINE bool IsOpen() const { return bOpen; }
	FORCEINLINE bool IsOnEnemy() const {return bOnEnemy; }
	FORCEINLINE bool IsInfiniteAmmo() const {return bInfiniteAmmo; }

#pragma endregion

#pragma region STATIC FUNCTIONS

	UFUNCTION()
	static bool IsEnemyInHit(const TArray<FHitResult>& HitResults);

#pragma endregion

#pragma region HEALTH INTERFACE
public:
	FORCEINLINE virtual void SubtractHealth(const float HealthToSubtract,EWeaponType DamageType) override
	{ 	HealthComponent->SubtractHealth(HealthToSubtract); };
	FORCEINLINE virtual void SetHealth(const float NewHealth) override { HealthComponent->SetHealth(NewHealth); };
	FORCEINLINE virtual const float GetHealth() const override { return HealthComponent->GetHealth(); };

#pragma endregion

#pragma region GETTERS AND SETTERS
public:
	// Setters
	// FORCEINLINE	void SetAmmoCurrent(const int32 Amount) { AmmoCurrent = Amount; }
	// FORCEINLINE	void SetAmmoReserve(const int32 Amount) { AmmoReserve = Amount; }
	void SetAmmoCurrent(const int32 Amount);
	void SetAmmoReserve(const int32 Amount);
	FORCEINLINE void SetInfiniteAmmo(const bool bValue) { bInfiniteAmmo = bValue; }

	// Getters
	FORCEINLINE int32 GetAmmoCurrent() const { return AmmoCurrent; }
	FORCEINLINE int32 GetAmmoReserve() const { return AmmoReserve; }
	FORCEINLINE int32 GetAmmoCurrentMax() const { return AmmoCurrentMax; }
	FORCEINLINE int32 GetAmmoReserveMax() const { return AmmoReserveMax; }
	FORCEINLINE float GetShootingCadence() const { return ShootingCadenceTime; }
	FORCEINLINE virtual UUpgradeComponent* GetUmbrellaUpgradeComponent() override {	return UpgradeComponent; };
	FORCEINLINE UUpgradeComponent* GetUpgradeComponent() const { return UpgradeComponent; }
	FORCEINLINE UCableComponent* GetCableComponent() const { return CableComponent; }
	FORCEINLINE UWidgetComponent* GetEnergyBarComponent() const { return EnergyBarComponent; }
	FORCEINLINE UMeleeComponent* GetMeleeComponent() const { return MeleeComponent; }

#pragma endregion 

#pragma endregion 

	/**********************************************************/
	/*            To be moved to grapple component            */
	/**********************************************************/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grapple", meta = (AllowPrivateAccess = "true"))
	UTimelineComponent* GrappleStartTimeline;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grapple", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* GrappleStartCurve;

	UPROPERTY()
	FVector GrappleEndPoint;

	UPROPERTY()
	AActor* GrappleHitActor;

	UPROPERTY()
	UPrimitiveComponent* GrappleHitComponent;
	
	UFUNCTION()
	void GrappleStartTimelineProgress(float Value);

	UFUNCTION()
	void GrappleStartTimelineFinish();
};
