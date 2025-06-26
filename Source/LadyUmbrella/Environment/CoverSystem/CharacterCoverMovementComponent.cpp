// Fill out your copyright notice in the Description page of Project Settings.


#include "LadyUmbrella/Environment/CoverSystem/CharacterCoverMovementComponent.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Environment/CoverSystem/CoverSpline.h"
#include "LadyUmbrella/Util/MathLibraries/EasingLibrary.h"

// Sets default values for this component's properties
UCharacterCoverMovementComponent::UCharacterCoverMovementComponent()
{
	// Variable Initialization
	CoveredState = ECoveredState::NoCover;
	InsideCoverState = EInsideCoverState::BaseCovered;

	MaxTimeToInterpolateToCover = 0.2f;
	DistanceToBeginInterpolationToCover = 75.0f;
	MaxCoverSpeed = 300.0f;
	
	TargetOrientation = 1.0f;
	StoredInCoverInput = FVector2D::Zero();
	
	CoverSplineCurrentlyUsing = nullptr;
	SectionOfCurrentlyUsingSpline = INVALID_PERCENTAGE_COMPLETE_VALUE;
	CoverSplineTravellingTowards = nullptr;
	TemporaryUsedSectionOfSplineTravellingTowards = INVALID_PERCENTAGE_COMPLETE_VALUE;

	ToCoverTravelCompletePercentage = INVALID_PERCENTAGE_COMPLETE_VALUE;
	ToCoverTravelDuration = 0;
	
	ExitCoverTransitionCompletePercentage = INVALID_PERCENTAGE_COMPLETE_VALUE;
	ExitCoverTransitionDirection = FVector::ZeroVector;

	// Rest of Constructor.
	PrimaryComponentTick.bCanEverTick = true;
}

#pragma region TICK FUNCTIONALITY

void UCharacterCoverMovementComponent::MoveBetweenCoversTickFunctionality(float DeltaTime)
{
	if (!IsValid(GenericCharacter))
	{
		return;
	}
	
	if (!IsValid(CoverSplineCurrentlyUsing) || !IsValid(CoverSplineTravellingTowards))
	{
		NotifyExitCover();
		return;
	}
	
	const FVector OriginLocation = CoverSplineCurrentlyUsing->FindLocationAtTime(SectionOfCurrentlyUsingSpline)
		+ FVector(0, 0, GenericCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	const FVector TargetLocation = CoverSplineTravellingTowards->FindLocationAtTime(TemporaryUsedSectionOfSplineTravellingTowards)
		+ FVector(0, 0, GenericCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	const FVector VectorToTarget = TargetLocation - OriginLocation;
		
	if (ToCoverTravelCompletePercentage == INVALID_PERCENTAGE_COMPLETE_VALUE) // Is it the first time that we entered this section.
	{
		ToCoverTravelCompletePercentage = 0.0f;

		// Calculating the mid-point of our transition.
		const FVector RightVector = GenericCharacter->GetActorRightVector();
		const float DotProductToRightVector = FVector::DotProduct(RightVector, VectorToTarget);
		const FVector DistanceToMidPoint = FVector(RightVector.X * DotProductToRightVector, RightVector.Y * DotProductToRightVector, (TargetLocation.Z - OriginLocation.Z) / 2);
			
		BetweenCoversTravellingMidPoint = OriginLocation + DistanceToMidPoint;

		// If we are moving to the right.
		if (DotProductToRightVector > 0)
		{
			BetweenCoversIsTargetSpeedRight = true;
		}
		else
		{
			BetweenCoversIsTargetSpeedRight = false;
		}
	}

	// Updating the percentage of completion.
	const float LengthOfCurve = FVector2D(VectorToTarget.X, VectorToTarget.Y).Length() * 2.9f / 4; // Trust me on this one I did the maths.

	ToCoverTravelCompletePercentage += 
		// Updating our Speed.
		abs(GenericCharacter->GetCoverTraversingSpeedSystem()->UpdateWithEstimatedVelocity(BetweenCoversIsTargetSpeedRight ? 1 : -1, DeltaTime))
		* MaxCoverSpeed * DeltaTime / LengthOfCurve;
		
	ToCoverTravelCompletePercentage = FMath::Clamp(ToCoverTravelCompletePercentage, 0.0f, 1.0f);

	// Placing the character's transform.
	GenericCharacter->SetActorLocation(FMath::Lerp(
		FMath::Lerp(OriginLocation, BetweenCoversTravellingMidPoint, ToCoverTravelCompletePercentage),
		FMath::Lerp(BetweenCoversTravellingMidPoint, TargetLocation, ToCoverTravelCompletePercentage),
		ToCoverTravelCompletePercentage));

	GenericCharacter->SetActorRotation(FMath::Lerp(CoverSplineCurrentlyUsing->FindForwardRotationAtTime(SectionOfCurrentlyUsingSpline),
	CoverSplineTravellingTowards->FindForwardRotationAtTime(TemporaryUsedSectionOfSplineTravellingTowards),
		ToCoverTravelCompletePercentage));

	if ((1 - ToCoverTravelCompletePercentage) * LengthOfCurve < 0.25f * DistanceToBeginInterpolationToCover)
	{
		NotifyReachedCoverTravellingTowards();
	}
}

void UCharacterCoverMovementComponent::EnteringCoverTickFunctionality(float DeltaTime)
{
	if (!IsValid(GenericCharacter))
	{
		return;
	}
	
	if (!IsValid(CoverSplineCurrentlyUsing))
	{
		NotifyExitCover();
		return;
	}
	
	// Updating the percentage of completion.
	ToCoverTravelCompletePercentage = FMath::Clamp(ToCoverTravelCompletePercentage + DeltaTime / ToCoverTravelDuration, 0.0f, 1.0f);

	const FVector TargetLocation = CoverSplineCurrentlyUsing->FindLocationAtTime(SectionOfCurrentlyUsingSpline);
			
	// Lerping the position of the character.
	GenericCharacter->SetActorLocation(FMath::Lerp(
		OriginPositionForEnterCoverLerp,
		TargetLocation + FVector(0, 0, GenericCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()),
		ToCoverTravelCompletePercentage));

	// Lerping the rotation of the character.
	GenericCharacter->SetActorRotation(FQuat::Slerp(
		OriginRotationForEnterCoverLerp,
		TargetRotationForEnterCoverLerp,
		UEasingLibrary::ApplyEase(ToCoverTravelCompletePercentage, EEaseType::InOutSine)));
			
	// Finishing the interpolation.
	if (ToCoverTravelCompletePercentage == 1.0f)
	{
		SetCoveredState(ECoveredState::InCover);
	}
}

void UCharacterCoverMovementComponent::BaseInCoverTickFunctionality(float DeltaTime)
{
	if (!IsValid(GenericCharacter))
	{
		return;
	}
	
	if (!IsValid(CoverSplineCurrentlyUsing))
	{
		NotifyExitCover();
		return;
	}

	// Setting the character orientation via player input.
	if (StoredInCoverInput.X > MIN_X_INPUT_FOR_COVER_REACTION)
	{
		TargetOrientation = 1;
	}
	else if (StoredInCoverInput.X < -MIN_X_INPUT_FOR_COVER_REACTION)
	{
		TargetOrientation = -1;
	}
	
	// Moving along a cover.
	if(!CoverSplineCurrentlyUsing->IsSinglePointCover())
	{
		// If we are at the end of a (non-loop) cover, we stop moving.
		if (!CoverSplineCurrentlyUsing->IsLoop()
			&& ((SectionOfCurrentlyUsingSpline == 0 && StoredInCoverInput.X < 0) || (SectionOfCurrentlyUsingSpline == 1 && StoredInCoverInput.X > 0)))
		{
			StoredInCoverInput.X = 0;
		}
																						// Here, we are saying that we do not want to move at all if we are moving too little.
		const float InCoverXInputTreated = UEasingLibrary::ApplyEase(FMath::Clamp(abs(StoredInCoverInput.X) < MIN_X_INPUT_FOR_COVER_REACTION ? 0 : abs(StoredInCoverInput.X), 0, 1), EEaseType::OutCubic);
		float SmoothedCoverSpeed = GenericCharacter->GetCoverTraversingSpeedSystem()->UpdateWithEstimatedVelocity(InCoverXInputTreated * FMath::Sign(StoredInCoverInput.X), DeltaTime);

		// Breaking down completely.
		if (StoredInCoverInput.X == 0 && abs(SmoothedCoverSpeed) < 0.05f && abs(GenericCharacter->GetCoverTraversingSpeedSystem()->GetSystemStoredValue()) < 0.05f)
		{
			SmoothedCoverSpeed = 0;
			GenericCharacter->GetCoverTraversingSpeedSystem()->ResetSystemToValue(0);
		}
		
		// Overriding the character orientation if the Character Speed is different from the player input.
		if (SmoothedCoverSpeed > 0)
		{
			TargetOrientation = 1;
		}
		else if (SmoothedCoverSpeed < 0)
		{
			TargetOrientation = -1;
		}
		
		SectionOfCurrentlyUsingSpline += SmoothedCoverSpeed * MaxCoverSpeed * DeltaTime / CoverSplineCurrentlyUsing->GetSplineLength();

		// Clamping the section is different depending on the type of cover.
		if (CoverSplineCurrentlyUsing->IsLoop())
		{
			if (SectionOfCurrentlyUsingSpline < 0)
			{
				SectionOfCurrentlyUsingSpline += 1;
			}
			else if (SectionOfCurrentlyUsingSpline > 1)
			{
				SectionOfCurrentlyUsingSpline -= 1;
			}
		}
		else
		{
			SectionOfCurrentlyUsingSpline = FMath::Clamp(SectionOfCurrentlyUsingSpline, 0.0f, 1.0f);
		}
		
		GenericCharacter->SetActorLocation(CoverSplineCurrentlyUsing->FindLocationAtTime(SectionOfCurrentlyUsingSpline) + FVector(0,0,GenericCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()));
		GenericCharacter->SetActorRotation(CoverSplineCurrentlyUsing->FindForwardRotationAtTime(SectionOfCurrentlyUsingSpline));
	}
	
	GenericCharacter->GetCoverOrientationSystem()->UpdateWithEstimatedVelocity(TargetOrientation, DeltaTime);
}

void UCharacterCoverMovementComponent::TransitionOutOfCoverFunctionality(float DeltaTime)
{
	if (!IsValid(GenericCharacter))
	{
		return;
	}
	
	ExitCoverTransitionCompletePercentage += DeltaTime / EXIT_COVER_TRANSITION_DURATION;

	if (ExitCoverTransitionCompletePercentage >= 1.0f) // End of the transition.
	{
		SetCoveredState(ECoveredState::NoCover);
	}
	else // Adding MovementInput to our character.
	{
		GenericCharacter->AddMovementInput(ExitCoverTransitionDirection, 1 - UEasingLibrary::ApplyEase(ExitCoverTransitionCompletePercentage, EEaseType::InQuart));
	}
}

void UCharacterCoverMovementComponent::AimInCoverTickFunctionality(float DeltaTime)
{
	
}

#pragma endregion

#pragma region FUNCTIONS

bool UCharacterCoverMovementComponent::NotifyBeginTravellingToCover(UCoverSpline* CoverToEnter, float SectionToEnter)
{
	if (!IsValid(CoverToEnter))
	{
		// FLogger::ErrorLog("Trying to Enter a Cover but the pointer to it was not valid.");
		return false;
	}
	
	if (SectionToEnter < 0 || SectionToEnter > 1)
	{
		SectionToEnter = FMath::Clamp(SectionToEnter, 0, 1);
	}

	// Checking if the cover is being used by another Character.
	if (CoverToEnter->HowManyCharactersUsingTheSpline() > 0)
	{
		if (Cast<APlayerCharacter>(GetOwner()) != nullptr)
		{
			// If we want to notify the IAs inside the cover that we are entering it, this is where we would do it.
		}
		else
		{
			return false;	
		}
	}
	
	CoverSplineTravellingTowards = CoverToEnter;
	TemporaryUsedSectionOfSplineTravellingTowards = SectionToEnter;
	
	CoverToEnter->RegisterEnterSpline(Cast<AGenericCharacter>(GetOwner()));

	// If we are already in a cover, we are travelling between covers.
	if (GetCoveredState() == ECoveredState::InCover)
	{
		SetInsideCoverState(EInsideCoverState::MovingBetweenCovers);
	}

	return true;
}

void UCharacterCoverMovementComponent::NotifyReachedCoverTravellingTowards()
{
	if (IsValid(CoverSplineCurrentlyUsing))
	{
		CoverSplineCurrentlyUsing->RegisterExitSpline(Cast<AGenericCharacter>(GetOwner()));
	}
	
	if(!IsValid(CoverSplineTravellingTowards))
	{
		NotifyExitCover();
		return;
	}
	
	CoverSplineCurrentlyUsing = CoverSplineTravellingTowards;
	CoverSplineTravellingTowards = nullptr;
	
	SectionOfCurrentlyUsingSpline = TemporaryUsedSectionOfSplineTravellingTowards;

	SetCoveredState(ECoveredState::EnteringCover);
}

void UCharacterCoverMovementComponent::NotifyExitCover()
{
	if (IsValid(CoverSplineCurrentlyUsing))
	{
		SetCoveredState(ECoveredState::ExitingCover);
		
		CoverSplineCurrentlyUsing->RegisterExitSpline(Cast<AGenericCharacter>(GetOwner()));
		CoverSplineCurrentlyUsing = nullptr;
	}
	else // We can't exit transition out of a cover that no longer exists (probably was destroyed).
	{
		SetCoveredState(ECoveredState::NoCover);
	}

	// We may try to Exit a cover while travelling to one too. That is also taken into account.
	if (IsValid(CoverSplineTravellingTowards))
	{
		CoverSplineTravellingTowards->RegisterExitSpline(Cast<AGenericCharacter>(GetOwner()));
		CoverSplineTravellingTowards = nullptr;
	}
}

ECoveredState UCharacterCoverMovementComponent::SetCoveredState(const ECoveredState NewCoveredState)
{
	if (GetCoveredState() == NewCoveredState || !IsValid(GenericCharacter))
	{
		return CoveredState;
	}

	// Exit State actions
	switch (CoveredState)
	{
	case ECoveredState::NoCover:
		break;
		
	case ECoveredState::InCover:
		SetInsideCoverState(EInsideCoverState::BaseCovered); // We reset our cover state when we exit a cover.
		
		GenericCharacter->SetInCoverAimingDirection(GenericCharacter->INVALID_IN_COVER_AIMING_DIRECTION);
		
		break;
		
	case ECoveredState::EnteringCover:
		ToCoverTravelCompletePercentage = INVALID_PERCENTAGE_COMPLETE_VALUE;
		TemporaryUsedSectionOfSplineTravellingTowards = INVALID_PERCENTAGE_COMPLETE_VALUE;
		
		break;
		
	case ECoveredState::ExitingCover:
		ExitCoverTransitionCompletePercentage = INVALID_PERCENTAGE_COMPLETE_VALUE;
		break;
	}

	// Enter State actions
	switch (NewCoveredState)
	{
	case ECoveredState::NoCover:
		
		// Re-enabling collision.
		GenericCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
				
		// Resetting variables.
		StoredInCoverInput = FVector2D::Zero();
		TemporaryUsedSectionOfSplineTravellingTowards = INVALID_PERCENTAGE_COMPLETE_VALUE;
		SectionOfCurrentlyUsingSpline = INVALID_PERCENTAGE_COMPLETE_VALUE;
		break;
		
	case ECoveredState::InCover:
		if (GenericCharacter->IsAiming())
		{
			SetInsideCoverState(EInsideCoverState::Aiming);
		}
		break;
		
	case ECoveredState::EnteringCover:
		// Setting up the transition to an InCover state.
		BeginEnteringCoverLerp();
			
		// Resetting the orientation system.
		GenericCharacter->GetCoverOrientationSystem()->ResetSystemToValue(TargetOrientation);
		GenericCharacter->GetCoverTraversingSpeedSystem()->ResetSystemToValue(0);
	    	
		// Resetting the speed of the Character.
		GenericCharacter->GetMovementComponent()->StopActiveMovement();
	    	
		// Disabling collision.
		GenericCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		
		break;

	case ECoveredState::ExitingCover:
		ExitCoverTransitionCompletePercentage = 0.0f;
		
		if (!TryToTransitionOutOfCover()) // If we fail when trying to begin a Transition Out of our Cover.
		{
			return SetCoveredState(ECoveredState::NoCover);
		}
		break;
	}
	
	CoveredState = NewCoveredState;
	return CoveredState;
}

EInsideCoverState UCharacterCoverMovementComponent::SetInsideCoverState(const EInsideCoverState NewInsideCoverState)
{
	if (GetInsideCoverState() == NewInsideCoverState || !IsValid(GenericCharacter))
	{
		return InsideCoverState;
	}

	// Exit State actions
	switch (InsideCoverState)
	{
	case EInsideCoverState::BaseCovered:
		break;
	case EInsideCoverState::Aiming:
		break;
	case EInsideCoverState::MovingBetweenCovers:
		break;
	default:
		break;
	}

	// Enter State actions
	switch (NewInsideCoverState)
	{
	case EInsideCoverState::BaseCovered:
		break;
	case EInsideCoverState::Aiming:
		GenericCharacter->GetCoverTraversingSpeedSystem()->ResetSystemToValue(ZERO_TRAVERSAL_SPEED);
		break;
	case EInsideCoverState::MovingBetweenCovers:
		break;
	default:
		break;
	}
	
	InsideCoverState = NewInsideCoverState;
	return InsideCoverState;
}

bool UCharacterCoverMovementComponent::IsTravellingToCover() const
{
	return IsValid(CoverSplineTravellingTowards);
}

bool UCharacterCoverMovementComponent::IsInTallCover() const
{
	if (GetCoveredState() != ECoveredState::InCover
		|| GetInsideCoverState() == EInsideCoverState::MovingBetweenCovers
		|| !IsValid(CoverSplineCurrentlyUsing))
	{
		return false;
	}

	const float SectionFromCentreOfCheck = HORIZONTAL_DISTANCE_FROM_CENTRE_WHEN_CHECKING_IF_COVER_IS_TALL / CoverSplineCurrentlyUsing->GetSplineLength();
	const float SectionToCheck0 = FMath::Clamp(SectionOfCurrentlyUsingSpline - SectionFromCentreOfCheck, 0.0f, 1.0f);
	const float SectionToCheck1 = FMath::Clamp(SectionOfCurrentlyUsingSpline + SectionFromCentreOfCheck, 0.0f, 1.0f);
	
	return CoverSplineCurrentlyUsing->TestTallCoverAtTime(SectionToCheck0) && CoverSplineCurrentlyUsing->TestTallCoverAtTime(SectionToCheck1);
}

float UCharacterCoverMovementComponent::GetInCoverOrientation() const
{
	if (IsValid(GenericCharacter))
	{
		return GenericCharacter->GetCoverOrientationSystem()->GetSystemStoredValue();
	}
	
	return 0;
}

float UCharacterCoverMovementComponent::GetInCoverSpeed() const
{
	if (IsValid(GenericCharacter))
	{
		return GenericCharacter->GetCoverTraversingSpeedSystem()->GetSystemStoredValue();
	}
	
	return 0;
}

void UCharacterCoverMovementComponent::StoreInCoverInput(const float ForwardInput, const float RightInput)
{
	StoredInCoverInput = FVector2D(RightInput, ForwardInput);
}

void UCharacterCoverMovementComponent::BeginEnteringCoverLerp()
{
	if (!IsValid(CoverSplineCurrentlyUsing) || !IsValid(GenericCharacter))
	{
		return;
	}
	
	ToCoverTravelCompletePercentage = 0.0f;
	OriginPositionForEnterCoverLerp = GenericCharacter->GetActorLocation();
	OriginRotationForEnterCoverLerp = GenericCharacter->GetActorRotation().Quaternion();
	TargetPositionForEnterCoverLerp = CoverSplineCurrentlyUsing->FindLocationAtTime(SectionOfCurrentlyUsingSpline);
	ToCoverTravelDuration = FVector::DistXY(OriginPositionForEnterCoverLerp, TargetPositionForEnterCoverLerp) / DistanceToBeginInterpolationToCover * MaxTimeToInterpolateToCover;

	// Deciding whether the character will face right or left when entering the cover.
	const float DotProductForOrientation = FVector::DotProduct(
		CoverSplineCurrentlyUsing->FindRightRotationAtTime(SectionOfCurrentlyUsingSpline).Vector(),
		TargetPositionForEnterCoverLerp - OriginPositionForEnterCoverLerp);
	   	
	if (DotProductForOrientation > 0) // Facing right.
	{
		TargetOrientation = 1;
	}
	else if (DotProductForOrientation < 0) // Facing left.
	{
		TargetOrientation = -1;
	}
		
	TargetRotationForEnterCoverLerp = CoverSplineCurrentlyUsing->FindForwardRotationAtTime(SectionOfCurrentlyUsingSpline).Quaternion();
}

bool UCharacterCoverMovementComponent::TryToTransitionOutOfCover()
{
	// Exit Cover Transitions only exists for Players, cause AIs have path finding.
	// If this isn't a player, we will cancel the transition.
	return false;
}

void UCharacterCoverMovementComponent::OnRegister()
{
	Super::OnRegister();

	GenericCharacter = Cast<AGenericCharacter>(GetOwner());
}

#pragma endregion

#pragma region VIRTUAL FUNCTIONS

void UCharacterCoverMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	switch (CoveredState)
	{
	case ECoveredState::NoCover:
		break;

		// In cover Logic.	
	case ECoveredState::InCover:
		switch (InsideCoverState)
		{
	case EInsideCoverState::BaseCovered:
		BaseInCoverTickFunctionality(DeltaTime);
			break;
	case EInsideCoverState::Aiming:
		AimInCoverTickFunctionality(DeltaTime);
		break;
	case EInsideCoverState::MovingBetweenCovers:
		MoveBetweenCoversTickFunctionality(DeltaTime);
			break;
		}
		break;
		
	case ECoveredState::EnteringCover:
		EnteringCoverTickFunctionality(DeltaTime);
		break;
		
	case ECoveredState::ExitingCover:
		TransitionOutOfCoverFunctionality(DeltaTime);
		break;

	default: ;
	}
}

#pragma endregion
