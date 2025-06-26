// 
// CameraSystemComponent.cpp
// 
// Implementation of the CameraSystemComponent class.
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#include "CustomCameraSystemComponent.h"

#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Environment/CoverSystem/CoverParent.h"
#include "LadyUmbrella/Util/Logger.h"
#include "Shakes/LegacyCameraShake.h"
#include "Runtime/CinematicCamera/Public/CineCameraComponent.h"

UCustomCameraSystemComponent::UCustomCameraSystemComponent()
{
	bAutoActivate = true;
	
	// Curves
	ConstructorHelpers::FObjectFinder<UCurveFloat> YawCurveFloat(TEXT("/Script/Engine.CurveFloat'/Game/DataStructures/Curves/Camera/C_Yaw.C_Yaw'"));
	if (YawCurveFloat.Succeeded())
	{
		ManualYawCurve = YawCurveFloat.Object;
	}
	
	ConstructorHelpers::FObjectFinder<UCurveFloat> PitchCurveFloat(TEXT("/Script/Engine.CurveFloat'/Game/DataStructures/Curves/Camera/C_Pitch.C_Pitch'"));
	if (PitchCurveFloat.Succeeded())
	{
		ManualPitchCurve = PitchCurveFloat.Object;
	}

	// Shakes
	ConstructorHelpers::FClassFinder<ULegacyCameraShake> PlayerShake(TEXT("/Script/Engine.Blueprint'/Game/Blueprints/CameraShakes/BP_ShakePlayer.BP_ShakePlayer_C'"));
	if (PlayerShake.Succeeded())
	{
		CameraShakes.Add(FShakeSource::PLAYER, PlayerShake.Class);	
	}
	
	ConstructorHelpers::FClassFinder<ULegacyCameraShake> EnvShake(TEXT("/Script/Engine.Blueprint'/Game/Blueprints/CameraShakes/BP_ShakeEnv.BP_ShakeEnv_C'"));
	if (EnvShake.Succeeded())
	{
		CameraShakes.Add(FShakeSource::ENVIRONMENT, EnvShake.Class);
	}
	
	ConstructorHelpers::FClassFinder<ULegacyCameraShake> DefaultShake(TEXT("/Script/Engine.Blueprint'/Game/Blueprints/CameraShakes/BP_ShakeDefault.BP_ShakeDefault_C'"));
	if (DefaultShake.Succeeded())
	{
		CameraShakes.Add(FShakeSource::UNKNOWN, DefaultShake.Class);
	}
	
	// Camera States
	CurrentState	= FCameraStateKey::DEFAULT;
	
	RelativeLowCoverCollisioneHeightOffset = -100.f;
	
	CameraStates.Add(FCameraStateKey::DEFAULT, FCameraState());
	CameraStates.Add(FCameraStateKey::CINEMATIC, FCameraState());
	CameraStates.Add(FCameraStateKey::LOOKAT, FCameraState());
	CameraStates.Add(FCameraStateKey::COVER, FCameraState());
	CameraStates.Add(FCameraStateKey::AIM, FCameraState());
	
	CameraStates[FCameraStateKey::DEFAULT].Distance					= 200.f;
	CameraStates[FCameraStateKey::DEFAULT].DesiredScreenRatio		= FVector2D(0.33f,0.35f);
	CameraStates[FCameraStateKey::DEFAULT].BottomHitThreshold		= 0.2;
	CameraStates[FCameraStateKey::DEFAULT].SideHitThreshold			= 0.2;
	CameraStates[FCameraStateKey::DEFAULT].WallHitThreshold			= 0.2;
	CameraStates[FCameraStateKey::DEFAULT].InterpolationType		= ECameraInterpolation::EASE_O;
	CameraStates[FCameraStateKey::DEFAULT].InterpolationSpeed		= 3.5f;
	CameraStates[FCameraStateKey::DEFAULT].bAfterCollisionDelay		= true;
	CameraStates[FCameraStateKey::DEFAULT].DelaySeconds				= 0.7f;

	/*
	 * TODO: Cinematic events (custom camera for ziplines, reactive...) 
	CameraStates[FCameraStateKey::CINEMATIC].Distance				= 400.f;
	CameraStates[FCameraStateKey::CINEMATIC].bAfterCollisionDelay	= true;
	CameraStates[FCameraStateKey::CINEMATIC].DelaySeconds			= 0.5f;
	CameraStates[FCameraStateKey::CINEMATIC].InterpolationType		= ECameraInterpolation::SPRING;
	CameraStates[FCameraStateKey::CINEMATIC].InterpolationSpeed		= 8.f;
	
	* TODO: Look at functionality based on contextual input
	CameraStates[FCameraStateKey::LOOKAT].bFocusDistanceDynamic		= true;
	CameraStates[FCameraStateKey::LOOKAT].bCameraLag				= false;
	CameraStates[FCameraStateKey::LOOKAT].DesiredScreenRatio		= FVector2D(0.15f,0.5f);
	CameraStates[FCameraStateKey::LOOKAT].InterpolationType			= ECameraInterpolation::EASE_O;
	CameraStates[FCameraStateKey::LOOKAT].InterpolationSpeed		= 8.f;
	*/
	
	CameraStates[FCameraStateKey::COVER].Distance					= 325.f;
	CameraStates[FCameraStateKey::COVER].DesiredScreenRatio			= FVector2D(0.2f,0.35f);
	CameraStates[FCameraStateKey::COVER].InterpolationType			= ECameraInterpolation::EASE;
	CameraStates[FCameraStateKey::COVER].InterpolationSpeed			= 3.f;
	CameraStates[FCameraStateKey::COVER].bCameraLag					= false;
	
	CameraStates[FCameraStateKey::AIM].bFocusDistanceDynamic		= true;
	CameraStates[FCameraStateKey::AIM].Distance						= 50.f;
	CameraStates[FCameraStateKey::AIM].DesiredScreenRatio			= FVector2D(0.05f,0.1f);
	CameraStates[FCameraStateKey::AIM].BottomHitThreshold			= 0.3;
	CameraStates[FCameraStateKey::AIM].SideHitThreshold				= 0.3;
	CameraStates[FCameraStateKey::AIM].WallHitThreshold				= 0.3;
	CameraStates[FCameraStateKey::AIM].InterpolationType			= ECameraInterpolation::LINEAR;
	CameraStates[FCameraStateKey::AIM].InterpolationSpeed			= 3.5f;
	CameraStates[FCameraStateKey::AIM].bCameraLag					= false;
	
	// Parameters
	ResetStepSize				= 0.1f;
	CameraMultipliers.Add("KBM", FCameraMultipliers());
	CameraMultipliers.Add("XInputController", FCameraMultipliers());
	
	CameraCollisionRadius		= 40.f;
	TargetBoomLength			= 0.f;
	TargetOffset				= FVector::ZeroVector;
	CurrentOffset				= FVector::ZeroVector;
	ViewPitchRange				= FVector2D(-60,40);
	
	CameraMoveAssistStrength	= 0.5f;
	CurrentTargetYaw			= 0.f;
	YawAssistanceTimer			= 1.f;	
	YawAssistanceDelay			= 5.f;
	CameraSideMultiplier		= 2.f;
	DelayedCollisionTimer		= 0.f;
	InterpTimer					= 0.f;
	InterpolationStart_Dist		= 0.f;
	InterpolationStart_Offset	= FVector::ZeroVector;
	
	bCameraHasMotion			= false;
	bCameraIsResetting			= false;
	bYawAssistance				= false;
	bResetOnCover				= false;
}

void UCustomCameraSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	// Ignored Actors in regard to avoidance and collision
	TArray<AActor*> CameraTaggedActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("CameraIgnore"), CameraTaggedActors);
	for (AActor* CameraTaggedActor : CameraTaggedActors)
	{
		IgnoredActorsCamera.Add(CameraTaggedActor);
	}

	for (TTuple<FName, FCameraMultipliers>& StareMult : CameraMultipliers)
	{
		StareMult.Value.Sensibility = FMath::Clamp(StareMult.Value.Sensibility, StareMult.Value.SensibilityRange.X, StareMult.Value.SensibilityRange.Y);
		StareMult.Value.AimMultiplier = FMath::Clamp(StareMult.Value.AimMultiplier, StareMult.Value.MultiplierRange.X, StareMult.Value.MultiplierRange.Y);
		StareMult.Value.DefaultMultiplier = FMath::Clamp(StareMult.Value.DefaultMultiplier, StareMult.Value.MultiplierRange.X, StareMult.Value.MultiplierRange.Y);
	}
}

void UCustomCameraSystemComponent::SetUpCamera(USpringArmComponent* Arm, UCineCameraComponent* Cam)
{
	CameraBoom = Arm;
	FollowCamera = Cam;
	ResetCameraParams();
	
	InterpolationStart_Dist = CameraCollisionRadius;
	float TempMin = ViewPitchRange.GetMin();
	ViewPitchRange.Y = ViewPitchRange.GetMax();
	ViewPitchRange.X = TempMin;
}

void UCustomCameraSystemComponent::CameraTick(const float& DeltaSeconds)
{
	if (IsValid(CameraBoom) && IsValid(FollowCamera))
	{
		UpdateForCameraState(DeltaSeconds);
		if (bCameraIsResetting)
		{
			ResetCamProgress(GetWorld()->GetDeltaSeconds());
		}
		ManageCameraCollisions();
		UpdateCameraPosition(DeltaSeconds);
	}
}

void UCustomCameraSystemComponent::ResetCameraParams()
{
	if (!IsValid(CameraBoom) || !IsValid(FollowCamera))
	{
		UE_LOG(LogTemp, Warning, TEXT("CameraBoom or FollowCamera is invalid. Aborting ResetCameraParams."));
		return;
	}
	
	CurrentState = FCameraStateKey::DEFAULT;

	TargetOffset = FVector::ZeroVector;
	CameraBoom->TargetOffset = FVector::ZeroVector;
	CameraBoom->TargetArmLength = CameraStates[CurrentState].Distance;
	
	FollowCamera->FocusSettings.FocusMethod = ECameraFocusMethod::Manual;
	FollowCamera->FocusSettings.ManualFocusDistance = MaxFocusDist;
	FollowCamera->FocusSettings.bSmoothFocusChanges = true;
	
	YawAssistanceTimer = YawAssistanceDelay;
	DelayedCollisionTimer = CameraStates[CurrentState].DelaySeconds;
	bCameraHasMotion = false;
	
	APlayerCameraManager* CameraManager = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
	if (IsValid(CameraManager))
	{
		CameraManager->ViewPitchMin = ViewPitchRange.X;
		CameraManager->ViewPitchMax = ViewPitchRange.Y;
	}
}

#pragma region SENSIBILITY

void UCustomCameraSystemComponent::UpdateCurrentControlSensitivity(FString DeviceType, float NewValue)
{
	FCameraMultipliers* TargetModifiers = &CameraMultipliers[FName(DeviceType)];
	TargetModifiers->Sensibility = FMath::Clamp(TargetModifiers->Sensibility, TargetModifiers->SensibilityRange.X, TargetModifiers->SensibilityRange.Y);
}

void UCustomCameraSystemComponent::UpdateModifier(EModifierType Modifier, TArray<float> NewValues)
{
	TArray<FName> Keys;
	CameraMultipliers.GetKeys(Keys);
	switch (Modifier)
	{
	case EModifierType::DEFAULT:
		for (int Index = 0; Index < CameraMultipliers.Num(); Index++)
		{
			FCameraMultipliers* Current = &CameraMultipliers[Keys[Index]];
			Current->DefaultMultiplier = FMath::Clamp(NewValues[Index],Current->MultiplierRange.X,Current->MultiplierRange.Y);
		}
		break;
		
	case EModifierType::AIM:
		for (int Index = 0; Index < CameraMultipliers.Num(); Index++)
		{
			FCameraMultipliers* Current = &CameraMultipliers[Keys[Index]];
			Current->AimMultiplier = FMath::Clamp(NewValues[Index],Current->MultiplierRange.X,Current->MultiplierRange.Y);
		}
		break;
		
	case EModifierType::YAW:
		for (int Index = 0; Index < CameraMultipliers.Num(); Index++)
		{
			CameraMultipliers[Keys[Index]].InvertYaw = NewValues[Index] < 0;
		}
		break;
		
	case EModifierType::PITCH:
		for (int Index = 0; Index < CameraMultipliers.Num(); Index++)
		{
			CameraMultipliers[Keys[Index]].InvertPitch = NewValues[Index] < 0;
		}
		break;
	}
}

void UCustomCameraSystemComponent::UpdateModifier(FName DeviceType, EModifierType Modifier, float NewValue)
{
	FCameraMultipliers* TargetModifiers = &CameraMultipliers[FName(DeviceType)];
	
	switch (Modifier)
	{
		case EModifierType::DEFAULT:
			TargetModifiers->DefaultMultiplier = FMath::Clamp(NewValue,TargetModifiers->MultiplierRange.X,TargetModifiers->MultiplierRange.Y);
			break;
		
		case EModifierType::AIM:
			TargetModifiers->AimMultiplier = FMath::Clamp(NewValue,TargetModifiers->MultiplierRange.X,TargetModifiers->MultiplierRange.Y);
			break;
		
		case EModifierType::YAW:
			TargetModifiers->InvertYaw = NewValue < 0;
			break;
		
		case EModifierType::PITCH:
			TargetModifiers->InvertPitch = NewValue < 0;
			break;
	}
}

#pragma endregion 

#pragma region CONTROL

FVector2D UCustomCameraSystemComponent::GetAdjustedLookVector(FString DeviceType, FVector2D LookAxisVector)
{
	FCameraMultipliers ActiveMultipliers = CameraMultipliers[FName(DeviceType)];
	
	float ResultX = LookAxisVector.X;
	float ResultY = LookAxisVector.Y;

	if (ActiveMultipliers.bSensibilityFitsCurve)
	{
		if (IsValid(ManualYawCurve) && IsValid(ManualPitchCurve))
		{
			ResultX = ManualYawCurve->GetFloatValue(ResultX);
			ResultY = ManualYawCurve->GetFloatValue(ResultY);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Yaw or Pitch Curve are invalid. Unable to do adjust Input to curve."));
		}
	}
	
	ResultX *= ((ActiveMultipliers.InvertYaw) ? -1 : 1) * ActiveMultipliers.Sensibility;
	ResultY *= ((ActiveMultipliers.InvertPitch) ? -1 : 1) * ActiveMultipliers.Sensibility;

	if (GetCurrentCameraStateKey() == FCameraStateKey::AIM)
	{
		ResultX *= ActiveMultipliers.AimMultiplier;
		ResultY *= ActiveMultipliers.AimMultiplier;
	}
	else
	{
		ResultX *= ActiveMultipliers.DefaultMultiplier;
		ResultY *= ActiveMultipliers.DefaultMultiplier;
	}

	if (GetCurrentCameraStateKey() == FCameraStateKey::COVER)
	{
		FHitResult CameraCoverHit;
		const float ProvingDistance = 200.f;
		UCharacterCoverMovementComponent* CoverMovComponent = Cast<UCharacterCoverMovementComponent>(GetOwner()->GetComponentByClass(UCharacterCoverMovementComponent::StaticClass()));
		bool bLowCover = IsValid(CoverMovComponent) ? !CoverMovComponent->IsInTallCover() : false;
		FVector Start = CameraBoom->GetComponentLocation() + FVector(0.f, 0.f, bLowCover ?  RelativeLowCoverCollisioneHeightOffset : 0.f);
		FVector End = CameraBoom->GetComponentLocation() + (FollowCamera->GetComponentLocation() - CameraBoom->GetComponentLocation()).GetSafeNormal() * ProvingDistance;
		End.Z = Start.Z;
		bool bHasCollided = UKismetSystemLibrary::LineTraceSingle(
		GetWorld(),
		Start, // I know this is a magic number, will fix later (pls don't kill the PR)
		End,
		ETraceTypeQuery::TraceTypeQuery1,
		true,
		IgnoredActorsCamera,
		EDrawDebugTrace::None,
		CameraCoverHit,
		true,
		FColor::Yellow,
		FColor::Green,
		1.f);	

		const float SideDot		= FVector::DotProduct(FollowCamera->GetRightVector().GetSafeNormal(), CameraCoverHit.Normal);
		const float UpDot		= FVector::DotProduct(FollowCamera->GetUpVector().GetSafeNormal(), CameraCoverHit.Normal);
		
		bool bCoverCollitionAgainstMovement = (SideDot * ResultX) > 0;
		bool bFloorCollitionAgainstMovement = (UpDot * ResultY) > 0;

		if (bHasCollided)
		{
			/* Deprecated, remains just in case something similar is needed
			 AActor* Cover = CameraCoverHit.GetActor();
			if (IsValid(Cover) && Cover->IsA(ACoverParent::StaticClass()) && bCoverCollitionAgainstMovement)
			{
				ResultX = 0.f;
			}*/
			/* Needs further testing
			if (!bFloorCollitionAgainstMovement)
			{
				FLogger::WarningLog("Cannot move anymore towards that direction (Hitting Floor)");
				ResultY = 0.f;
			}
			*/
			
		}
	}
	
	return FVector2D(ResultX, ResultY);
}

void UCustomCameraSystemComponent::MoveAssistance(FVector2D MoveAxisVector, bool bMoveControlledByCover)
{
	if (bCameraHasMotion || GetCurrentCameraStateKey() != FCameraStateKey::DEFAULT)
	{
		UE_LOG(LogTemp, Warning, TEXT("Unable to do Move Assistance."));
		return;
	}

	FVector2D NormalizedMovementVector = MoveAxisVector.GetSafeNormal();

	if (YawAssistanceTimer <= 0.f && !bMoveControlledByCover)
	{
		GetWorld()->GetFirstPlayerController()->AddYawInput(NormalizedMovementVector.X * CameraMoveAssistStrength);
	}
}

void UCustomCameraSystemComponent::SetCameraState(FCameraStateKey StateKey)
{
	if(CameraStates.Contains(StateKey))
	{
		CurrentState = StateKey;
	}
	//CameraBoom->SetRelativeLocation(FVector(0, 0, RelativeHeights[StateKey]));
	InterpolationStart_Dist = CameraBoom->TargetArmLength;
	InterpolationStart_Offset = CurrentOffset;
	DelayedCollisionTimer = 0.f;
	InterpTimer = 0.f;
	if (GetCurrentCameraStateKey() == FCameraStateKey::COVER)
	{
		if (bResetOnCover)
		{
			ResetCameraPosition();
		}
		CameraBoom->SocketOffset;
	}
}

void UCustomCameraSystemComponent::SetCameraStateDistance(FCameraStateKey StateKey, float Distance2Player)
{
	if (CameraStates[StateKey].Distance != Distance2Player)
	{
		CameraStates[StateKey].Distance = Distance2Player;
		InterpolationStart_Dist = CameraBoom->TargetArmLength;
		InterpTimer = 0.f;
	}
}

void UCustomCameraSystemComponent::SetCameraStateShoulder(FCameraStateKey StateKey, bool InvertedShoulder)
{
	if (CameraStates[StateKey].bScreenRatioMirrored != InvertedShoulder)
	{
		CameraStates[StateKey].bScreenRatioMirrored = InvertedShoulder;
		InterpolationStart_Offset = CurrentOffset;
		InterpTimer = 0.f;
	}
}

void UCustomCameraSystemComponent::SetCameraStateOffsetValue(FCameraStateKey StateKey, FVector2D Ratio)
{
	if (CameraStates[StateKey].DesiredScreenRatio != Ratio)
	{
		CameraStates[StateKey].DesiredScreenRatio.X = Ratio.X;
		CameraStates[StateKey].DesiredScreenRatio.Y = Ratio.Y;
		InterpolationStart_Offset = CurrentOffset;
		InterpTimer = 0.f;
	}
}

void UCustomCameraSystemComponent::SetCameraStateValues(FCameraStateKey StateKey, float Distance2Player,
	bool InvertedOffset)
{
	SetCameraStateDistance(StateKey, Distance2Player);
	SetCameraStateShoulder(StateKey, InvertedOffset);
}

void UCustomCameraSystemComponent::ResetCameraPosition()
{
	GetWorld()->GetFirstPlayerController()->DisableInput(GetWorld()->GetFirstPlayerController());
	bCameraIsResetting = true;
}

void UCustomCameraSystemComponent::ResetCamProgress(const double& DeltaSeconds)
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!IsValid(PlayerController))
	{
		return;
	}

	FRotator ControlRot = PlayerController->GetControlRotation();
	FQuat ControlQuat = ControlRot.Quaternion();

	const float AngularDistance = FMath::RadiansToDegrees(UKismetMathLibrary::Quat_AngularDistance(ControlQuat, ControlQuat));
	const float AngularThreshold = 1.0f;
	
	if (AngularDistance > AngularThreshold)
	{
		FQuat InterpolatedQuat = FQuat::Slerp(ControlQuat, ControlQuat, DeltaSeconds * ResetStepSize);
        PlayerController->SetControlRotation(InterpolatedQuat.Rotator());
	}
	else
	{
		PlayerController->EnableInput(PlayerController);
		bCameraIsResetting = false;
	}
}

void UCustomCameraSystemComponent::SetCurrentDistanceAsArmLength()
{
	CameraBoom->TargetArmLength = FVector::Distance(CameraBoom->GetComponentLocation(), FollowCamera->GetComponentLocation());
}

#pragma endregion 

#pragma region STATES

void UCustomCameraSystemComponent::UpdateForCameraState(const float& DeltaSeconds)
{
	FCameraState State = GetCurrentCameraState();
	TargetOffset = ComputeOffset(State);
	FollowCamera->SetUseFieldOfViewForLOD(State.bUseFoVForLOD);
}

#pragma endregion 

#pragma region SHAKES

void UCustomCameraSystemComponent::MakeCameraShake(const FShakeSource& Type, FVector Location, float RadiusFrom, float RadiusTo)
{
	if(Location.IsNearlyZero())
	{
		Location = GetOwner()->GetActorLocation();
	}
	
	UGameplayStatics::PlayWorldCameraShake(
		GetWorld(),
		CameraShakes[Type],
		Location,
		RadiusFrom,
		RadiusTo,
		1.f,
		true);
}

#pragma endregion 

#pragma region COLLISION

void UCustomCameraSystemComponent::UpdateCameraPosition(const float& DeltaSeconds)
{
	FCameraState CurrentCameraState = GetCurrentCameraState();
	CameraBoom->bEnableCameraLag = CurrentCameraState.bCameraLag;	
	
	// Arm Length & Offset
	FVector DesiredOffset = CurrentOffset;
	float DesiredArmLength = CameraBoom->TargetArmLength;
	float Progress = InterpolateFloat();
	
	DesiredOffset = CurrentOffset.Equals(TargetOffset)
	? TargetOffset
	: FMath::Lerp(InterpolationStart_Offset,TargetOffset, Progress);

	DesiredArmLength = FMath::IsNearlyEqual(CameraBoom->TargetArmLength, TargetBoomLength)
	? TargetBoomLength
	: FMath::Lerp(InterpolationStart_Dist,TargetBoomLength, Progress);

	if (InterpTimer == 1.f)
	{
		InterpolationStart_Dist = CameraBoom->TargetArmLength;
		InterpolationStart_Offset = CurrentOffset;
		CameraBoom->bEnableCameraLag = false;
	}
	
	ManageArmExpectedValue(DesiredArmLength, DesiredOffset);
	
	// TODO: if (onStairs) -> Smooth Bumpy Behaviour (https://github.com/mrezai/GodotStairs/blob/master/Player/Player.gd) (https://youtu.be/Tb-R3l0SQdc?t=1699)
	// TODO: if (onRamp) -> ?
	// TODO: if (able2LookAt) -> Lose Some Camera Control (New Asset)
	// TODO: if (cameraEvent) -> Completely Lose Camera Control (New Asset)
	
	// Focus Distance
	if (CurrentCameraState.bFocusDistanceDynamic)
	{
		TArray<AActor*> AttachedActors;
		GetOwner()->GetAttachedActors(AttachedActors);

		FHitResult HitForFocus;
		bool bAimingAtObject = UKismetSystemLibrary::LineTraceSingle(
			GetWorld(),
			FollowCamera->GetComponentLocation(),
			FollowCamera->GetComponentLocation() + FollowCamera->GetForwardVector() * MaxFocusDist,
			ETraceTypeQuery::TraceTypeQuery1,
			false,
			AttachedActors,
			EDrawDebugTrace::None,
			HitForFocus,
			true);
			
		if (bAimingAtObject)
		{
			FollowCamera->FocusSettings.ManualFocusDistance = HitForFocus.Distance;
		}
		else
		{
			FollowCamera->FocusSettings.ManualFocusDistance = MaxFocusDist;
		}
	}
	else
	{
		float DistPlayerFocus = FVector::Distance(CameraBoom->GetComponentLocation(), FollowCamera->GetComponentLocation());
		DistPlayerFocus += DistPlayerFocus < CameraCollisionRadius ? 0 : (TargetBoomLength * CameraBoom->TargetArmLength/TargetBoomLength) - CameraCollisionRadius;
		FollowCamera->FocusSettings.ManualFocusDistance = DistPlayerFocus;
	}

	// Timers
	if (bYawAssistance)
	{
		YawAssistanceTimer -= DeltaSeconds;
	}
	if (CurrentCameraState.bAfterCollisionDelay)
	{
		DelayedCollisionTimer -= DeltaSeconds;
	}
	const bool bCollisionDelayResolved	= !CurrentCameraState.bAfterCollisionDelay || DelayedCollisionTimer <= 0.f;
	if (InterpTimer < 1.f && (bCollisionDelayResolved || InterpolationStart_Dist > TargetBoomLength))
	{
		InterpTimer += DeltaSeconds * CurrentCameraState.InterpolationSpeed;
	}
	
	//FLogger::InfoLog("<%.2f> Start %.1f | Target Dist %.1f", InterpTimer, InterpolationStart_Dist, TargetBoomLength);
}

void UCustomCameraSystemComponent::ManageCameraCollisions()
{
	FCameraState State = GetCurrentCameraState();

	const FVector Start = CameraBoom->GetComponentLocation();
	const FVector End	= Start + CameraBoom->TargetOffset - CameraBoom->GetTargetRotation().Vector() * CameraBoom->TargetArmLength;

	CameraHits.Empty();
	bool bHasCollided = UKismetSystemLibrary::SphereTraceMulti(
		GetWorld(),
		Start,
		End,
		CameraCollisionRadius,
		ETraceTypeQuery::TraceTypeQuery1,
		true,
		IgnoredActorsCamera,
		EDrawDebugTrace::None,
		CameraHits,
		true);
	
	if (bHasCollided)
	{
		if (State.bAfterCollisionDelay)
		{
			DelayedCollisionTimer = State.DelaySeconds;
		}

		if (!FMath::IsNearlyEqual(InterpolationStart_Dist, CameraBoom->TargetArmLength))
		{
			InterpolationStart_Dist = CameraBoom->TargetArmLength;
		}
		InterpolationStart_Offset = CurrentOffset;
		
		FHitResult SynthesizedHit;
		SynthesizedHit.Normal		= FVector::ZeroVector;
		SynthesizedHit.Location		= FVector::ZeroVector;
		SynthesizedHit.ImpactPoint	= FVector::ZeroVector;

		for (const FHitResult& Hit : CameraHits)
		{
			SynthesizedHit.Normal		+= Hit.Normal;
			SynthesizedHit.Location		+= Hit.Location;
			SynthesizedHit.ImpactPoint	+= Hit.ImpactPoint;
		}

		SynthesizedHit.Normal		/= CameraHits.Num();
		SynthesizedHit.Location		/= CameraHits.Num();
		SynthesizedHit.ImpactPoint	/= CameraHits.Num();
		
		EvaluateCollision(SynthesizedHit);
	}
	else
	{
		TargetBoomLength = State.Distance;
		CameraBoom->bDoCollisionTest = false;
	}
	
}

FVector UCustomCameraSystemComponent::ComputeOffset(const FCameraState& CurrState)
{
	if (!IsValid(GetWorld()->GetFirstPlayerController()))
	{
		return FVector::ZeroVector;
	}
	
	int32 ViewportWidth = 0, ViewportHeight = 0;
	GetWorld()->GetFirstPlayerController()->GetViewportSize(ViewportWidth,ViewportHeight);
	
	const float DesiredPercentX = CurrState.bScreenRatioMirrored ? (1.f - CurrState.DesiredScreenRatio.X) : CurrState.DesiredScreenRatio.X;
	const float DesiredPercentY = CurrState.DesiredScreenRatio.Y;
	const float RadiansFoV		= FMath::DegreesToRadians(FollowCamera->FieldOfView);
	const float FocalLenght		= (ViewportWidth / 2.0f) / FMath::Tan(RadiansFoV / 2.0f);

	// Perspective Projection supposing that Screen X is within [-ViewportWidth/2 to +ViewportWidth/2] being 0 the center
	const float RawScreenOffsetY = (0.5f - DesiredPercentX) * ViewportWidth;
	const float RawScreenOffsetZ = (DesiredPercentY - 0.5f) * ViewportHeight;
		
	float TargetOffsetY = RawScreenOffsetY * (CurrState.Distance / FocalLenght);
	float TargetOffsetZ = RawScreenOffsetZ * (CurrState.Distance / FocalLenght);
	
	return FVector(0.f, TargetOffsetY, TargetOffsetZ);
}

void UCustomCameraSystemComponent::EvaluateCollision(const FHitResult& CollisionResult)
{
	const FVector HitNormal		 = CollisionResult.Normal;
	const FCameraState CurrState = GetCurrentCameraState();

	const float SideDot		= FVector::DotProduct(FollowCamera->GetRightVector().GetSafeNormal(), HitNormal);
	const float ForwardDot	= FVector::DotProduct(FollowCamera->GetForwardVector().GetSafeNormal(), HitNormal);
	const float UpDot		= FVector::DotProduct(FollowCamera->GetUpVector().GetSafeNormal(), HitNormal);

	const float ProcessedSide = FMath::Abs(SideDot)/CurrState.SideHitThreshold;
	const float ProcessedFwd = ForwardDot/CurrState.WallHitThreshold;
	const float ProcessedUp = UpDot/CurrState.BottomHitThreshold;
	
	// Checking for most significant collision type
	bool bWallBehind		= ProcessedFwd > ProcessedUp && ProcessedFwd > ProcessedSide;
	bool bBottomCollision	= !bWallBehind && ProcessedUp > ProcessedSide;
	bool bSideCollision		= !(bWallBehind || bBottomCollision);

	//if (bWallBehind) FLogger::DebugLog("Most Significant collision is Back");
	//if (bBottomCollision) FLogger::DebugLog("Most Significant collision is Down");
	//if (bSideCollision) FLogger::DebugLog("Most Significant collision is Side");
	
	if (bWallBehind || bBottomCollision || (bSideCollision && GetCurrentCameraStateKey() != FCameraStateKey::COVER))
	{
		CameraCorrectiveMovement(CollisionResult);
	}
	else if (bSideCollision)
	{
		const bool SideCollisionFromRight = SideDot > 0.f;
		//ClampLateralOffset(TargetOffset, CollisionResult.Location, SideCollisionFromRight);	
	}
}

void UCustomCameraSystemComponent::CameraCorrectiveMovement(const FHitResult& Hit)
{
	// Projecting Hit Location into Spring Arm Vector
	const FVector End(CameraBoom->GetComponentLocation() + CameraBoom->TargetOffset - CameraBoom->GetTargetRotation().Vector() * CameraBoom->TargetArmLength);
	const FVector OriginVector(End - CameraBoom->GetComponentLocation());
	const FVector HitVector(Hit.ImpactPoint - (OriginVector.GetSafeNormal() * CameraCollisionRadius) - CameraBoom->GetComponentLocation());
	const FVector ProjectedPoint(CameraBoom->GetComponentLocation() + FVector::DotProduct(HitVector, OriginVector) / FVector::DotProduct(OriginVector, OriginVector) * OriginVector);

	// Setting Arm Length as distance to Projected Point
	float Dist2Point = (ProjectedPoint - CameraBoom->GetComponentLocation()).Size();
	//FLogger::DebugLog("[EVALUATING CHANGE] %.1f -- TO --> %.1f", TargetBoomLength,  Dist2Point);
	if (TargetBoomLength > Dist2Point)
	{
		if (FMath::IsNearlyEqual(GetCurrentCameraState().Distance, TargetBoomLength))
		{
			InterpTimer = 0.f;
		}

		if (Dist2Point > 40.f)
		{
			TargetBoomLength = Dist2Point;
		}
		else
		{
			TargetBoomLength = CameraCollisionRadius;
		}
		//FLogger::DebugLog("[NEW] <%.2f> Start %.1f | Target Dist %.1f", InterpTimer, InterpolationStart_Dist, TargetBoomLength);
	}
	
	CameraBoom->bDoCollisionTest = true;
}

FVector UCustomCameraSystemComponent::GetOffsetFromDistance(const float BoomDistance, const FVector TheoreticalOffset) const
{
	const float ExpectedLateralOffset = TheoreticalOffset.Y * FMath::Min(1.f, BoomDistance / GetCurrentCameraState().Distance);
	const float ExpectedHeightOffset = TheoreticalOffset.Z * FMath::Min(1.f, BoomDistance / GetCurrentCameraState().Distance);
	return FVector(
		ExpectedLateralOffset * -CameraBoom->GetTargetRotation().Vector().Y,
		ExpectedLateralOffset * CameraBoom->GetTargetRotation().Vector().X,
		ExpectedHeightOffset);
}

float UCustomCameraSystemComponent::GetEquivalentDistanceFromCurrentOffset() const
{
	const FVector2D LateralDerivedDist = FVector2D(CameraBoom->TargetOffset.X / -CameraBoom->GetTargetRotation().Vector().Y, CameraBoom->TargetOffset.Y / CameraBoom->GetTargetRotation().Vector().X) / CurrentOffset.Y * GetCurrentCameraState().Distance;
	const float HeightDerivedDist = CameraBoom->TargetOffset.Z / CurrentOffset.Z * GetCurrentCameraState().Distance;

	return HeightDerivedDist < CameraCollisionRadius ? CameraCollisionRadius : HeightDerivedDist;
}


void UCustomCameraSystemComponent::ManageArmExpectedValue(const float ExpectedDistance, FVector ExpectedOffset) 
{
	FCameraState CurrState = GetCurrentCameraState();
	
	const FVector ComputedExpectedOffset = GetOffsetFromDistance(ExpectedDistance, ExpectedOffset);
	const FVector ExpectedEnd = CameraBoom->GetComponentLocation() + ComputedExpectedOffset - CameraBoom->GetTargetRotation().Vector() * ExpectedDistance;
	
	FHitResult CorrectiveHit;
	const bool bGoingForward			= ExpectedDistance < CameraBoom->TargetArmLength;
	const bool bCollisionDelayResolved	= !CurrState.bAfterCollisionDelay || DelayedCollisionTimer <= 0.f;
	bool bCorrectiveCollision			= UKismetSystemLibrary::SphereTraceSingle(
		GetWorld(),
		FollowCamera->GetComponentLocation(),
		ExpectedEnd,
		CameraCollisionRadius * 1.1f,
		ETraceTypeQuery::TraceTypeQuery1,
		true,
		IgnoredActorsCamera,
		EDrawDebugTrace::None,
		CorrectiveHit,
		true);

	if (bCorrectiveCollision && !bGoingForward)
	{
		if (CurrState.bAfterCollisionDelay)
		{
			DelayedCollisionTimer = CurrState.DelaySeconds;
		}
		InterpolationStart_Dist = CameraBoom->TargetArmLength;
		InterpolationStart_Offset = CurrentOffset;
	}
	
	if (bGoingForward)
	{
		CameraBoom->TargetArmLength = ExpectedDistance < CameraCollisionRadius ? CameraCollisionRadius : ExpectedDistance;
		CurrentOffset = ExpectedOffset;
	}
	else if (!bCorrectiveCollision && bCollisionDelayResolved)
	{
		CameraBoom->TargetArmLength = ExpectedDistance;
		CurrentOffset = ExpectedOffset;
	}

	FVector NewProcessedOffset = GetOffsetFromDistance(CameraBoom->TargetArmLength, CurrentOffset);
	if (!bGoingForward && !bCorrectiveCollision && GetEquivalentDistanceFromCurrentOffset() > CameraBoom->TargetArmLength)
	{
		NewProcessedOffset = GetOffsetFromDistance(GetEquivalentDistanceFromCurrentOffset(), CurrentOffset);
	}
	CameraBoom->TargetOffset = NewProcessedOffset;
}

void UCustomCameraSystemComponent::ClampLateralOffset(FVector& DesiredOffset, const FVector& HitLocation, const bool FromRight) const
{
	FLogger::InfoLog("[UNTESTED] Clamping Lateral Offset");
	const FVector CameraLocation = FollowCamera->GetComponentLocation();
	const FVector CameraRight = FollowCamera->GetRightVector();
	const FVector ToCollision = HitLocation - CameraLocation;

	// Clamp Lateral offset to side collision hit
	const float HitRightDist = FVector::DotProduct(ToCollision, CameraRight);
	const float DesiredRightDist = FVector::DotProduct(DesiredOffset, CameraRight);
	const float Excess = DesiredRightDist - HitRightDist;

	// Only if received the collision for the shoulder offset side
	if ((DesiredRightDist > 0 && Excess > 0) || (DesiredRightDist < 0 && Excess < 0))
	{
		DesiredOffset -= CameraRight * Excess;
	}
}

#pragma endregion 

float UCustomCameraSystemComponent::GetFieldOfView() const
{
	return GetFollowCamera()->FieldOfView;
}

bool UCustomCameraSystemComponent::IsActorInFrustum(const AActor* Actor) const
{
	ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (LocalPlayer != nullptr && LocalPlayer->ViewportClient != nullptr && LocalPlayer->ViewportClient->Viewport)
	{
		FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
			LocalPlayer->ViewportClient->Viewport,
			GetWorld()->Scene,
			LocalPlayer->ViewportClient->EngineShowFlags)
			.SetRealtimeUpdate(true));

		FVector ViewLocation;
		FRotator ViewRotation;
		FSceneView* SceneView = LocalPlayer->CalcSceneView(&ViewFamily, ViewLocation, ViewRotation, LocalPlayer->ViewportClient->Viewport);
		if (SceneView != nullptr)
		{
			return SceneView->ViewFrustum.IntersectSphere(
						Actor->GetActorLocation(), Actor->GetSimpleCollisionRadius());
		}			 
	}

	return false;
}

bool UCustomCameraSystemComponent::IsComponentInFrustum(const UPrimitiveComponent* Component) const
{
	ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (LocalPlayer != nullptr && LocalPlayer->ViewportClient != nullptr && LocalPlayer->ViewportClient->Viewport)
	{
		FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
			LocalPlayer->ViewportClient->Viewport,
			GetWorld()->Scene,
			LocalPlayer->ViewportClient->EngineShowFlags)
			.SetRealtimeUpdate(true));

		FVector ViewLocation;
		FRotator ViewRotation;
		FSceneView* SceneView = LocalPlayer->CalcSceneView(&ViewFamily, ViewLocation, ViewRotation, LocalPlayer->ViewportClient->Viewport);
		if (SceneView != nullptr)
		{
			return SceneView->ViewFrustum.IntersectSphere(
						Component->GetComponentLocation(), 50.f);
		}			 
	}

	return false;
}

bool UCustomCameraSystemComponent::IsOutsideFrustum(const AActor* Actor, const float Tolerance) const
{
	const APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	if (!IsValid(Player))
	{
		return true;
	}
	
	const FVector Forward = Player->GetFollowCamera()->GetForwardVector();
	const FVector Difference = (Actor->GetActorLocation() - Player->GetFollowCamera()->GetComponentLocation()).GetSafeNormal();
	
	const float DotProduct = FVector::DotProduct(Forward, Difference);
	const float FrustumLimit = 1.0f - (Player->GetCameraSystem()->GetFieldOfView() / 360.0f) - Tolerance;
	
	// FLogger::DebugLog("%f, %f", DotProduct, FrustumLimit);
	return (DotProduct < FrustumLimit);
	
}

#pragma region INTERPOLATION

float UCustomCameraSystemComponent::InterpolateFloat()
{
	const FCameraState CurrentCameraState = GetCurrentCameraState();
	const float StepPercent = FMath::Clamp(InterpTimer, 0.f, 1.f);
	switch (CurrentCameraState.InterpolationType)
	{
		case ECameraInterpolation::EASE:
			return FMath::SmoothStep(0.f, 1.f, StepPercent);

		case ECameraInterpolation::EASE_I:
			return FMath::InterpEaseIn(0.f, 1.f, StepPercent, CurrentCameraState.EaseExponent);

		case ECameraInterpolation::EASE_O:
			return FMath::InterpEaseOut(0.f, 1.f, StepPercent, CurrentCameraState.EaseExponent);
		
		case ECameraInterpolation::EASE_IO:
			return FMath::InterpEaseInOut(0.f, 1.f, StepPercent, CurrentCameraState.EaseExponent);
		
		case ECameraInterpolation::SPRING_I:
			return ElasticEaseInLerp(0.f, 1.f, StepPercent);
		
		case ECameraInterpolation::SPRING_O:
			return ElasticEaseOutLerp(0.f, 1.f, StepPercent);
		
		case ECameraInterpolation::SPRING_IO:
			return ElasticEaseInOutLerp(0.f, 1.f, StepPercent);

		case ECameraInterpolation::LINEAR:
			return FMath::Lerp(0.f, 1.f, StepPercent);
		
		case ECameraInterpolation::CUSTOM:
				return UE::Curves::BezierInterp(CurrentCameraState.BezierPoints.X,CurrentCameraState.BezierPoints.Y,CurrentCameraState.BezierPoints.Z, CurrentCameraState.BezierPoints.W, StepPercent);

		default:
			return 1.f;
	}
}
#pragma endregion 
