// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractiveMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "MotionWarpingComponent.h"
#include <RootMotionModifier.h>

#include "EngineUtils.h"
#include "GenericCharacter.h"
#include "Enemies/EnemyCharacter.h"
#include "LadyUmbrella/Interfaces/InteractiveMovement.h"
#include "LadyUmbrella/Util/Logger.h"

// Sets default values for this component's properties
UInteractiveMovementComponent::UInteractiveMovementComponent()
	: SurfaceNormalZValue(1)
	, MantleSphereTraceOffset(85.f)
	, StickToWallDistance(150.f)
	, MantleDistance(50.f)
	, MantleAnimPlayRate(1)
	, SphereTraceRadius(10.f)
	, SphereTraceVerticalOffset(FVector(0, 0, 50))
	, SphereTraceLength(100.f)
	, SphereTraceStep(25)
	, MinimumLandSpace(80)
	, CheckVaultDistance(100.f)
	, MinVaultableLength(25.f)
	, VaultOverHeight(100.f)
	, VaultAnimPlayRate(1)
	, ShortVaultAnimPlayRate(1)
	, LongVaultAnimPlayRate(0.4f)
	, MaxVaultDistance(100.f)
	, LongVaultDistance(300)
	, bShortVault(false)
	, ShortVaultDistance(100.f)
	, VaultLandTraceLenght(400.1f)
	, bIsVoidVault(false)
	, KickedBoxTraceExtent(200, 200.f, 100.f)
	, StepAnimPlayRate(1)
	, StepCheckDistance(150)
	, CheckSwingDistance(300.f)
	, SwingTraceVerticalOffset(-80.f)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UInteractiveMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	Owner = GetOwner();
	
	for (auto Component : Owner->GetComponents())
	{
		if (Component->IsA(UCapsuleComponent::StaticClass()))
		{
			Capsule = Cast<UCapsuleComponent>(Component);
		}
		else if (Component->IsA(USkeletalMeshComponent::StaticClass()))
		{
			SkeletalMesh = Cast<USkeletalMeshComponent>(Component);
		}
		else if (Component->IsA(UCharacterMovementComponent::StaticClass()))
		{
			CharacterMovement = Cast<UCharacterMovementComponent>(Component);
		}
		else if (Component->IsA(UMotionWarpingComponent::StaticClass()))
		{
			MotionWarping = Cast<UMotionWarpingComponent>(Component);
		}
	}

	AnimInstance = SkeletalMesh->GetAnimInstance();
}


// Called every frame
void UInteractiveMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


void UInteractiveMovementComponent::CheckMantle(TArray<AActor*> IgnoredActors)
{
	if (!Capsule || !SkeletalMesh || !CharacterMovement)
	{
		return;
	}
	
	FVector SphereTraceStart;
	FVector SphereTraceEnd;
	//float SphereTraceRadius = 10;
	FHitResult OutHitSphere;

	SphereTraceStart = Owner->GetActorLocation() + Owner->GetActorUpVector() * MantleSphereTraceOffset;
	SphereTraceEnd = SphereTraceStart + Owner->GetActorForwardVector() * StickToWallDistance;
	bool bSphereTrace = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), SphereTraceStart, SphereTraceEnd, SphereTraceRadius, ETraceTypeQuery::TraceTypeQuery1, false, IgnoredActors, EDrawDebugTrace::None, OutHitSphere, true);

	FVector LineTraceStart;
	FVector LineTraceEnd;
	FHitResult OutHit;
	FLatentActionInfo LatentInfo;
	LatentInfo.CallbackTarget = this;

	for (int i = 0; i < StickToWallDistance; i += 10)
	{
		float LinetraceLength = 30;
		LineTraceStart = Owner->GetActorForwardVector() * (i + MantleDistance) + OutHitSphere.TraceStart;
		LineTraceEnd = LineTraceStart - FVector(0, 0, LinetraceLength);
		if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), LineTraceStart, LineTraceEnd, ETraceTypeQuery::TraceTypeQuery1, false, IgnoredActors, EDrawDebugTrace::None, OutHit, true, FColor::Yellow))
		{
			AActor* ManagingActor = OutHit.HitObjectHandle.GetManagingActor();
			if (!ManagingActor)
			{
				return;
			}
			if (OutHit.bStartPenetrating || ManagingActor->Implements<UInteractiveMovement>())
			{
				//FLogger::DebugLog("Penetra");
				return;
			}

			if (bSphereTrace && OutHitSphere.Distance < (i + MantleDistance))
			{
				//FLogger::DebugLog("Collision");
				return;
			}

			FVector EndPosition = FVector((OutHit.Location - Owner->GetActorForwardVector() * Capsule->GetScaledCapsuleRadius()).X, (OutHit.Location - Owner->GetActorForwardVector() * Capsule->GetScaledCapsuleRadius()).Y, OutHit.Location.Z - Capsule->GetScaledCapsuleHalfHeight() + 20);

			// Check that the player has enough space at the beginning of the animation
			float RadiusOffset = 15;
			FHitResult OutHitSphereLandPosition;
			SphereTraceStart = OutHit.Location - Owner->GetActorForwardVector() * (Capsule->GetScaledCapsuleRadius() + RadiusOffset);
			SphereTraceEnd = SphereTraceStart - FVector(0, 0, Capsule->GetScaledCapsuleHalfHeight());

			if (UKismetSystemLibrary::SphereTraceSingle(GetWorld(), SphereTraceStart, SphereTraceEnd, Capsule->GetScaledCapsuleRadius(), ETraceTypeQuery::TraceTypeQuery1, false, IgnoredActors, EDrawDebugTrace::None, OutHitSphereLandPosition, true))
			{
				//FLogger::DebugLog("Not enough room for the climb ledge start");
				return;
			}

			// Check that the player has enough space at the destination location
			SphereTraceStart = OutHit.Location + Owner->GetActorUpVector() * (Capsule->GetScaledCapsuleRadius() + RadiusOffset);
			SphereTraceEnd = LineTraceStart + FVector(0, 0, Capsule->GetScaledCapsuleHalfHeight() * 2);

			if (UKismetSystemLibrary::SphereTraceSingle(GetWorld(), SphereTraceStart, SphereTraceEnd, Capsule->GetScaledCapsuleRadius(), ETraceTypeQuery::TraceTypeQuery1, false, IgnoredActors, EDrawDebugTrace::None, OutHitSphereLandPosition, true))
			{
				//FLogger::DebugLog("Not enough room for the climb ledge end");
				return;
			}

			LinetraceLength = 50;
			LineTraceStart = OutHit.Location - Owner->GetActorForwardVector() * RadiusOffset;
			LineTraceEnd = LineTraceStart - FVector(0, 0, LinetraceLength);

			if (!(UKismetSystemLibrary::LineTraceSingle(GetWorld(), LineTraceStart, LineTraceEnd, ETraceTypeQuery::TraceTypeQuery2, false, IgnoredActors, EDrawDebugTrace::None, OutHit, true, FColor::Purple)))
			{
				UKismetSystemLibrary::MoveComponentTo(Capsule, EndPosition, Owner->GetActorRotation(), true, true, .1, true, EMoveComponentAction::Type::Move, LatentInfo);
				Mantle();
			}

			return;
		}
	}
}

void UInteractiveMovementComponent::Mantle()
{
	CharacterMovement->SetMovementMode(MOVE_Flying);

	if (IsValid(AnimInstance) && IsValid(MantleMontage))
	{
		AnimInstance->OnMontageEnded.AddDynamic(this, &UInteractiveMovementComponent::OnMantleCompleted);
		AnimInstance->Montage_Play(MantleMontage);
	}
}

bool UInteractiveMovementComponent::CheckSwing(TArray<AActor*> IgnoredActors) const
{
	if (!Capsule || !SkeletalMesh || !CharacterMovement)
	{
		return false;
	}
	
	//constexpr float SphereTraceRadius = 10;
	FHitResult OutHit;

	FVector SphereTraceStart = Owner->GetActorLocation() + Owner->GetActorUpVector() * SwingTraceVerticalOffset;
	FVector SphereTraceEnd = SphereTraceStart + Owner->GetActorForwardVector() * CheckSwingDistance;
	bool bSphereTrace = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), SphereTraceStart, SphereTraceEnd, SphereTraceRadius, UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel2), false, IgnoredActors, EDrawDebugTrace::None, OutHit, true);

	
	if (!bSphereTrace)
	{
		return false;
	}
	
	AActor* HitActor = OutHit.HitObjectHandle.GetManagingActor();
	if (!IsValid(HitActor))
	{
		return false;
	}
	if (HitActor->Implements<UInteractiveMovement>())
	{
		if (IInteractiveMovement* InterfacePtr = Cast<IInteractiveMovement>(HitActor))
		{
			return InterfacePtr->Interact(Owner);
		}
		return false;
	}

	return false;
}


bool UInteractiveMovementComponent::CheckVault(TArray<AActor*> IgnoredActors)
{
	if (!Capsule || !SkeletalMesh || !CharacterMovement || !MotionWarping)
	{
		return false;
	}
	
	//constexpr float SphereTraceRadius = 10;
	FHitResult OutHit;

	FVector SphereTraceStart = Owner->GetActorLocation();
	FVector SphereTraceEnd = SphereTraceStart + Owner->GetActorForwardVector() * CheckVaultDistance;
	bool bSphereTrace = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), SphereTraceStart, SphereTraceEnd, SphereTraceRadius, ETraceTypeQuery::TraceTypeQuery1, false, IgnoredActors, EDrawDebugTrace::None, OutHit, true);

	if (OutHit.HasValidHitObjectHandle())
	{
		if (!(OutHit.bStartPenetrating) && bSphereTrace)
		{
			return SetUpVault(OutHit.ImpactPoint, IgnoredActors);
		}
		return false;
	}
	return false;
}

bool UInteractiveMovementComponent::SetUpVault(const FVector& StartVaultLocation, TArray<AActor*> IgnoredActors)
{
	PrepareIgnoredActors(IgnoredActors);

	int LastIndex = 0;
	bool bCanLand = false;

	for (int iIndex = 0; iIndex <= (MaxVaultDistance + SphereTraceStep); iIndex += SphereTraceStep)
	{
		if (HandleVaultTrace(StartVaultLocation, IgnoredActors, iIndex, LastIndex, bCanLand))
		{
			break;
		}
	}

	return FinalizeVault(LastIndex, StartVaultLocation, bCanLand);
}

void UInteractiveMovementComponent::PrepareIgnoredActors(TArray<AActor*>& IgnoredActors) const
{
	for (TActorIterator<AGenericCharacter> It(GetWorld()); It; ++It)
	{
		IgnoredActors.Add(*It);
	}
}

bool UInteractiveMovementComponent::HandleVaultTrace(const FVector& StartVaultLocation, const TArray<AActor*>& IgnoredActors, int iIndex, int& LastIndex, bool& bCanLand)
{
	FVector SphereTraceStart = StartVaultLocation + Owner->GetActorForwardVector() * iIndex + SphereTraceVerticalOffset;
	FVector SphereTraceEnd = SphereTraceStart - (Owner->GetActorUpVector() * SphereTraceLength);
	FHitResult OutHit;

	if (UKismetSystemLibrary::SphereTraceSingle(GetWorld(), SphereTraceStart, SphereTraceEnd, SphereTraceRadius, ETraceTypeQuery::TraceTypeQuery1, false, IgnoredActors, EDrawDebugTrace::None, OutHit, true))
	{
		return ProcessSphereTraceHit(StartVaultLocation, OutHit, iIndex, LastIndex, bCanLand);
	}

	return ProcessVaultLanding(SphereTraceStart, OutHit, IgnoredActors, bCanLand);
}

bool UInteractiveMovementComponent::ProcessSphereTraceHit(const FVector& StartVaultLocation, const FHitResult& OutHit, int iIndex, int& LastIndex, bool& bCanLand)
{
	LastIndex = iIndex;

	if (OutHit.bStartPenetrating)
	{
		bCanWarp = false;
		return true;
	}

	if (iIndex == 0)
	{
		if (OutHit.ImpactNormal.Z < SurfaceNormalZValue)
		{
			bCanLand = false;
			return true;
		}

		VaultStartPos = OutHit.Location;
		VaultMiddlePos = VaultStartPos;
		bCanWarp = true;
	}
	else
	{
		VaultMiddlePos = OutHit.ImpactPoint;
		bCanWarp = true;
	}

	return false;
}

bool UInteractiveMovementComponent::ProcessVaultLanding(const FVector& SphereTraceStart, const FHitResult& OutHit, const TArray<AActor*>& IgnoredActors, bool& bCanLand)
{
	DetectKickableActor(OutHit, SphereTraceStart, IgnoredActors);
	return CheckLandingLocation(SphereTraceStart, IgnoredActors, bCanLand);
}

void UInteractiveMovementComponent::DetectKickableActor(const FHitResult& OutHit, const FVector& SphereTraceStart, const TArray<AActor*>& IgnoredActors)
{
	FVector BoxTraceStart = OutHit.TraceStart + (Owner->GetActorForwardVector() * MinimumLandSpace);
	FVector BoxTraceEnd = SphereTraceStart + Owner->GetActorForwardVector() * 100;
	TArray<FHitResult> OutHits;

	if (UKismetSystemLibrary::BoxTraceMulti(GetWorld(), BoxTraceStart, BoxTraceEnd, KickedBoxTraceExtent, Owner->GetActorRotation(), ETraceTypeQuery::TraceTypeQuery1, false, {}, EDrawDebugTrace::None, OutHits, true))
	{
		for (const FHitResult& Hit : OutHits)
		{
			AActor* HitActor = Hit.GetActor();
			if (HitActor && HitActor->Implements<UKickInterface>())
			{
				if (IKickInterface* KickInterfacePtr = Cast<IKickInterface>(HitActor))
				{
					KickInterfacePtr->KnockBackActor();
				}
				KickedActor = HitActor;
				KickHit = Hit;
				VaultMiddlePos = KickedActor->GetActorLocation();
				break;
			}
		}
	}
}

FVector UInteractiveMovementComponent::GetLineTraceStart() const
{
	if (KickedActor)
	{
		return KickedActor->GetActorLocation();
	}

	return VaultMiddlePos + Owner->GetActorForwardVector() * MinimumLandSpace;
}

bool UInteractiveMovementComponent::CheckLandingLocation(const FVector& SphereTraceStart, const TArray<AActor*>& IgnoredActors, bool& bCanLand)
{
	const FVector LineTraceStart = GetLineTraceStart();
	const FVector LineTraceEnd = LineTraceStart - Owner->GetActorUpVector() * VaultLandTraceLenght;
	FHitResult LandHit;

	if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), LineTraceStart, LineTraceEnd, ETraceTypeQuery::TraceTypeQuery1, false, IgnoredActors, EDrawDebugTrace::None, LandHit, true))
	{
		VaultLandPos = LandHit.ImpactPoint;
		bIsVoidVault = FMath::Abs(LandHit.Location.Z - VaultMiddlePos.Z) > VaultOverHeight;

		if (!bIsVoidVault && FMath::IsNearlyEqual(VaultLandPos.Z, VaultStartPos.Z, 100))
		{
			bCanLand = false;
			return true;
		}

		bCanLand = true;
		return true;
	}

	bCanWarp = false;
	return true;
}


bool UInteractiveMovementComponent::FinalizeVault(int LastIndex, const FVector& StartVaultLocation, bool bCanLand)
{
	if (LastIndex < MinVaultableLength)
	{
		return false;
	}

	bShortVault = LastIndex <= ShortVaultDistance;
	bLongVault = LastIndex >= LongVaultDistance;

	if (!bCanLand)
	{
		if ((LastIndex >= MinimumLandSpace) && FVector::Distance(StartVaultLocation, Owner->GetActorLocation()) < StepCheckDistance)
		{
			bCanWarp = true;
			StepStartPos = StartVaultLocation;
			StepLandPos = StepStartPos + Owner->GetActorForwardVector() * SphereTraceStep;
			return StepMotionWarp();
		}
		return false;
	}

	return VaultMotionWarp();
}


bool UInteractiveMovementComponent::VaultMotionWarp()
{
	if (!bCanWarp)
	{
		VaultLandPos = FVector(0, 0, 200000);
		return false;
	}

	CharacterMovement->SetMovementMode(EMovementMode::MOVE_Flying, 0);

	FMotionWarpingTarget WarpingTarget;
	WarpingTarget.Location = VaultStartPos;
	WarpingTarget.Rotation = Owner->GetActorRotation();
	WarpingTarget.Name = "VaultStart";
	MotionWarping->AddOrUpdateWarpTarget(WarpingTarget);

	WarpingTarget.Location = VaultMiddlePos;
	WarpingTarget.Rotation = Owner->GetActorRotation();
	WarpingTarget.Name = "VaultMiddle";
	MotionWarping->AddOrUpdateWarpTarget(WarpingTarget);

	WarpingTarget.Location = VaultLandPos;
	WarpingTarget.Rotation = Owner->GetActorRotation();
	WarpingTarget.Name = "VaultLand";
	MotionWarping->AddOrUpdateWarpTarget(WarpingTarget);
	
	if (bShortVault)
	{
		if (IsValid(AnimInstance) && IsValid(ShortVaultMontage))
		{
			AnimInstance->OnMontageEnded.AddDynamic(this, &UInteractiveMovementComponent::OnShortVaultCompleted);
			AnimInstance->Montage_Play(ShortVaultMontage, ShortVaultAnimPlayRate);
		}
	}
	else
	{
		if (IsValid(AnimInstance) && IsValid(VaultMontage))
		{
			AnimInstance->OnMontageEnded.AddDynamic(this, &UInteractiveMovementComponent::OnVaultCompleted);
			AnimInstance->Montage_Play(VaultMontage, VaultAnimPlayRate);
		}
	}
	return true;
}

bool UInteractiveMovementComponent::StepMotionWarp()
{
	if (!bCanWarp /*|| !(UKismetMathLibrary::InRange_FloatFloat(VaultLandPos.Z, SkeletalMesh->GetComponentLocation().Z - 50, SkeletalMesh->GetComponentLocation().Z + 50))*/)
	{
		//VaultLandPos = FVector(0, 0, 200000);
		//ACharacter::Jump();
		return false;
	}

	CharacterMovement->SetMovementMode(EMovementMode::MOVE_Flying, 0);

	FMotionWarpingTarget WarpingTarget;
	WarpingTarget.Location = StepStartPos;
	WarpingTarget.Rotation = Owner->GetActorRotation();
	WarpingTarget.Name = "StepStart";
	MotionWarping->AddOrUpdateWarpTarget(WarpingTarget);

	WarpingTarget.Location = StepLandPos;
	WarpingTarget.Rotation = Owner->GetActorRotation();
	WarpingTarget.Name = "StepLand";
	MotionWarping->AddOrUpdateWarpTarget(WarpingTarget);

	if (IsValid(AnimInstance) && IsValid(StepMontage))
	{
		AnimInstance->OnMontageEnded.AddDynamic(this, &UInteractiveMovementComponent::OnStepCompleted);
		AnimInstance->Montage_Play(StepMontage, StepAnimPlayRate);
	}
	return true;
}

void UInteractiveMovementComponent::OnMotionWarpStarted(FName NotifyName)
{
	if (NotifyName == "VaultMiddle")
	{
		//FLogger::DebugLog("EMPIEZA MOTION WARP");
		if (bLongVault)
		{
			if (IsValid(AnimInstance) && IsValid(MantleMontage))
			{
				AnimInstance->Montage_SetPlayRate(VaultMontage, LongVaultAnimPlayRate);
			}
		}
	}
	
	if (NotifyName == "VaultLand")
    {
    	if (bLongVault)
    	{
    		if (IsValid(AnimInstance) && IsValid(MantleMontage))
    		{
    			AnimInstance->Montage_SetPlayRate(VaultMontage, VaultAnimPlayRate);
    		}
    	}
    }
}

void UInteractiveMovementComponent::OnMotionWarpFinished(FName NotifyName)
{
	if (NotifyName == "VaultMiddle")
	{
		if (IsValid(KickedActor))
		{
			IKickInterface* KickInterfacePtr = Cast<IKickInterface>(KickedActor);

			//DrawDebugSphere(GetWorld(), VaultMiddlePos, 10, 12, FColor::Orange, false, 2, 0U, 2);

			FVector OwnerLocation = Owner->GetActorLocation();
			if (KickInterfacePtr)
			{
				KickInterfacePtr->LaunchActor(((OwnerLocation + Owner->GetActorForwardVector() * 100) - OwnerLocation).GetSafeNormal());//KickedActor->GetActorForwardVector() * -1);
				//DrawDebugLine(GetWorld(), Owner->GetActorLocation() /*- FVector(0,0, 80)*/, OwnerLocation + Owner->GetActorForwardVector() * 100, FColor::Green, false, 2.0f, 0, 2.0f);
			}

			KickedActor = nullptr;
		}
		else
		{
			if (bIsVoidVault)
			{
				CharacterMovement->SetMovementMode(MOVE_Falling);
			}
		}
	}
}

void UInteractiveMovementComponent::OnVaultCompleted(UAnimMontage* Montage, bool bInterrupted)
{
	if (IsValid(Montage) && Montage == VaultMontage)
	{
		SkeletalMesh->GetAnimInstance()->OnMontageEnded.RemoveDynamic(this, &UInteractiveMovementComponent::OnVaultCompleted);
		
		if (!bIsVoidVault)
		{
			CharacterMovement->SetMovementMode(MOVE_Walking);
		}
		bCanWarp = false;
		VaultLandPos = FVector(0, 0, 200000);
	}
}

void UInteractiveMovementComponent::OnShortVaultCompleted(UAnimMontage* Montage, bool bInterrupted)
{
	if (IsValid(Montage) && Montage == ShortVaultMontage)
	{
		SkeletalMesh->GetAnimInstance()->OnMontageEnded.RemoveDynamic(this, &UInteractiveMovementComponent::OnShortVaultCompleted);
		if (!bIsVoidVault)
		{
			CharacterMovement->SetMovementMode(MOVE_Walking);
		}
		bCanWarp = false;
		VaultLandPos = FVector(0, 0, 200000);
	}
}


void UInteractiveMovementComponent::OnMantleCompleted(UAnimMontage* Montage, bool bInterrupted)
{
	if (IsValid(Montage) && Montage == MantleMontage)
	{
		SkeletalMesh->GetAnimInstance()->OnMontageEnded.RemoveDynamic(this, &UInteractiveMovementComponent::OnMantleCompleted);
		CharacterMovement->SetMovementMode(MOVE_Walking);
	}
}

void UInteractiveMovementComponent::OnStepCompleted(UAnimMontage* Montage, bool bInterrupted)
{
	if (IsValid(Montage) && Montage == StepMontage)
	{
		SkeletalMesh->GetAnimInstance()->OnMontageEnded.RemoveDynamic(this, &UInteractiveMovementComponent::OnStepCompleted);
		CharacterMovement->SetMovementMode(MOVE_Walking);
		bCanWarp = false;
		//VaultLandPos = FVector(0, 0, 200000);
	}
}

