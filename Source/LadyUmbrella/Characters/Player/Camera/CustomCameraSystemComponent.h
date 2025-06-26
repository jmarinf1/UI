//  
// CameraSystemComponent.h
// 
// Camera Pitch & Yaw player input.
// Management of Camera Collisions and Adjustments.
// Management of camera States (switching and behaviours). 
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "Curves/CurveEvaluation.h"
#include "LadyUmbrella/Characters/Player/Camera/CameraComponentTypes/FCameraState.h"
#include "Runtime/CinematicCamera/Public/CineCameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "CustomCameraSystemComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LADYUMBRELLA_API UCustomCameraSystemComponent : public UActorComponent
{
	GENERATED_BODY()
	
#pragma region ATTRIBUTES
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	bool bResetOnCover; // Toggle if the camera position should reset on cover.
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true", ClampMin = 0.01f, ClampMax = 10.f, UIMin = 0.01f, UIMax = 10.f))
	float ResetStepSize; // Step per second for camera reset rotation (the higher, the faster the camera resets).

#pragma region COMPONENTS
	
	UPROPERTY()
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY()
	TObjectPtr<UCineCameraComponent> FollowCamera;

#pragma endregion 
		
#pragma region SENSIBILITY
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly , Category = "Camera Sensibility", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCurveFloat> ManualYawCurve; // Curve that describes the movement of the camera yaw.

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera Sensibility", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCurveFloat> ManualPitchCurve; // Curve that describes the movement of the camera pitch.
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera Sensibility", meta = (AllowPrivateAccess = "true", NoResetToDefault))
	TMap<FName, FCameraMultipliers> CameraMultipliers;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera Sensibility", meta = (AllowPrivateAccess = "true", NoResetToDefault, ClampMin = "-90.0", ClampMax = "90.0", UIMin = "-90.0", UMax = "90.0"))
	FVector2D ViewPitchRange; 
	
#pragma endregion 
	
#pragma region STATES
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera States", meta = (AllowPrivateAccess = "true"))
	TMap<FCameraStateKey, FCameraState> CameraStates; // Map for all camera (mutually exclusive) states. For a combined state, manage it outside.

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera States", meta = (AllowPrivateAccess = "true"))
	float RelativeLowCoverCollisioneHeightOffset;
	
#pragma endregion 

#pragma region COLLISION 
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Collisions", meta = (AllowPrivateAccess = "true", ClampMin = 0.f, ClampMax = 100.0f, UIMin = 0.f, UIMax = 100.0f))
	float CameraCollisionRadius; // Size of radius around the camera for automatic Camera Boom correction.

#pragma endregion 
	
#pragma region YAW_ASSISTANCE
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Yaw Assistance", meta = (EditConditionHides, AllowPrivateAccess = "true"))
	bool bYawAssistance; // Time without input until sideways movement affects camera rotation.

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Yaw Assistance", meta = (EditCondition = "bYawAssistance", InlineEditCondition, AllowPrivateAccess = "true", ClampMin = 0.f, ClampMax = 1.0f, UIMin = 0.f, UIMax = 1.0f))
	float CameraMoveAssistStrength; // How aggressive the camera correction is when following the movement input.

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Yaw Assistance", meta = (EditCondition = "bYawAssistance", AllowPrivateAccess = "true", Units = "Seconds"))
	float YawAssistanceDelay; // Time without input until sideways movement affects camera rotation.

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Yaw Assistance", meta = (EditCondition = "bYawAssistance", AllowPrivateAccess = "true", ClampMin = 0.f, ClampMax = 100.0f, UIMin = 0.f, UIMax = 100.0f, Units = "Times"))
	float CameraSideMultiplier; // How aggressive should the camera correction is when correcting on side collision.
	
#pragma endregion 
	
#pragma region SHAKES

	/**
	 * Camera shakes associated with the following events:
	 *
	 * [PLAYER] Actions taken by the player, particularly for shooting;
	 * [ENVIRONMENT] Elements from the environment such as explosive barrels and falling debris;
	 * [UNKNOWN] Any shake provoked by an unknown source such as an earthquake.
	 *
	 * _(Subject to change)_
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera Shakes", meta = (AllowPrivateAccess = "true"))
	TMap<FShakeSource, TSubclassOf<class UCameraShakeBase>> CameraShakes;

#pragma endregion 

#pragma region INTERPOLATION

	float InterpTimer;
	float InterpolationStart_Dist;
	FVector InterpolationStart_Offset;
	
#pragma endregion
	
#pragma region CONSTANTS

	const float TwoPIThirds = (2.f * PI) / 3.f;

	const float MaxFocusDist = 30000.f; // In centimeters

#pragma endregion 
	
	TArray<FHitResult> CameraHits;
	TArray<TObjectPtr<AActor>> IgnoredActorsCamera;
	FCameraStateKey CurrentState;

	float TargetBoomLength;
	FVector TargetOffset;
	FVector CurrentOffset;
	
	float CurrentTargetYaw;
	float YawAssistanceTimer;
	float DelayedCollisionTimer;
	
	bool bCameraHasMotion;
	bool bCameraIsResetting;

#pragma endregion 

public:

	UCustomCameraSystemComponent();
	
	void CameraTick(const float& DeltaSeconds);
	void SetUpCamera(USpringArmComponent* Arm, UCineCameraComponent* Cam); // Set up system with appropriate references.
	void ResetCameraParams(); // Used to reset the camera system to its starting values
	
	UFUNCTION(BlueprintCallable, Category = "Camera")
	void ResetCameraPosition(); // Resets camera behind player.
		
private:
	
	virtual void BeginPlay() override;
	
	void UpdateCameraPosition(const float& DeltaSeconds);
	void ResetCamProgress(const double& DeltaSeconds);

#pragma region SENSIBILITY
public:
	
	/** Updates Camera current controller Sensibility */
	UFUNCTION(BlueprintCallable, Category = "Camera Sensibility")
	void UpdateCurrentControlSensitivity(FString DeviceType = "KBM", float NewValue = 1.f);
	
	/** Updates a single Camera Control Modifier for a specific device */
	UFUNCTION(BlueprintCallable, Category = "Camera Sensibility")
	void UpdateModifier(EModifierType Modifier, TArray<float> NewValue);
	/** Updates a single Camera Control Modifier for a specific device */
	void UpdateModifier(FName DeviceType, EModifierType Modifier, float NewValue);
	
	/** Provides an adjusted Look Vector for Player input based on the Camera Sensibility Parameters */
	UFUNCTION(BlueprintCallable, Category = "Camera Sensibility")
	FVector2D GetAdjustedLookVector(FString DeviceType, FVector2D LookAxisVector);
	
	/** Used to add rotation on movement based on the Camera Yaw Assistance Parameters */
	UFUNCTION(BlueprintCallable, Category = "Camera Sensibility")
	void MoveAssistance(FVector2D MoveAxisVector, bool bMoveControlledByCover);

#pragma endregion 
	
#pragma region STATES
	
	/** Get Current FCameraStateKey. */
	UFUNCTION(BlueprintCallable, Category = "Camera States", meta=(AllowPrivateAccess = "true"))
	FCameraStateKey GetCurrentCameraStateKey() const { return CurrentState; };
	
	/** Get Current FCameraState from states you created. */
	UFUNCTION(BlueprintCallable, Category = "Camera States", meta=(AllowPrivateAccess = "true"))
	FCameraState& GetCurrentCameraState() const { return const_cast<FCameraState&>(CameraStates[CurrentState]); };

	/** Get FCameraState based on its FCameraStateKey. */
	UFUNCTION(BlueprintCallable, Category = "Camera States", meta=(AllowPrivateAccess = "true"))
	FCameraState& GetCameraState(FCameraStateKey StateKey) const { return CameraStates.Contains(StateKey) ? const_cast<FCameraState&>(CameraStates[StateKey]) : const_cast<FCameraState&>(CameraStates[FCameraStateKey::DEFAULT]); };

	/** Set one of the states as the current one based on FCameraStateKey. */
	UFUNCTION(BlueprintCallable, Category = "Camera States")
	void SetCameraState(FCameraStateKey StateKey);

	/** Change Distance value of a state (Be Careful!). */
	UFUNCTION(BlueprintCallable, Category = "Camera States")
	void SetCameraStateDistance(FCameraStateKey StateKey, float Distance2Player);
	
	/** Invert Offset value of a state (Shoulder Change). */
	UFUNCTION(BlueprintCallable, Category = "Camera States")
	void SetCameraStateShoulder(FCameraStateKey StateKey, bool InvertedShoulder);

	/** Change Offset value of a state. */
	UFUNCTION(BlueprintCallable, Category = "Camera States")
	void SetCameraStateOffsetValue(FCameraStateKey StateKey, FVector2D Ratio);
	
	/** Change values of a state: Distance (Be Careful!) and Shoulder. */
	UFUNCTION(BlueprintCallable, Category = "Camera States")
	void SetCameraStateValues(FCameraStateKey StateKey, float Distance2Player, bool InvertedOffset);

	/** Get Offset for a certain FCameraState. */
	UFUNCTION(BlueprintCallable, Category = "Camera States")
	FVector ComputeOffset(const FCameraState& CurrentState);
	
	/** Check wether FCameraStateKey has been set up in the system*/
	UFUNCTION(BlueprintCallable, Category = "Camera States")
	bool StateExists(FCameraStateKey StateKey) const { return CameraStates.Contains(StateKey); };

private:
	
	void UpdateForCameraState(const float& DeltaSeconds); // Sets internal variables based on the current state

#pragma endregion 

#pragma region COLLISION
public:
	
	/** Call For Cases In which the player may make a large suden move/jump so that the camera sets the distance without jumping */
	UFUNCTION(BlueprintCallable, Category = "Camera Collisions")
	void SetCurrentDistanceAsArmLength();

	/** Should be called every time the player moves the camera (true) and stops doing so (false). */
	UFUNCTION(BlueprintCallable, Category = "Camera")
	void SetCameraInMotion (bool Moving) { bCameraHasMotion = Moving; if (!Moving) YawAssistanceTimer = YawAssistanceDelay; }

private:
	
	void ManageCameraCollisions(); // Check for camera collision based on the current camera state
	void EvaluateCollision(const FHitResult& CollisionResult); // Given a collision, change camera parameters
	void CameraCorrectiveMovement(const FHitResult& Hit); // Change arm length based on collision location
	FVector GetOffsetFromDistance(const float BoomDistance, const FVector TheoreticalOffset) const;
	float GetEquivalentDistanceFromCurrentOffset() const;
	void ClampLateralOffset(FVector& DesiredOffset, const FVector& HitLocation, const bool FromRight) const; // Clamp target offset based on collision location
	void ManageArmExpectedValue(const float ExpectedDistance, FVector ExpectedOffset); // Given a change in the current length, evaluate whether it should occur or not

#pragma endregion 

#pragma region SHAKES
public:
	/** Make Camera shake according to a parameter and a source */
	UFUNCTION(BlueprintCallable, Category = "Camera Shakes")
	void MakeCameraShake(const FShakeSource& Type, FVector Location = FVector(0.f, 0.f, 0.f), float RadiusFrom = 0.f, float RadiusTo = 2500.f);
	
#pragma endregion 

#pragma region INTERPOLATION
private:
	// Interpolate float from Current to Target (Next) with type: FCameraInterpolation
	float InterpolateFloat();
	
	// Ensures initial and final speed are 0 [Currently not working]
	template <class T>
	T SmoothStepLerp(T Current, T Next, T StepPercent) { return FMath::SmoothStep(Current, Next, StepPercent); }

	// Slow initial speed
	template<class T>
	T EaseInLerp(T Current, T Next, T StepPercent, T Exp) { return FMath::Lerp(Current, Next, FMath::Pow(StepPercent, Exp)); }
	
	// Slow final speed
	template<class T>
	T EaseOutLerp(T Current, T Next, T StepPercent, T Exp) { return FMath::Lerp(Current, Next, 1.f - FMath::Pow(1.f - StepPercent, Exp)); }

	template<class T>
	T ElasticEaseInLerp(T Current, T Next, T StepPercent)
	{
		float Step = StepPercent == 0.f ? 0.f : StepPercent == 1.f ? 1.f : -FMath::Pow(2.f, -10.f * StepPercent) * FMath::Sin((StepPercent * 10.f - 0.75f) * TwoPIThirds);
		return FMath::Lerp(Current, Next, Step);
	}

	template<class T>
	T ElasticEaseOutLerp(T Current, T Next, T StepPercent)
	{
		float Step = StepPercent == 0.f ? 0.f : StepPercent == 1.f ? 1.f : FMath::Pow(2.f, -10.f * StepPercent) * FMath::Sin((StepPercent * 10.f - 0.75f) * TwoPIThirds) + 1.f;
		return FMath::Lerp(Current, Next, Step);
	}
	
	template<class T>
	T ElasticEaseInOutLerp(T Current, T Next, T StepPercent)
	{
		float Step = StepPercent == 0.f ? 0.f : StepPercent == 1.f ? 1.f : StepPercent < 0.5f
		? -FMath::Pow(2.f, -10.f * StepPercent) * FMath::Sin((StepPercent * 10.f - 0.75f) * TwoPIThirds)
		: FMath::Pow(2.f, -10.f * StepPercent) * FMath::Sin((StepPercent * 10.f - 0.75f) * TwoPIThirds) + 1.f;
		return FMath::Lerp(Current, Next, Step);
	}
	
#pragma endregion

public:
	float GetFieldOfView() const;
	bool IsOutsideFrustum(const AActor* Actor, const float Tolerance) const;
	bool IsActorInFrustum(const AActor* Actor) const;
	bool IsComponentInFrustum(const UPrimitiveComponent* Actor) const;
	
	FORCEINLINE TWeakObjectPtr<USpringArmComponent> GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE TWeakObjectPtr<UCineCameraComponent> GetFollowCamera() const { return FollowCamera; }
};
