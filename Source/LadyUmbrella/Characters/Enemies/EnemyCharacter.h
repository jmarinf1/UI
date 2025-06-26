// 
// EnemyCharacter.h
// 
// Parent class for all Enemy Character in the game
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "LadyUmbrella/Characters/GenericCharacter.h"
#include "LadyUmbrella/Interfaces/HookInterface.h"
#include "LadyUmbrella/Interfaces/KickInterface.h"
#include "LadyUmbrella/Weapons/WeaponTypes.h"
#include "LadyUmbrella/UI/Basics/EnemyStateIcon.h"
#include "LadyUmbrella/Weapons/Umbrella/Umbrella.h"
#include "EnemyCharacter.generated.h"

class AElectricProjectile;
class AWayPoint;
class ASplinePath;
class AGenericWeapon;
class APlayerCharacter;
class AIController;
class UWidgetComponent;
class UEnemyStateIcon;
class UTexture2D;

DECLARE_DELEGATE(FOnElectricShootFinished)

UCLASS()
class LADYUMBRELLA_API AEnemyCharacter : public AGenericCharacter, public IHookInterface, public IKickInterface
{
	GENERATED_BODY()
public:
	UPROPERTY()
	FVector WeaponTraceEndPoint;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = true))
	UAnimMontage* ReloadMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = true))
	TSubclassOf<AGenericWeapon> WeaponBP;
	
	UPROPERTY(EditAnywhere, Category = "Weapon")
	FName socketName;
	
	UPROPERTY(VisibleAnywhere)
	int currentZone;

	UPROPERTY(VisibleAnywhere)
	int CurrentTokenArrayPosition;

	FOnElectricShootFinished OnElectricShootFinished;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple")
	float GrapplingMaxTime;
private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics", meta = (AllowPrivateAccess = true))
	UWidgetComponent* WidgetComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Graphics", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UEnemyStateIcon> EnemyStateWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Graphics", meta = (AllowPrivateAccess = "true"))
	UTexture2D* EnemyStunTexture;

	UPROPERTY(EditAnywhere, Category = "Graphics", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AElectricProjectile> ElectricProjectileClass;
	
	UPROPERTY(EditAnywhere)
	float F_LaunchSpeedFactor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = true))
	AGenericWeapon* EquippedWeapon;

	UPROPERTY(VisibleAnywhere, Category = "Weapon", meta = (AllowPrivateAccess = true))
	bool bPlayerLuckActive;

	// CoordinationManager
	UPROPERTY(EditAnywhere, Category = "CoordinationManager")
	bool bHasToken;

	UPROPERTY(EditAnywhere)
	bool bHasShield;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float StunDuration;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = true))
	TArray<UAnimMontage*> DamagedAnimMontages;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = true))
	UAnimMontage* CurrentDamageAnimMontage;
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	float ReboundParticleRange;
	
	UPROPERTY(EditAnywhere, Category = "Combat")
    bool DoesParticleRebound;
	
	

	UPROPERTY(EditAnywhere, Category = "Combat")
	float SpeedCrounching;
	
	TWeakObjectPtr<UCurveFloat> AmmoDropAmountCurve;
	TWeakObjectPtr<UCurveFloat> AmmoDropRateCurve;
	TWeakObjectPtr<UCurveFloat> DeadForceCurve;
public:
	virtual void BeginPlay() override;

	AEnemyCharacter();

	void Initialize();

	AController* GetAIController() const;
	
	ASplinePath* GetSplinePath() const;

	void SetSplinePath(ASplinePath* splinePath);

	virtual void SetDead() override;
	
	UFUNCTION(Category = "Weapon")
	void Fire();

	UFUNCTION(Category = "Weapon")
	AGenericWeapon* GetEquippedWeapon() const;

	UFUNCTION(Category = "Weapon")
	bool GetIsReloading() const;

	UFUNCTION(Category = "Weapon")
	void SetMissChance(float newMissChance);

	UFUNCTION(Category = "Weapon")
	float GetMissChance() const;

	UFUNCTION(Category = "Widgets")
	UEnemyStateIcon* GetWidget() const;
	
	void Reload();
	
	UFUNCTION()
	void OnReloadCompleted(UAnimMontage* Montage, bool bInterrupted);
	
	void SetWeaponBP(const TSubclassOf<AGenericWeapon>& NewWeaponBP);
	
	virtual AActor* GetShield() const { return nullptr; }
	
	// CoordinationManager
	void SetHasToken(const bool newTokenState);
	bool GetHasToken() const;
	
	virtual void SubtractHealth(const float _HealthToSubtract, EWeaponType DamageType) override;

	UFUNCTION(BlueprintCallable)
	void GetFireHitResult(FHitResult &OutHit, bool bMiss);

	virtual void ApplyStun(float Duration) override;
	virtual void EndStun() override;

	UFUNCTION()
	void EndGrapplingPosition();
private:
	UPROPERTY(VisibleAnywhere, Category = "Path following")
	ASplinePath* splinepath;

	UPROPERTY()
	AUmbrella* Umbrella;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	float DefaultMissChance;

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	float MissChance;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	float ForgivenessTime;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	float AimCoverForgivenessTime;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = true))
	bool bCanInteractWithReactiveElements;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = true))
	float MinLeftRightOffsetLongDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = true))
	float MaxLeftRightOffsetLongDistance;
			
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = true))
	float MinLeftRightOffsetCloseDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = true))
	float MaxLeftRightOffsetCloseDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = true))
	float MinUpOffsetLateralBullet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = true))
	float MaxUpOffsetLateralBullet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = true))
	float MinLeftRightOffsetOverhead;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = true))
	float MaxLeftRightOffsetOverhead;
			
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = true))
	float MinUpOffsetOverhead;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = true))
	float MaxUpOffsetOverhead;
	
	UPROPERTY(VisibleAnywhere)
	APlayerCharacter* PlayerRef;

	UPROPERTY()
	FTimerHandle MissChanceTimer;
	
	UPROPERTY()
	FTimerHandle CoverMissChanceTimer;

	UPROPERTY()
	FTimerHandle PlayerLuckTimer;

	UPROPERTY()
	FTimerHandle ActivatingElectricDardDelegate;

	UPROPERTY()
	float EnemyKickedImpulse;
	
	UPROPERTY(EditAnywhere, Category = "Weapon")
	UParticleSystem* ParticleExplosionSystem;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	FVector ParticleExplosionScale;
	
	//shoot trace data
	FVector ProjectileDirection;
	FVector ProjectileSpawnLocation;
	FRotator ProjectileSpawnRotation;

	void DropAmmo();
	
	bool bIsReloading;
	// Hook Interface Functions
	virtual void ContactWithCable(FVector& Direction, UPrimitiveComponent* ComponentHit) override{};
	virtual void ContactWithCable(FVector& Direction) override;
	virtual GrabType GetGrabType() override;
	
	// Kick Interface Function
	virtual void LaunchActor(FVector Direction) override;
	virtual void KnockBackActor() override;
			
	// Timer that manages the movement of the enemy towards the player when hooked
	UPROPERTY()
	FTimerHandle TimerHandle_GrapplingDelay;
	UPROPERTY()
	FTimerHandle TimerHandle_GrapplingMaxTime;
	
	UFUNCTION()
	void PlayReactMontage();

	UFUNCTION()
	void PlayKickedMontage();
	
	UFUNCTION()
	void ChangeEnemiesMissChance(bool NewState);

	UFUNCTION()
	void ModifyMissChanceDependingOnUmbrellaState(const bool IsShieldOpen);

	UFUNCTION()
	void WaitToChangeEnemiesMissChance();

	UFUNCTION()
	void LosePlayerLuck();

	UFUNCTION()
	void CheckLineofSightWithPlayer();
	
	UFUNCTION()
	void PauseAILogic(FString Cause);
	
	void StopAILogic();

protected:
	virtual void Tick(float DeltaTime) override;
private:
	UFUNCTION()
	void SetGrapplingPosition(FVector& EndPoint);
	
	UFUNCTION()
	void SpawnBulletNiagaraSystem(FVector WeaponTraceStart, FVector WeaponTraceEnd);

	UFUNCTION()
	FVector GetBulletMissedDirection(const FVector& WeaponTraceDirection, const FVector& WeaponTraceEnd);

	UFUNCTION()
	void BulletInteractionWithReactiveElements(const FHitResult& OutHit);
	
	UFUNCTION()
	void OnElectricShoot();
	
	UFUNCTION()
	void BulletInteraction(const FHitResult& OutHit);

	UFUNCTION()
	void OnDamagedMontageCompleted(UAnimMontage* Montage, bool bInterrupted);
	
	
	UFUNCTION()
	void OnKickedMontageCompleted(UAnimMontage* Montage, bool bInterrupted);
	
	UFUNCTION()
	void OnNiagaraSystemFinished(UNiagaraComponent* PSystem);
};
