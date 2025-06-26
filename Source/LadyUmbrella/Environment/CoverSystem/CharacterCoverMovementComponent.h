// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CoverEnums.h"
#include "CharacterCoverMovementComponent.generated.h"

class UCoverSpline;
class AGenericCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIsCoveredChanged, bool, newState);
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )

class LADYUMBRELLA_API UCharacterCoverMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCharacterCoverMovementComponent();

#pragma region CONSTANTS
	
	const float MAX_DISTANCE_FROM_EDGE_TO_PERFORM_EXIT_TRANSITION = 100.0;
	const float MIN_X_INPUT_FOR_COVER_REACTION = 0.15f;
	const float EXIT_COVER_TRANSITION_DURATION = 0.5f;
	const float INVALID_PERCENTAGE_COMPLETE_VALUE = -1.0f;
	const float ZERO_TRAVERSAL_SPEED = 0.0f;
	const float HORIZONTAL_DISTANCE_FROM_CENTRE_WHEN_CHECKING_IF_COVER_IS_TALL = 40.0f;
	const float DISTANCE_OF_LINE_TRACE_WHEN_CHECKING_AIMING_DIRECTION = 2000.0f;

#pragma endregion

#pragma region PARAMETERS

protected:
	/**
	 * The Maximum Speed that the character can move with in cover.
	 */
	UPROPERTY(EditAnywhere, Category= "Cover Movement", meta = (DisplayPriority = -2))
	float MaxCoverSpeed;
	
	/**
	 * When a character is told to enter a cover, it starts walking towards it.
	 * When the distance between that character and the cover point it travels to is smaller than this variable, 
	 * the character stops walking and its position starts interpolating with that of the cover.
	 */
	UPROPERTY(EditAnywhere, Category= "Cover Movement", meta = (DisplayPriority = -2))
	float DistanceToBeginInterpolationToCover;
	
	/**
	 * When a character is told to enter a cover, it starts walking towards it. After it gets close enough to the cover,
	 * it will begin interpolating its position with the cover's position.
	 * This variable determines how long that interpolation can be in the longest case.
	 */
	UPROPERTY(EditAnywhere, Category= "Cover Movement", meta = (DisplayPriority = -2))
	float MaxTimeToInterpolateToCover;

#pragma endregion

#pragma region VARIABLES

protected:
	UPROPERTY()
	ECoveredState CoveredState;

	UPROPERTY()
	EInsideCoverState InsideCoverState;
	
	UPROPERTY()
	float TargetOrientation;
	
	/**
	 * The character's 2D movement input relative to the cover the character is in.
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Cover Movement")
	FVector2D StoredInCoverInput;
	
	/**
	 * 	The Cover Spline Component where the character is currently taking cover.
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Cover Movement")
	UCoverSpline* CoverSplineCurrentlyUsing;
	
	/**
	 * The Section that the character occupies in the CoverSpline.
	 * 0 is the left-most point. 1 is the right-most point.
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Cover Movement")
	float SectionOfCurrentlyUsingSpline;
	
	/**
	 * The Spline the character is travelling towards.
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Cover Movement")
	UCoverSpline* CoverSplineTravellingTowards;

	UPROPERTY()
	float TemporaryUsedSectionOfSplineTravellingTowards;

private:
	// Travelling to cover
	UPROPERTY(VisibleInstanceOnly, Category = "Cover Movement")
	float ToCoverTravelCompletePercentage;

	UPROPERTY(VisibleInstanceOnly, Category = "Cover Movement")
	float ToCoverTravelDuration;

	// Entering cover
	UPROPERTY()
	FVector OriginPositionForEnterCoverLerp;
	UPROPERTY()
	FVector TargetPositionForEnterCoverLerp;

	UPROPERTY()
	FQuat OriginRotationForEnterCoverLerp;
	UPROPERTY()
	FQuat TargetRotationForEnterCoverLerp;

protected:
	// Exiting cover
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Cover Movement")
	float ExitCoverTransitionCompletePercentage;

	UPROPERTY()
	FVector ExitCoverTransitionDirection;

private:
	// Moving between covers
	UPROPERTY()
	FVector BetweenCoversTravellingMidPoint;

	UPROPERTY()
	bool BetweenCoversIsTargetSpeedRight;

protected:
	UPROPERTY()
	AGenericCharacter* GenericCharacter;

#pragma endregion 

#pragma region TICK FUNCTIONALITY
private:
	UFUNCTION()
	void MoveBetweenCoversTickFunctionality(float DeltaTime);

	UFUNCTION()
	void EnteringCoverTickFunctionality(float DeltaTime);
	
	UFUNCTION()
	void TransitionOutOfCoverFunctionality(float DeltaTime);

protected:
	UFUNCTION()
	virtual void BaseInCoverTickFunctionality(float DeltaTime);

	UFUNCTION()
	virtual void AimInCoverTickFunctionality(float DeltaTime);

#pragma endregion
	
#pragma region FUNCTIONS

public:
	// Notifies
	UFUNCTION()
	virtual bool NotifyBeginTravellingToCover(UCoverSpline* CoverToEnter, float SectionToEnter);

	UFUNCTION()
	virtual void NotifyReachedCoverTravellingTowards();

	UFUNCTION()
	void NotifyExitCover();

	// Input
	UFUNCTION()
	void StoreInCoverInput(const float ForwardInput, const float RightInput);
	
	// Cover states.
	UFUNCTION()
	FORCEINLINE ECoveredState GetCoveredState() const { return CoveredState; };

	UFUNCTION(BlueprintCallable)
	EInsideCoverState GetInsideCoverState() const { return InsideCoverState; };

	UFUNCTION(BlueprintCallable)
	virtual ECoveredState SetCoveredState(const ECoveredState NewCoveredState);
	
	UFUNCTION(BlueprintCallable)
	virtual EInsideCoverState SetInsideCoverState(const EInsideCoverState NewInsideCoverState);

	// Quick checks and Getters.
	UFUNCTION(BlueprintCallable)
	bool IsTravellingToCover() const;

	UFUNCTION(BlueprintCallable)
	bool IsMovementControlledByCover() const { return CoveredState == ECoveredState::InCover || CoveredState == ECoveredState::EnteringCover; };

	/**
	 * True when the character is not behind cover OR when the character is travelling between covers.
	 * @return 
	 */
	UFUNCTION()
	FORCEINLINE bool CanCharacterBePulledByHook() const
		{ return CoveredState == ECoveredState::NoCover || CoveredState == ECoveredState::EnteringCover || CoveredState == ECoveredState::ExitingCover || InsideCoverState == EInsideCoverState::MovingBetweenCovers; };

	UFUNCTION(BlueprintCallable)
	bool IsInTallCover() const;

	UFUNCTION()
	FORCEINLINE UCoverSpline* GetCoverSplineCurrentlyUsing() const { return CoverSplineCurrentlyUsing; };

	UFUNCTION()
	FORCEINLINE UCoverSpline* GetCoverSplineTravellingTowards() const { return CoverSplineTravellingTowards; };
	
	UFUNCTION()
	FORCEINLINE float GetExitCoverTransitionCompletePercentage() const { return ExitCoverTransitionCompletePercentage; };
	
	UFUNCTION(BlueprintCallable)
	float GetInCoverSpeed() const;

	UFUNCTION()
	FORCEINLINE float GetTargetOrientation() const { return TargetOrientation; };
	
	UFUNCTION(BlueprintCallable)
	float GetInCoverOrientation() const;

protected:
	UFUNCTION()
	void BeginEnteringCoverLerp();

	UFUNCTION()
	virtual bool TryToTransitionOutOfCover();
	
#pragma endregion

#pragma region VIRTUAL FUNCTIONS
public:
	virtual void OnRegister() override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#pragma endregion

#pragma region DELEGATES
public:
	FOnIsCoveredChanged OnIsCoveredChanged;

#pragma endregion
};
