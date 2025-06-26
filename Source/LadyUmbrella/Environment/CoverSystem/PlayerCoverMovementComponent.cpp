// Fill out your copyright notice in the Description page of Project Settings.


#include "LadyUmbrella/Environment/CoverSystem/PlayerCoverMovementComponent.h"

#include "CoverSpline.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Util/Logger.h"
#include "LadyUmbrella/Weapons/Umbrella/Umbrella.h"

UPlayerCoverMovementComponent::UPlayerCoverMovementComponent()
{
	// PARAMETERS

	// Cover Movement
	CoverActorsAcceptanceRadius = 1200;
	CoverPointsAcceptanceRadius = 400;
	SecsNeededToExitCoverWithInput = 0.4f;
	MinDistanceWithEnvironmentToAllowAim = 400.0f;
	MaxInputAngleBackwardsToTryToExitCoverWithController = 45.0f;
	MaxInputAngleBackwardsToTryToExitCoverWithKeyboard = 35.0f;

	// Cover Camera
	AimingCancellationDistanceWhenEnteringCover = 100.0f;
	MaxAdditionalCameraDisplacementWhileMoving = 0.1f;
	ShoulderChangeCameraTransitionDuration = 0.7f;
	ShoulderPositionChangeCurveType = EEaseType::InOutSine;
	
	// Debug
	bShowDebugInformation = false;

	// Variables
	SecStorageTryingToExitCoverWithInput = 0;
	DesiredCameraHorizontalRatioInitialValue = 0;
	ChangeToLeftShoulderPercentageComplete = 0;
}

#pragma region FUNCTIONS

bool UPlayerCoverMovementComponent::CanMoveContinuousBetweenCovers(bool GoingLeftSection0) const
{
	// If we are not in cover.
	if (!IsValid(CoverSplineCurrentlyUsing))
	{
		return false;
	}
	
	// If we are not at the correct Section Point.
	if (!CoverSplineCurrentlyUsing->IsSinglePointCover() && SectionOfCurrentlyUsingSpline != (GoingLeftSection0 ? 0 : 1))
	{
		return false;
	}

	// If this Cover does not have a connection in that direction.
	if (!IsValid(GoingLeftSection0 ? CoverSplineCurrentlyUsing->GetReachablePointContinuous_0() : CoverSplineCurrentlyUsing->GetReachablePointContinuous_1()))
	{
		return false;
	}
	
	// Checking that our input is correct.
	const FVector2D InputVectorNormalized = StoredInCoverInput.GetSafeNormal();

	return GoingLeftSection0 ? InputVectorNormalized.X < -MIN_X_INPUT_FOR_CONTINUOUS_MOVEMENT : InputVectorNormalized.X > MIN_X_INPUT_FOR_CONTINUOUS_MOVEMENT;
}

bool UPlayerCoverMovementComponent::CanMoveForwardBetweenCovers(bool GoingLeftSection0) const
{
	// If we are not in cover.
	if (!IsValid(CoverSplineCurrentlyUsing))
	{
		return false;
	}
	
	// If we are not at the correct Section Point.
	if (!CoverSplineCurrentlyUsing->IsSinglePointCover() && SectionOfCurrentlyUsingSpline != (GoingLeftSection0 ? 0 : 1))
	{
		return false;
	}
	
	// If this Cover does not have a connection in that direction.
	if ((GoingLeftSection0 ? CoverSplineCurrentlyUsing->GetReachablePointForward_0() : CoverSplineCurrentlyUsing->GetReachablePointForward_1()) == nullptr)
	{
		return false;
	}

	// Checking that our input is correct.
	const FVector2D InputVectorNormalized = StoredInCoverInput.GetSafeNormal();

	if (InputVectorNormalized.Y <= 0)
	{
		return false;
	}
	
	return GoingLeftSection0 ? InputVectorNormalized.X < -MIN_X_INPUT_FOR_FORWARD_MOVEMENT : InputVectorNormalized.X > MIN_X_INPUT_FOR_FORWARD_MOVEMENT;
}

void UPlayerCoverMovementComponent::INTERNAL_DrawDebugEntryPointSphere(const UWorld* World, const FCoverEvaluation& CoverEvaluationToDraw) const
{
#ifdef UE_EDITOR
	if (bShowDebugInformation)
	{
		DrawDebugSphere(World, CoverEvaluationToDraw.Position, 10, 6, FLinearColor::LerpUsingHSV(FLinearColor::Red, FLinearColor::Green, CoverEvaluationToDraw.Evaluation).ToFColor(true), false, 2.f, 2.0f);
	}
#endif
}

float UPlayerCoverMovementComponent::CalculateMaxYInputBackwardsToExitCoverWithController() const
{
	return FVector2D::DotProduct(FVector2D(0.0f,-1.0f),
		FVector2D(0, 1).GetRotated(MaxInputAngleBackwardsToTryToExitCoverWithController));
}

float UPlayerCoverMovementComponent::CalculateMaxYInputBackwardsToExitCoverWithKeyboard() const
{
	return FVector2D::DotProduct(FVector2D(0.0f,-1.0f),
		FVector2D(0, 1).GetRotated(MaxInputAngleBackwardsToTryToExitCoverWithKeyboard));
}

bool UPlayerCoverMovementComponent::PlayerTryToEnterCover()
{
	if (!IsValid(PlayerCharacter))
	{
		return false;
	}
	
	// We try to find a new cover to go into.
	TArray<AActor*> CoverParents;
	const UWorld* World = GetWorld();
	UGameplayStatics::GetAllActorsOfClass(World, ACoverParent::StaticClass(), CoverParents);
	
	const FVector PlayerLocation = PlayerCharacter->GetActorLocation();
	FVector PlayerDirection = PlayerCharacter->GetMovementComponent()->GetLastInputVector();
	bool bPlayerInputDirectionWasValid = true;

	// If we have no concrete input for where to look for cover, we take the character's direction instead.
	if (PlayerDirection.SizeSquared() < FMath::Square(0.2f))
	{
		if (UCameraComponent* Camera = PlayerCharacter->GetFollowCamera())
		{
			PlayerDirection = FVector(Camera->GetForwardVector().X, Camera->GetForwardVector().Y, 0).GetSafeNormal();
		}
		else
		{
			PlayerDirection = PlayerCharacter->GetActorForwardVector();
		}
		
		bPlayerInputDirectionWasValid = false;
	}

	PlayerDirection = FVector(PlayerDirection.X, PlayerDirection.Y, 0).GetSafeNormal();

#ifdef UE_EDITOR
	if (bShowDebugInformation)
	{
		// Painting the player direction.
		DrawDebugLine(World, PlayerLocation, PlayerLocation + PlayerDirection * 1000, FColor::Orange, false, 3.0f, 2.0f);
		// Painting the area of Splines that we are evaluating.
		DrawDebugCylinder(World, PlayerLocation, PlayerLocation + FVector(0, 0, 50), CoverActorsAcceptanceRadius, 100, FColor::Silver, false, 3.0f, 2.0f);
		// Painting the valid range.
		DrawDebugCylinder(World, PlayerLocation, PlayerLocation + FVector(0, 0, 50), CoverPointsAcceptanceRadius, 50, FColor::Red, false, 3.0f, 2.0f);	
	}
#endif

	TArray<FCoverEvaluation> CoverEvaluations;

	// Filling our CoverEvaluationsStruct
	for (AActor* DetectedActor : CoverParents)
	{
		const ACoverParent* CoverParent = Cast<ACoverParent>(DetectedActor);

		// Failed casts shouldn't continue.
		if (!IsValid(CoverParent) ||
			(CoverParent->GetActorLocation() - PlayerLocation).SizeSquared() > FMath::Square(CoverActorsAcceptanceRadius))
		{
			continue;
		}

		// Cover Splines are components of ACoverParent actors.
		TSet<UActorComponent*> Components = CoverParent->GetComponents();

		for (UActorComponent* Comp : Components)
		{
			UCoverSpline* Cover = Cast<UCoverSpline>(Comp);

			if (!IsValid(Cover))
			{
				continue;
			}

			TArray<FVector> BufferEntryPoints;
			float SectionLength = 0;
		
			Cover->CalculatePlayerEntryPoints(BufferEntryPoints, SectionLength);
			const int NumberOfPlayerEntryPoints = BufferEntryPoints.Num();

			for (int EntryPointIndex = 0; EntryPointIndex < NumberOfPlayerEntryPoints; EntryPointIndex++)
			{
				CoverEvaluations.Emplace(
					FCoverEvaluation(
						Cover,
						BufferEntryPoints[EntryPointIndex],
						SectionLength * static_cast<float>(EntryPointIndex),
						1.0f));
			}	
		}
	}

	if (CoverEvaluations.Num() == 0) // If we found no availible covers.
	{
		return false;
	}
	
	int ResultingIndex = -1;
	float MaxEvaluationSoFar = 0;
	
	// Evaluating every position.
	for (int Index = 0; Index < CoverEvaluations.Num(); Index++)
	{
		FVector VectorToPoint = CoverEvaluations[Index].Position - PlayerLocation;
		float DistanceSquaredXYToPoint = VectorToPoint.SizeSquared2D();

		// Filtering those that are too far away (in the XY plane OR the Z axis) from the player to be valid.
		if (DistanceSquaredXYToPoint > FMath::Square(CoverPointsAcceptanceRadius) || abs(VectorToPoint.Z) > 150)
		{
			CoverEvaluations[Index].Evaluation = 0;
			INTERNAL_DrawDebugEntryPointSphere(World, CoverEvaluations[Index]);
			continue;
		}

		float DistanceXYToPoint = FMath::Sqrt(DistanceSquaredXYToPoint);
		
		// The closer to the player, the more valid it is.
		CoverEvaluations[Index].Evaluation *= FMath::Lerp(1, 0.2f,
			UEasingLibrary::ApplyEase(DistanceXYToPoint/CoverPointsAcceptanceRadius, EEaseType::OutCirc));

		if (CoverEvaluations[Index].Evaluation < MaxEvaluationSoFar) // If already this point is evaluated worse than another, we stop evaluating for optimization.
		{
			INTERNAL_DrawDebugEntryPointSphere(World, CoverEvaluations[Index]);
			continue;
		}

		const FVector PointFacingDirection = CoverEvaluations[Index].Cover->FindForwardRotationAtTime(CoverEvaluations[Index].Section).Vector().GetSafeNormal();
		const float DotProductSharesAimingDirection = FVector::DotProduct(PlayerDirection.GetSafeNormal(),PointFacingDirection);
		
		// If the direction of the player is similar to that of the entry point.
		// If the player had a valid direction input, we give more weight to it in the evaluation.
		CoverEvaluations[Index].Evaluation *= FMath::Lerp(
			bPlayerInputDirectionWasValid ? 0.1f : 0.6f,
			1,
			(UEasingLibrary::ApplyEase((DotProductSharesAimingDirection + 1) / 2, EEaseType::InOutQuint)));
		
		if (CoverEvaluations[Index].Evaluation < MaxEvaluationSoFar) // If already this point is evaluated worse than another, we stop evaluating for optimization.
		{
			INTERNAL_DrawDebugEntryPointSphere(World, CoverEvaluations[Index]);
			continue;
		}

		const float DotProductInFrontOfPlayer = FVector::DotProduct(PlayerDirection.GetSafeNormal(), VectorToPoint.GetSafeNormal());

		// If the point is behind the player in the direction we are aiming to be seated.
		// WE DON'T DO THIS IF WE ARE SUPER CLOSE TO THE PLAYER (otherwise, it is possible to be on top of the point and still miss it)
		if (DistanceXYToPoint > 35.0f && DotProductSharesAimingDirection > 0.0f)
		{
			if (DotProductInFrontOfPlayer < 0.0f)
			{
				CoverEvaluations[Index].Evaluation *= FMath::Lerp(
					0.3f,
					1,
					UEasingLibrary::ApplyEase(DotProductInFrontOfPlayer + 1, EEaseType::InQuart));
			}
		}

		// If we need to go in an opposite direction to the direction we will face.
		const float DotProductFacingDirectionAndMovement = FVector::DotProduct(VectorToPoint.GetSafeNormal(), PointFacingDirection.GetSafeNormal());

		if (DotProductFacingDirectionAndMovement < 0.0f)
		{
			CoverEvaluations[Index].Evaluation *= FMath::Lerp(0.2f, 1,
				UEasingLibrary::ApplyEase(DotProductFacingDirectionAndMovement + 1, EEaseType::InCubic));
		}
		
		if (CoverEvaluations[Index].Evaluation > MaxEvaluationSoFar)
		{
			MaxEvaluationSoFar = CoverEvaluations[Index].Evaluation;
			ResultingIndex = Index;
		}
		
		INTERNAL_DrawDebugEntryPointSphere(World, CoverEvaluations[Index]);
	}
	
		
	// If we found a valid point, we move towards it.
	if (ResultingIndex != -1)
	{
		NotifyBeginTravellingToCover(CoverEvaluations[ResultingIndex].Cover, CoverEvaluations[ResultingIndex].Section);
		
#ifdef UE_EDITOR
		if (bShowDebugInformation)
		{
			DrawDebugLine(World, PlayerLocation, CoverEvaluations[ResultingIndex].Position, FColor::Green, false, 3.0f, 2.0f);
		}
#endif
		
		return true;
	}
	
	return false;
}

bool UPlayerCoverMovementComponent::TryToMoveBetweenCovers()
{
	if (!IsMovementControlledByCover() || !IsValid(CoverSplineCurrentlyUsing) || GetInsideCoverState() == EInsideCoverState::MovingBetweenCovers)
	{
		return false;
	}

	if (SectionOfCurrentlyUsingSpline != 0 && SectionOfCurrentlyUsingSpline != 1)
	{
		return false;
	}
	
	if (CanMoveContinuousBetweenCovers(true))
	{
		TargetOrientation = -1;
		NotifyBeginTravellingToCover(CoverSplineCurrentlyUsing->GetReachablePointContinuous_0()->GetOwningSpline(),
				CoverSplineCurrentlyUsing->GetReachablePointContinuous_0()->GetSection());
	}
	else if (CanMoveContinuousBetweenCovers(false))
	{
		TargetOrientation = 1;
		NotifyBeginTravellingToCover(CoverSplineCurrentlyUsing->GetReachablePointContinuous_1()->GetOwningSpline(),
				CoverSplineCurrentlyUsing->GetReachablePointContinuous_1()->GetSection());
	}
	else if (CanMoveForwardBetweenCovers(true))
	{
		TargetOrientation = -1;
		NotifyBeginTravellingToCover(CoverSplineCurrentlyUsing->GetReachablePointForward_0()->GetOwningSpline(),
				CoverSplineCurrentlyUsing->GetReachablePointForward_0()->GetSection());
	}
	else if (CanMoveForwardBetweenCovers(false))
	{
		TargetOrientation = 1;
		NotifyBeginTravellingToCover(CoverSplineCurrentlyUsing->GetReachablePointForward_1()->GetOwningSpline(),
				CoverSplineCurrentlyUsing->GetReachablePointForward_1()->GetSection());
	}

	return true;
}

#pragma endregion

#pragma region VIRTUAL FUNCTIONS

ECoveredState UPlayerCoverMovementComponent::SetCoveredState(const ECoveredState NewCoveredState)
{
	// We don't wanna do anything when we aren't really changing States.
	if (CoveredState == NewCoveredState)
	{
		return CoveredState;
	}

	// We do things before the state is changed in the Super::SetCoveredState.
	if (IsValid(PlayerCharacter))
	{
		// Exit State Actions
		switch (CoveredState)
		{
		case ECoveredState::NoCover:
			break;
		case ECoveredState::InCover:
			{
				// Resetting the timer of exiting the cover with input.
				SecStorageTryingToExitCoverWithInput = 0;

				PlayerCharacter->TurnCoverUIOff();
				OnIsCoveredChanged.Broadcast(false);
				
				//  Transition when we are not aiming.
				if (!PlayerCharacter->IsAiming())
				{
					PlayerCharacter->SetPlayerGeneralState(EPlayerState::FreeMovement);
					PlayerCharacter->GetCameraSystem()->SetCameraState(FCameraStateKey::DEFAULT);
				}
				// Making the player rotate with the camera if we are aiming.
				else if (UCharacterMovementComponent* MovementComponent = PlayerCharacter->GetCharacterMovement())
				{
					PlayerCharacter->SetPlayerGeneralState(EPlayerState::AimingUmbrella);
					MovementComponent->bUseControllerDesiredRotation = true;
				}
			}
			break;
		case ECoveredState::EnteringCover:
			break;
		case ECoveredState::ExitingCover:
			break;
		}

		// Enter State Actions
		switch (NewCoveredState)
		{
		case ECoveredState::NoCover:
			break;
		case ECoveredState::InCover:
			{
				PlayerCharacter->TurnCoverUIOn();
				OnIsCoveredChanged.Broadcast(true);

				// Setting up the camera position.
				if (!PlayerCharacter->IsAiming())
				{
					PlayerCharacter->SetPlayerGeneralState(EPlayerState::InCover);
					PlayerCharacter->GetCameraSystem()->SetCameraState(FCameraStateKey::COVER);
				}
				else
				{
					PlayerCharacter->SetPlayerGeneralState(EPlayerState::InCoverAiming);
				}

				// Resetting the In-Cover Camera Smoothing values
				if (IsValid(CameraAdditionalHorizontalRatioSmoothComponent))
				{
					CameraAdditionalHorizontalRatioSmoothComponent->ResetSystemToValue(0.0f);
				}

				// Making the player not rotate with the camera.
				if (UCharacterMovementComponent* MovementComponent = PlayerCharacter->GetCharacterMovement())
				{
					MovementComponent->bUseControllerDesiredRotation = false;
				}
			}
			break;
		case ECoveredState::EnteringCover:
			break;
		case ECoveredState::ExitingCover:
			break;
		}
	}
	
	return Super::SetCoveredState(NewCoveredState);
}

EInsideCoverState UPlayerCoverMovementComponent::SetInsideCoverState(const EInsideCoverState NewInsideCoverState)
{
	// We don't wanna do anything when we aren't really changing States.
	if (InsideCoverState == NewInsideCoverState)
	{
		return InsideCoverState;
	}

	// We do things before the state is changed in the Super::SetInsideCoverState.
	if (IsValid(PlayerCharacter))
	{
		// Exit State Actions
		switch (InsideCoverState)
		{
		case EInsideCoverState::BaseCovered:
			OnIsCoveredChanged.Broadcast(false);
			break;
		case EInsideCoverState::Aiming:
			OnIsCoveredChanged.Broadcast(true);
			break;
		case EInsideCoverState::MovingBetweenCovers:
			break;
		}

		// Enter State Actions
		switch (NewInsideCoverState)
		{
		case EInsideCoverState::BaseCovered:
			if (CoveredState == ECoveredState::InCover) // Since the InsideCoverState gets reset when we exit a cover, we check whether we need to call it or not.
			{
				OnIsCoveredChanged.Broadcast(true);

				// Setting the shoulder value. We do this here and not in SetCoveredState() cause this is also called when going from Aiming in cover to not aiming.
				if (GetTargetOrientation() == 1)
				{
					ChangeToLeftShoulderPercentageComplete = 0;
				}
				else if (GetTargetOrientation() == -1)
				{
					ChangeToLeftShoulderPercentageComplete = 1;
				}
			}
			break;
		case EInsideCoverState::Aiming:
				OnIsCoveredChanged.Broadcast(true);
			break;
		case EInsideCoverState::MovingBetweenCovers:
			break;
		}
	}
	
	return Super::SetInsideCoverState(NewInsideCoverState);
}

bool UPlayerCoverMovementComponent::NotifyBeginTravellingToCover(UCoverSpline* CoverToEnter, float SectionToEnter)
{
	// Checking if we need to stop aiming.
	if (IsValid(CoverToEnter) && IsValid(PlayerCharacter) && PlayerCharacter->IsAiming())
	{
		// We know that CoverSplineTravellingTowards is Valid because it is checked in the Super.
		const FVector EntryLocation = CoverToEnter->FindLocationAtTime(SectionToEnter);

		const float DistanceToEntrySquared = FVector::DistSquared2D(EntryLocation, PlayerCharacter->GetActorLocation());
		
		// If we are too far away, we cancel the aim.
		if (DistanceToEntrySquared > FMath::Square(AimingCancellationDistanceWhenEnteringCover))
		{
			PlayerCharacter->ResetAiming();
		}
	}
	
	return Super::NotifyBeginTravellingToCover(CoverToEnter, SectionToEnter);
}

void UPlayerCoverMovementComponent::NotifyReachedCoverTravellingTowards()
{
	Super::NotifyReachedCoverTravellingTowards();

	// If the player is trying to aim when they reach the cover, but for some reason they aren't doing it, we enable aim.
	if (IsValid(PlayerCharacter) && !PlayerCharacter->IsAiming() && PlayerCharacter->IsAimButtonHeld())
	{
		PlayerCharacter->StartAiming();
	}
}

void UPlayerCoverMovementComponent::BaseInCoverTickFunctionality(float DeltaTime)
{
	Super::BaseInCoverTickFunctionality(DeltaTime);

	if (!IsValid(PlayerCharacter))
	{
		return;
	}

	if (IsValid(CameraAdditionalHorizontalRatioSmoothComponent) && IsValid(PlayerCharacter->GetCameraSystem()))
	{
		// Setting the new Desired Screen Ratio value.
		PlayerCharacter->GetCameraSystem()->GetCameraState(FCameraStateKey::COVER).DesiredScreenRatio.X = DesiredCameraHorizontalRatio;
	}
}

void UPlayerCoverMovementComponent::AimInCoverTickFunctionality(float DeltaTime)
{
	Super::AimInCoverTickFunctionality(DeltaTime);

	if (!IsValid(PlayerCharacter))
	{
		return;
	}
	
	const UCameraComponent* Camera = PlayerCharacter->GetFollowCamera();
	const FVector CameraPosition = Camera->GetComponentLocation();
	const FVector ForwardVector = Camera->GetForwardVector();

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(PlayerCharacter);
	
	FHitResult HitResult;
	FVector AimingDirection;
	
	const bool bIsAimingAtAnObject = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		CameraPosition,
		CameraPosition + ForwardVector * DISTANCE_OF_LINE_TRACE_WHEN_CHECKING_AIMING_DIRECTION,
		ECollisionChannel::ECC_Camera,
		CollisionParams);
	
	if (bIsAimingAtAnObject && IsValid(GetCoverSplineCurrentlyUsing()))
	{
		// If the hit was far away enough.
		const bool bWasHitFarAwayEnough = HitResult.Distance > MinDistanceWithEnvironmentToAllowAim;
		
		// If the hit actor is the cover that we are shooting from (from a close-up distance).
		const bool bIsAimingAtCoverWeAreShootingFrom = HitResult.GetActor() != GetCoverSplineCurrentlyUsing()->TryGetParent();

		// If it is not the invisible cover that we are shooting from (from a close-up distance).
		const bool bIsAimingAtCoverWithEnvironmentTag = !HitResult.GetActor()->ActorHasTag(GetCoverSplineCurrentlyUsing()->ENVIRONMENT_COVER_TAG);		
		
		if (bWasHitFarAwayEnough || (!bIsAimingAtCoverWeAreShootingFrom && !bIsAimingAtCoverWithEnvironmentTag))
		{
			AimingDirection = HitResult.Location - PlayerCharacter->GetActorLocation();
		}
		else // If we are not meant to aim in this direction.
		{
			AimingDirection = PlayerCharacter->INVALID_IN_COVER_AIMING_DIRECTION;
		}
	}
	else // If we don't hit anything, we aim freely in the direction of the camera.
	{
		AimingDirection = CameraPosition + ForwardVector * DISTANCE_OF_LINE_TRACE_WHEN_CHECKING_AIMING_DIRECTION - PlayerCharacter->GetActorLocation();
	}

	PlayerCharacter->SetInCoverAimingDirection(AimingDirection);
}

void UPlayerCoverMovementComponent::OnRegister()
{
	Super::OnRegister();

	PlayerCharacter = Cast<APlayerCharacter>(GenericCharacter);

	if (IsValid(PlayerCharacter))
	{
		CameraAdditionalHorizontalRatioSmoothComponent = PlayerCharacter->GetInCoverCameraHorizontalAdditionalRatioSmooth();
		DesiredCameraHorizontalRatioInitialValue = PlayerCharacter->GetCameraSystem()->GetCameraState(FCameraStateKey::COVER).DesiredScreenRatio.X;
	}
}

void UPlayerCoverMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (!IsValid(PlayerCharacter))
	{
		return;
	}

	// Updating the Shoulder to Shoulder camera Transition.
	ChangeToLeftShoulderPercentageComplete += DeltaTime / ShoulderChangeCameraTransitionDuration * GetTargetOrientation();
	ChangeToLeftShoulderPercentageComplete = FMath::Clamp(ChangeToLeftShoulderPercentageComplete, 0.0f, 1.0f);

	// Updating the camera Horizontal Desired Screen Ratio.
	if (IsValid(CameraAdditionalHorizontalRatioSmoothComponent))
	{
		const FVector PlayerRightDirection = PlayerCharacter->GetActorRightVector();

		float TargetRatio = DesiredCameraHorizontalRatioInitialValue;

		// Adding additional displacement if the player is moving.
		CameraAdditionalHorizontalRatioSmoothComponent->UpdateWithEstimatedVelocity(abs(GetInCoverSpeed()) * MaxAdditionalCameraDisplacementWhileMoving, DeltaTime);
		TargetRatio -= CameraAdditionalHorizontalRatioSmoothComponent->GetSystemStoredValue();
		
		// Making the screen ratio change depending on the rotation of the camera in regard to the character.
		// This makes the camera return to the center of the screen when the camera is perpendicular to the character,
		// and makes the camera invert its position when looking at the character from its front.
		const float TargetRatioRight = MIDDLE_SCREEN_RATIO + (TargetRatio - MIDDLE_SCREEN_RATIO) * FVector::DotProduct(PlayerCharacter->GetFollowCamera()->GetRightVector().GetSafeNormal(), -PlayerRightDirection);
		const float TargetRatioLeft = MIDDLE_SCREEN_RATIO + (TargetRatio - MIDDLE_SCREEN_RATIO) * FVector::DotProduct(PlayerCharacter->GetFollowCamera()->GetRightVector().GetSafeNormal(), PlayerRightDirection);

		// Lerping between 2 possible TargetRatios based on the Shoulder Position.
		DesiredCameraHorizontalRatio = FMath::Lerp(TargetRatioRight, TargetRatioLeft,
			UEasingLibrary::ApplyEase(ChangeToLeftShoulderPercentageComplete, ShoulderPositionChangeCurveType));
	}
	
	// Here we manage the movement towards a cover before we are close enough to interpolate.
	if (IsTravellingToCover() && !IsMovementControlledByCover()) 	// This doesn't include when we move between covers, that we do at the GenericCharacter.cpp
	{
		const FVector TargetLocation = CoverSplineTravellingTowards->FindLocationAtTime(TemporaryUsedSectionOfSplineTravellingTowards);
		const FVector PlayerLocation = PlayerCharacter->GetActorLocation();
	
		if ((TargetLocation - PlayerLocation).SizeSquared2D() < FMath::Square(DistanceToBeginInterpolationToCover))
		{
			NotifyReachedCoverTravellingTowards(); // This will begin the interpolation inside GenericCharacter.cpp tick.
		}
		else
		{
			PlayerCharacter->AddMovementInput(FVector(TargetLocation.X - PlayerLocation.X, TargetLocation.Y - PlayerLocation.Y, 0).GetSafeNormal());
		}
	}
	else if (IsMovementControlledByCover() && (GetInsideCoverState() == EInsideCoverState::BaseCovered || GetInsideCoverState() == EInsideCoverState::Aiming))
	{
		// Checking if we are trying to exit cover via movement input.
		if (PlayerCharacter->GetPlayerDevice() == "KBM" && StoredInCoverInput.Y < CalculateMaxYInputBackwardsToExitCoverWithKeyboard()
			|| PlayerCharacter->GetPlayerDevice() != "KBM" && StoredInCoverInput.Y < CalculateMaxYInputBackwardsToExitCoverWithController()) // Is player trying to exit cover with input.
		{
			SecStorageTryingToExitCoverWithInput += DeltaTime;

			if (SecStorageTryingToExitCoverWithInput > SecsNeededToExitCoverWithInput)
			{
				NotifyExitCover();
			}
		}
		else
		{
			SecStorageTryingToExitCoverWithInput = 0;
		}
	}
}

bool UPlayerCoverMovementComponent::TryToTransitionOutOfCover()
{
	if (!IsValid(PlayerCharacter))
	{
		return false;
	}

	// If we need to have an Exit Transition because of an open shield while in Cover.
	if (IsValid(PlayerCharacter->GetUmbrella()) && PlayerCharacter->GetUmbrella()->IsOpen())
	{
		ExitCoverTransitionDirection = -CoverSplineCurrentlyUsing->FindForwardRotationAtTime(SectionOfCurrentlyUsingSpline).Vector();
		return true;
	}
	
	// Do we need an Exit Transition?
	const float CurrentSplineLength = CoverSplineCurrentlyUsing->GetSplineLength();

	const bool bIsAtLeftCoverEdge = CoverSplineCurrentlyUsing->IsSinglePointCover() || SectionOfCurrentlyUsingSpline * CurrentSplineLength < MAX_DISTANCE_FROM_EDGE_TO_PERFORM_EXIT_TRANSITION;
	const bool bIsAtRightCoverEdge = CoverSplineCurrentlyUsing->IsSinglePointCover() || (1 - SectionOfCurrentlyUsingSpline) * CurrentSplineLength < MAX_DISTANCE_FROM_EDGE_TO_PERFORM_EXIT_TRANSITION;
	const bool bExitingCoverThroughTheLeft = bIsAtLeftCoverEdge && (StoredInCoverInput.X < -MIN_X_INPUT_FOR_COVER_REACTION || TargetOrientation < 0);
	const bool bExitingCoverThroughTheRight = bIsAtRightCoverEdge && (StoredInCoverInput.X > MIN_X_INPUT_FOR_COVER_REACTION || TargetOrientation > 0);
	const bool bIsTryingToExitForward = StoredInCoverInput.Y > 0.15f;
	
	if (!CoverSplineCurrentlyUsing->IsLoop() && bIsTryingToExitForward)
	{
		if (bIsAtLeftCoverEdge && bExitingCoverThroughTheLeft)
		{
			ExitCoverTransitionDirection = CoverSplineCurrentlyUsing->FindLeftRotationAtTime(0).Vector();
			return true;
		}
		
		if (bIsAtRightCoverEdge && bExitingCoverThroughTheRight)
		{
			ExitCoverTransitionDirection = CoverSplineCurrentlyUsing->FindRightRotationAtTime(CoverSplineCurrentlyUsing->IsSinglePointCover() ? 0 : 1).Vector();
			return true;
		}
	}

	// Else we don't need an Exit Transition
	return false;
}

#pragma endregion
