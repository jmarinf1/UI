// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LadyUmbrella/Environment/CoverSystem/CharacterCoverMovementComponent.h"
#include "LadyUmbrella/Util/MathLibraries/EasingLibrary.h"
#include "PlayerCoverMovementComponent.generated.h"

/**
 * 
 */

class USecondOrder1DSystem;
class APlayerCharacter;

/**
 * Internal struct used to evaluate Cover Entry Points, don't use it outside of this script.
 */
USTRUCT()
struct FCoverEvaluation
{
	GENERATED_BODY()

	UCoverSpline* Cover;
	FVector Position;
	float Section;
	float Evaluation;
};

UCLASS()
class LADYUMBRELLA_API UPlayerCoverMovementComponent : public UCharacterCoverMovementComponent
{
	GENERATED_BODY()

	UPlayerCoverMovementComponent();


#pragma region CONSTANTS
	
	// This is the minimum angle required to distinguish a continuous movement from forward movement between covers.
	const float MIN_X_INPUT_FOR_CONTINUOUS_MOVEMENT = FMath::Cos(FMath::DegreesToRadians(35));
	const float MIN_X_INPUT_FOR_FORWARD_MOVEMENT = FMath::Cos(FMath::DegreesToRadians(85));

	// Camera
	const float MIDDLE_SCREEN_RATIO = 0.5f;

#pragma endregion
	
#pragma region PARAMETERS
private:
	/********************/
	/*  COVER MOVEMENT  */
	/********************/
	
	/**
	 * The maximum distance between the player and a CoverParent for its Entry Points to be considered when trying to enter cover.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover Movement", meta = (AllowPrivateAccess = "true", DisplayPriority = -1))
	float CoverActorsAcceptanceRadius;
	
	/**
	 * The maximum distance between the player and a Cover EntryPoint for it to be considered when trying to enter cover.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover Movement", meta = (AllowPrivateAccess = "true", DisplayPriority = -1))
	float CoverPointsAcceptanceRadius;

	/**
	 * The amount of time (in secs) a players must be trying to exit cover via input for it to happen.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover Movement", meta = (AllowPrivateAccess = "true", DisplayPriority = -1))
	float SecsNeededToExitCoverWithInput;

	/**
	 * Determines the angle maximum angle with the backwards vector allowed when trying to exit a cover via input.
	 * Only for Controller.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover Movement", meta = (AllowPrivateAccess = "true", DisplayPriority = -1))
	float MaxInputAngleBackwardsToTryToExitCoverWithController;

	/**
	* Determines the angle maximum angle with the backwards vector allowed when trying to exit a cover via input.
	* Only for Keyboard.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover Movement", meta = (AllowPrivateAccess = "true", DisplayPriority = -1))
	float MaxInputAngleBackwardsToTryToExitCoverWithKeyboard;

	/**
	 * The minimum distance the player can be from an environment element for them to be allowed to aim the gun while in cover.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover Movement", meta = (AllowPrivateAccess = "true", DisplayPriority = -1))
	float MinDistanceWithEnvironmentToAllowAim;

	
	/********************/
	/*   COVER CAMERA   */
	/********************/

	/**
	 * The maximum distance the player can take cover at without disabling their aiming.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover Camera", meta = (AllowPrivateAccess = "true", DisplayPriority = -1))
	float AimingCancellationDistanceWhenEnteringCover;
	
	/**
	 * The maximum amount of additional camera displacement applied to the camera when the player is moving along the Cover.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover Camera", meta = (AllowPrivateAccess = "true", DisplayPriority = -1))
	float MaxAdditionalCameraDisplacementWhileMoving;
	
	/**
	 * The duration that the shoulder camera change will last for.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover Camera", meta = (AllowPrivateAccess = "true", DisplayPriority = -1))
	float ShoulderChangeCameraTransitionDuration;
	
	/**
	 * The type of easing applied to the Should Change camera movement.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover Camera", meta = (AllowPrivateAccess = "true", DisplayPriority = -1))
	EEaseType ShoulderPositionChangeCurveType;
	

	/*****************/
	/*     DEBUG     */
	/*****************/
	
	/**
	 * Whether to display the Cover Movement Debug information.
	 */
	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bShowDebugInformation;

#pragma endregion
	
#pragma region VARIABLES
private:
	UPROPERTY()
	APlayerCharacter* PlayerCharacter;

	UPROPERTY()
	float SecStorageTryingToExitCoverWithInput;

	/**
	 * The Horizontal Camera Ratio value that was set up in the Camera Component.
	 */
	UPROPERTY()
	float DesiredCameraHorizontalRatioInitialValue;
	
	/**
	 * The smoothed Horizontal Camera Ratio value that gets passed to the Cover
	 */
	UPROPERTY()
	float DesiredCameraHorizontalRatio;

	UPROPERTY()
	USecondOrder1DSystem* CameraAdditionalHorizontalRatioSmoothComponent;

	UPROPERTY()
	float ChangeToLeftShoulderPercentageComplete;
	
#pragma endregion

#pragma region FUNCTIONS

	UFUNCTION()
	bool CanMoveContinuousBetweenCovers(bool GoingLeftSection0) const;

	UFUNCTION()
	bool CanMoveForwardBetweenCovers(bool GoingLeftSection0) const;

	UFUNCTION()
	void INTERNAL_DrawDebugEntryPointSphere(const UWorld* World, const FCoverEvaluation& CoverEvaluationToDraw) const;

	/**
	 * Used to calculate the minimum Y Input (in-cover) to try to exit cover with a controller.
	 * @return 
	 */
	UFUNCTION()
	float CalculateMaxYInputBackwardsToExitCoverWithController() const;

	/**
	 * Used to calculate the minimum Y Input (in-cover) to try to exit cover with a keyboard.
	 * @return 
	 */
	UFUNCTION()
	float CalculateMaxYInputBackwardsToExitCoverWithKeyboard() const;

public:
	UFUNCTION()
	bool PlayerTryToEnterCover();

	UFUNCTION()
	bool TryToMoveBetweenCovers();

#pragma endregion
	
#pragma region VIRTUAL FUNCTIONS
	
	virtual ECoveredState SetCoveredState(const ECoveredState NewCoveredState) override;

	virtual EInsideCoverState SetInsideCoverState(const EInsideCoverState NewInsideCoverState) override;

	virtual bool NotifyBeginTravellingToCover(UCoverSpline* CoverToEnter, float SectionToEnter) override;

	virtual void NotifyReachedCoverTravellingTowards() override;

	virtual void BaseInCoverTickFunctionality(float DeltaTime) override;
	
	virtual void AimInCoverTickFunctionality(float DeltaTime) override;

	virtual void OnRegister() override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual bool TryToTransitionOutOfCover() override;
	
#pragma endregion
};
