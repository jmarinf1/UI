// 
// PlayerCharacter.h
// 
// Main character, which is the character the player will control and posses throughout the game. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "InputTriggers.h"
#include "LadyUmbrella/Characters/Player/Camera/CustomCameraSystemComponent.h"
#include "LadyUmbrella/Characters/Player/PlayerStateComponent.h"
#include "LadyUmbrella/Characters/GenericCharacter.h"
#include "LadyUmbrella/Util/SaveSystem/FSaveStructPlayer.h"
#include "LadyUmbrella/Util/DataDrivenStructs/FPlayerDataDriven.h"
#include "LadyUmbrella/Interfaces/GetWeaponInterface.h"
#include "LadyUmbrella/Util/SaveSystem/SaveInterface.h"
#include "Logging/LogMacros.h"
#include "Containers/Map.h"
#include "PlayerCharacter.generated.h"


//IA Delegate Declaration
DECLARE_DELEGATE(FOnPlayerShoot);

#pragma region FORWARD_DECLARATIONS
class AUmbrella;
class IInteractInterface;
class UAnimMontage;
class UCineCameraComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UTimelineComponent;
class UNumericProgressBar;
class UMotionWarpingComponent;
class UAimAssistComponent;
class UFreeRoamCoverComponent;
class UInteractiveMovementComponent;
class UHitReactionComponent;

#pragma endregion

struct FInputActionValue;

UCLASS(config=Game)
class APlayerCharacter : public AGenericCharacter, public ISaveInterface, public IGetWeaponInterface
{
	GENERATED_BODY()

#pragma region PARAMETERS

#pragma region GENERAL

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "General", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AUmbrella> UmbrellaSubclass;

#pragma	endregion 
	
#pragma region COMBAT

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float MeleeRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float RecoilAnimSpeedOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float CameraLookMultiplierAimAssist;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* MeleeMovementCurve;

#pragma endregion

#pragma region INPUT

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	FString PlayerDevice;

#pragma endregion 

#pragma region MOVEMENT

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement", meta = (AllowPrivateAccess = "true"))
	float MaxWalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement", meta = (AllowPrivateAccess = "true"))
	float StrafeWalkSpeed;

#pragma endregion

#pragma region DATA DRIVEN

	UPROPERTY(EditAnywhere, Blueprintable, Category = "Data Driven", DisplayName = "Name of the Row", meta = (AllowPrivateAccess = "true"))
	FName RowName;

#pragma endregion 
	
#pragma endregion

#pragma region COMPONENTS
private:
#pragma region PLAYER STATE

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UPlayerStateComponent* PlayerStateComponent;

#pragma	endregion 
	
#pragma region CAMERA

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UCustomCameraSystemComponent* CameraSystem;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UCineCameraComponent* FollowCamera;

	UPROPERTY(VisibleInstanceOnly)
	USecondOrder1DSystem* InCoverCameraHorizontalAdditionalRatioSmooth;

#pragma endregion

#pragma region COMBAT

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UTimelineComponent* MeleeTimeLine;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UAimAssistComponent* AimAssistComponent;

#pragma endregion

#pragma region MOVEMENT

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UFreeRoamCoverComponent* FreeRoamCoverComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UMotionWarpingComponent* MotionWarpingComponent;

#pragma endregion 

#pragma region UI
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UHitReactionComponent* HitReactionComponent;
#pragma endregion

#pragma endregion
	
#pragma region VARIABLES
private:
#pragma region GENERAL

	/**********************/
	/*       GENERAL      */
	/**********************/

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "General", meta = (AllowPrivateAccess = "true"))
	FSaveStructPlayer PlayerSaveStruct;

	UPROPERTY()
	AUmbrella* Umbrella; // Umbrella attached to the player

	UPROPERTY()
	bool bGodmode;

	UPROPERTY()
	bool bNoClip;

#pragma endregion
	
#pragma region ANIMATIONS
	
	/****************************/
	/*        ANIMATIONS        */
	/****************************/

	const float GrappleMontagePlayRate = 1.5f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	UAnimMontage* MeleeMontage;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	UAnimMontage* RecoilMontage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	UAnimMontage* GrapplePullMontage;

#pragma endregion

#pragma region CAMERA
	/****************************/
	/*          CAMERA          */
	/****************************/

	UPROPERTY()
	bool bIsAimingButtonPressed;
	
	UPROPERTY()
	float InCoverLateralOffsetInitialValue;

	UPROPERTY()
	bool bResetOnCover;
	
#pragma endregion

#pragma region COMBAT

	/****************************/
	/*          COMBAT          */
	/****************************/

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bIsInCombatVolume;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bIsMeleeAttacking;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	FRotator TargetMeleeRot;
	
#pragma endregion
	
#pragma region INPUT
	/****************************/
	/*          INPUT           */
	/****************************/

	UPROPERTY()
	bool bIsManuallyAiming;
	
#pragma endregion 
	
#pragma region MOVEMENT
	/****************************/
	/*         MOVEMENT         */
	/****************************/
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Interactive Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsGrappling;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Interactive Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsSwinging;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Interactive Movement", meta = (AllowPrivateAccess = "true"))
	FTimerHandle MovementTimerHandleWhileGrappling;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Interactive Movement", meta = (AllowPrivateAccess = "true"))
	FTimerHandle TimerHandle_GrapplingDelay;
	
#pragma endregion

#pragma region IA
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IA", meta = (AllowPrivateAccess = "true"))
	float FleeRadius;
		
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IA", meta = (AllowPrivateAccess = "true"))
	float CloseRadius;
	
#pragma endregion

#pragma region DATA_DRIVEN

	/**
	 * Pointer to the DataTable that has all Data
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Data Driven", DisplayName = "Player Table Info", meta = (AllowPrivateAccess = "true"))
	UDataTable* DataTablePtr;

	/**
	 * Element to control Rows from DataTablePtr Type
	 */
	UPROPERTY(VisibleInstanceOnly, Category = "Data Driven", DisplayName = "Row Type", meta = (RowType="CH_Francesca", AllowPrivateAccess = "true"))
	FDataTableRowHandle DataTableRowHandler;

	/**
	* Struct that represents the data that there is inside the DataTablePtr. Not needed for now, but will come in handy
	*/
	UPROPERTY(VisibleInstanceOnly, Category = "Data Driven", DisplayName = "Player Struct")
	FPlayerDataDriven DataDrivenStruct;
	
#pragma endregion

#pragma region DEATH AND RESPAWN

	UPROPERTY()
	FTimerHandle RespawnTimerHandle;

#pragma endregion 

#pragma endregion

#pragma region DELEGATES
public:
	FOnPlayerShoot OnPlayerShot;

#pragma endregion 

	
#pragma region FUNCTIONS

#pragma region BASIC FUNCTIONS

public:
	APlayerCharacter();
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void NotifyControllerChanged() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

#pragma endregion

#pragma region PLAYER STATE
public:
	/**
	 * Returns the PlayerState Component from the Player.
	 * @return  
	 */
	UFUNCTION(BlueprintCallable)
	UPlayerStateComponent* GetPlayerStateComponent() const ;
	
	/**
	 * Returns the current active Player State from the Player State Component.
	 * @return 
	 */
	UFUNCTION(BlueprintCallable)
	EPlayerState GetPlayerGeneralState() const;
	
	/**
	 * Checks if the Player State can transition to the New PlayerState and, if possible, performs the change.
	 * @param NewPlayerState The change that we want to transition into.
	 * @return Whether the change was performed. If we are already at the desired PlayerState, will return false.
	 */
	UFUNCTION(BlueprintCallable)
	bool SetPlayerGeneralState(EPlayerState NewPlayerState);
	
	/**
	 * Checks if the PlayerState can be transitioned to the given New PlayerState
	 * @param NewPlayerState The PlayerState whose transition to we are testing.
	 * @return Whether the transition can be performed. If the New PlayerState is the same as our Current PlayerState, will return false.
	 */
	UFUNCTION(BlueprintCallable)
	bool CanChangeToPlayerState(EPlayerState NewPlayerState);

#pragma endregion 

#pragma region DEATH AND RESPAWN
public:
	UFUNCTION(BlueprintCallable, Category = "DeathManagement")
	void KillPlayer();

protected:
	virtual void SetDead() override;

private:
	UFUNCTION()
	void Respawn();

#pragma endregion 

#pragma region COVERS
public:
	UFUNCTION(BlueprintCallable)
	void ToggleCover();

	UFUNCTION()
	void TryToMoveBetweenCovers();
	
	UFUNCTION()
	void TurnCoverUIOn();

	UFUNCTION()
	void TurnCoverUIOff();

#pragma endregion
	
#pragma region HEALTH INTERFACE DECLARATIONS
public:
	virtual void SubtractHealth(const float HealthToSubtract, EWeaponType DamageType, FVector LastDamageSourceLocation) override;

#pragma endregion

#pragma region THIRD PERSON SHOOTING
public:
	
	/**
	 * The function that we bind to input to manage aiming.
	 * This helps us differentiate when we are aiming / stop aiming via input vs via logic.
	 */
	UFUNCTION()
	void StartedHoldingAimButton();

	/**
	* The function that we bind to input to manage when we stop aiming.
	* This helps us differentiate when we are aiming / stop aiming via input vs via logic.
	*/
	UFUNCTION()
	void ReleasedAimButton();
	
	UFUNCTION(BlueprintCallable)
	void StartAiming();
	
	UFUNCTION(BlueprintCallable)
	void ResetAiming();

	UFUNCTION()
	void ShootingUmbrella();

	UFUNCTION()
	void Reload();
	
private:
	UFUNCTION()
	void Shoot();

#pragma endregion

#pragma region MELEE COMBAT
private:
	UFUNCTION()
	void MeleeTimelineProgress(float Value); // Binded via text, be careful if no uses appear.
	
	UFUNCTION()
	void StartMeleeAttack();
	
	UFUNCTION()
	FVector CheckEnemyInMeleeRange();

#pragma endregion 

#pragma region MOVEMENT
public:
	UFUNCTION()
	void Move(const FInputActionValue& Value);
	
	virtual void Jump() override;
	
	UFUNCTION()
	void MovePlayerViaGrapple(const FVector& EndPosition);

	UFUNCTION()
	void StopMovingPlayer(const FInputActionValue& Value);

private:
	UFUNCTION()
	void CheckMantle();
	
	UFUNCTION()
	bool CheckVault();

	UFUNCTION()
	bool CheckSwing();

#pragma endregion

#pragma region GADGETS
public:
	UFUNCTION()
	void GadgetShoot();

	UFUNCTION()
	void CycleUmbrellaBullets();

	// GRAPPLE
	UFUNCTION()
	void Grappling(const FVector& EndPosition);
	
	UFUNCTION()
	void PlayGrappleAnim() const;
	
	bool CanSwapGadget() const;

	UFUNCTION()
	void SwapToHook();
	
	UFUNCTION()
	void SwapToElectric();

#pragma endregion

#pragma region INPUT
private:
	/**
	 * Manages the change in input by the player. Include this function in all Input Actions.
	 */
	UFUNCTION(BlueprintCallable, Category = PlayerDevice)
	void SetPlayerDevice();
	
#pragma endregion 

#pragma region SHIELD
public:
	UFUNCTION(BlueprintCallable)
	void ToggleShield();
	
	UFUNCTION(BlueprintCallable)
	void HoldShield();

	UFUNCTION()
	void StopShield();

#pragma endregion 

#pragma region CAMERA
public:
	UFUNCTION()
	void Look(const FInputActionValue& Value);

	UFUNCTION()
	void StopLook(const FInputActionValue& Value);
	
	UFUNCTION()
	FORCEINLINE bool GetResetOnCover() const { return bResetOnCover; }

#pragma endregion

#pragma region COLLISIONS

	UFUNCTION()
	void GetHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
#pragma endregion

#pragma region DATA DRIVEN
private:
	/**
	 * Called when wanted to set Player Attributes from DataTablePtr
	 */
	UFUNCTION()
	void LoadPlayerFromDataTable();

#pragma endregion 

#pragma region GETTERS AND SETTERS
public:
	// UMBRELLA
	UFUNCTION(BlueprintCallable)
	AUmbrella* GetUmbrella() const;

	// COMPONENTS
	UFUNCTION()
	FORCEINLINE UInteractiveMovementComponent* GetInteractiveMovementComponent() const { return InteractiveMovementComponent; };

	UFUNCTION()
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UFUNCTION()
	FORCEINLINE UCustomCameraSystemComponent* GetCameraSystem() const { return CameraSystem; }

	UFUNCTION()
	FORCEINLINE USecondOrder1DSystem* GetInCoverCameraHorizontalAdditionalRatioSmooth() const { return InCoverCameraHorizontalAdditionalRatioSmooth; };

	// VARIABLES
	FORCEINLINE bool IsAimButtonHeld() const { return bIsAimingButtonPressed; };
	FORCEINLINE bool IsGrappling() const { return bIsGrappling; }
	FORCEINLINE bool IsInCombatVolume() const { return bIsInCombatVolume; }
	FORCEINLINE bool IsSwinging() const { return bIsSwinging; }
	FORCEINLINE FString GetPlayerDevice() const { return PlayerDevice; }
	FORCEINLINE bool IsGodmode() const { return bGodmode; }
	FORCEINLINE bool IsNoClip() const { return bNoClip; }
	
	UFUNCTION(BlueprintCallable)
	bool IsMeleeAttacking() const;

	FORCEINLINE void SetIsGrappling(const bool bValue) { bIsGrappling = bValue; }
	FORCEINLINE void SetIsSwinging(const bool bValue) { bIsSwinging = bValue; }
	FORCEINLINE void SetIsInCombatVolume(const bool bValue) { bIsInCombatVolume = bValue; }
	FORCEINLINE void SetCameraLookMultiplierAimAssist(const float NewLookMultiplier) { CameraLookMultiplierAimAssist = NewLookMultiplier; }
	FORCEINLINE void SetIsMeleeAttacking(const bool bValue) { bIsMeleeAttacking = bValue; }
	FORCEINLINE void SetGodmode(const bool bValue) { bGodmode = bValue; }
	FORCEINLINE void SetNoClip(const bool bValue) { bNoClip = bValue; }
	
#pragma endregion

#pragma region SAVE INTERFACE
public:
	virtual void LoadGame() override;
	virtual void SaveGame() override;
	virtual void SaveDataCheckPoint() override;
	virtual void LoadDataCheckPoint() override;
	
#pragma endregion

#pragma endregion
	
	UFUNCTION(BlueprintCallable)
	void InteractingWithOverlappingElements(const FInputActionValue& Value);

#pragma region UPGRADES
	// TODO: organise this better
private:
	UPROPERTY()
	int CurrentPiecesForUpgrades;
	
public:
	UFUNCTION()
	FORCEINLINE void SetNewCurrentPiecesForUpgrades(const int NewPieces) { CurrentPiecesForUpgrades = NewPieces; };

	UFUNCTION()
	FORCEINLINE int GetCurrentPiecesForUpgrades() const { return CurrentPiecesForUpgrades; };
	
#pragma endregion

#pragma region GET WEAPON INTERFACE FUNCTIONS
	virtual AUmbrella* GetWeapon() const override { return Umbrella; }
#pragma endregion
};

