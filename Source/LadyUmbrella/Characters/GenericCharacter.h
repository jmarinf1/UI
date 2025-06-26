// 
// CoreCharacter.h
// 
// Generic character which is used as the common parent class of all characters in the game. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LadyUmbrella/Interfaces/HealthInterface.h"
#include "LadyUmbrella/Util/SecondOrderSystems/SecondOrder1DSystem.h"
#include "LadyUmbrella/Environment/CoverSystem/CharacterCoverMovementComponent.h"
#include "LadyUmbrella/Weapons/WeaponTypes.h"
#include "GenericCharacter.generated.h"

class AMainController;
struct FLocalizedStringRow;
enum class EActionCode : uint8;
enum class ECoverHeightState : uint8;
//delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeathChanged, AGenericCharacter*, Character);

class UCoverSpline;
class UHealthComponent;
class UInteractiveMovementComponent;
class UNiagaraSystem;
class UNiagaraComponent;

UCLASS()
class LADYUMBRELLA_API AGenericCharacter : public ACharacter, public IHealthInterface
{
	GENERATED_BODY()

protected:
	
	UPROPERTY()
	TObjectPtr<UDataTable> LocalizedStrings; 

	UPROPERTY()
	TWeakObjectPtr<AMainController> MainController;
	
public:
	AGenericCharacter();

#pragma region VARIABLES
	
#pragma region PARAMETERS
	
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnDeathChanged OnDeathChanged;

	UPROPERTY(EditDefaultsOnly, Category="HP Configuration")
	bool bIsDead = false;

	UPROPERTY(VisibleAnywhere,Category="HP Configuration")
	EWeaponType LastDamageType;

#pragma endregion

#pragma region CONSTANTS
public:
	const FVector INVALID_IN_COVER_AIMING_DIRECTION = FVector::Zero();
	
#pragma endregion

#pragma region MOVEMENT VARIABLES
private:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement", meta = (AllowPrivateAccess = "true"))
	bool bInZipline;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsStrafing;
	
#pragma endregion
	
#pragma region COMBAT VARIABLES

protected:
	UPROPERTY(VisibleAnywhere, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bIsAiming;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float AimOffsetPitchVal;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	// bool bUseRiotShield;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = true))
	bool bIsElectrified;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = true))
	bool bIsSmoked;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = true))
	bool bIsStunned;

	UPROPERTY()
	FTimerHandle ElectrifiedResetTimer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat")
	float ElectrifiedDuration;
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UNiagaraSystem* ElectricFXTemplate;
	UPROPERTY()
	UNiagaraComponent* ActiveElectricFX;

	UPROPERTY()
	FTimerHandle StunTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	EWeaponType WeaponType;
	
#pragma endregion

#pragma region COVER VARIABLES
private:
	UPROPERTY()
	FVector InCoverAimingDirection;
public:
	// Purely for animation purposes of the free roam cover component
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = true))
	bool IsInFreeRoamCover;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation",  meta = (AllowPrivateAccess = true))
	ECoverHeightState CoverHeightState;
private:
#pragma endregion

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General", meta = (AllowPrivateAccess = "true"))
	FString CharacterName;
	
#pragma endregion

	
#pragma region COMPONENTS
	
#pragma region COVER COMPONENTS
	
protected:
	UPROPERTY(EditDefaultsOnly)
	USecondOrder1DSystem* CoverTraversingSpeedSystem;
	
	UPROPERTY(EditDefaultsOnly)
	USecondOrder1DSystem* CoverOrientationSystem;

	UPROPERTY(EditDefaultsOnly)
	UCharacterCoverMovementComponent* CoverMovementComponent;

#pragma endregion

#pragma region OTHER COMPONENTS
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UHealthComponent* HealthComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInteractiveMovementComponent* InteractiveMovementComponent;

#pragma endregion

#pragma endregion
	

#pragma region FUNCTIONS

#pragma region COVER FUNCTIONS

public:
	UFUNCTION()
	bool NotifyBeginTravellingToCover(UCoverSpline* CoverToEnter, float SectionToEnter);

	UFUNCTION()
	void NotifyReachedCoverTravellingTowards();

	UFUNCTION()
	void NotifyExitCover();

	UFUNCTION()
	FORCEINLINE USecondOrder1DSystem* GetCoverTraversingSpeedSystem() const { return CoverTraversingSpeedSystem; }

	UFUNCTION()
	FORCEINLINE USecondOrder1DSystem* GetCoverOrientationSystem() const { return CoverOrientationSystem; }

	UFUNCTION()
	FORCEINLINE UCharacterCoverMovementComponent* GetCoverMovementComponent() const { return CoverMovementComponent; }

	UFUNCTION(BlueprintCallable)
	bool IsMovementControlledByCover() const;

	UFUNCTION(BlueprintCallable)
	float GetInCoverOrientation() const;
	
	UFUNCTION(BlueprintCallable)
	float GetInCoverSpeed() const;

	UFUNCTION(BlueprintCallable)
	FVector GetInCoverAimingDirection() const;

	UFUNCTION()
	FORCEINLINE void SetInCoverAimingDirection(const FVector& _NewAimingDirection) { InCoverAimingDirection = _NewAimingDirection; }

	UFUNCTION(BlueprintCallable)
	bool IsInTallCover() const;

#pragma endregion 

#pragma region COMBAT FUNCTIONS
public:
	UFUNCTION(BlueprintCallable)
	bool IsAiming() const;

	UFUNCTION(BlueprintCallable)
	bool SetIsAiming(const bool IsAiming);

	UFUNCTION(BlueprintCallable)
	bool IsStrafing() const;

	FORCEINLINE void SetIsStrafing(const bool bValue) { bIsStrafing = bValue; }

	UFUNCTION(BlueprintCallable)
	bool IsInZipline() const;

	FORCEINLINE void SetInZipline(const bool bValue) { bInZipline = bValue; }
	
	UFUNCTION()
	void OnDeath();
	
	virtual void SetDead();
	
	UFUNCTION(CallInEditor, Category="HP Configuration")
	void MarkAsDead() { SetDead(); }
	
	UFUNCTION()
	void SetElectrified(bool IsElectrified);

	UFUNCTION()
	void ResetElectrified();
	
	UFUNCTION()
	virtual void ApplyStun(float Duration);
	
	UFUNCTION()
	virtual void EndStun();

	UFUNCTION()
	bool PlayMontage(UAnimMontage* Montage, float PlayRate) const;

#pragma endregion

#pragma region VIRTUAL FUNCTIONS
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

#pragma endregion

#pragma region HEALTH INTERFACE FUNCTIONS DEFINITION
public:
	virtual void SubtractHealth(const float HealthToSubtract,enum EWeaponType DamageType) override;
	virtual void SetHealth(const float NewHealth) override;
	virtual const float GetHealth() const override;
	
#pragma endregion

public:
	UFUNCTION()
	FORCEINLINE UHealthComponent* GetHealthComponent() const { return HealthComponent; }
	
	void SpeakRandomActionLine(const EActionCode& ActionCode) const;
	const FString GetLocalizedString(TArray<FLocalizedStringRow*> Rows, const uint8 Index) const;
	
#pragma endregion
};
