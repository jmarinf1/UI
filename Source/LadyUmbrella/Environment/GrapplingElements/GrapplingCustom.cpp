// Fill out your copyright notice in the Description page of Project Settings.


#include "GrapplingCustom.h"
#include "LadyUmbrella/Util/Logger.h"


AGrapplingCustom::AGrapplingCustom()
{

	SpeedOfRotationPitch = 0.f;
	SpeedOfRotationYaw	 = 0.f;
	SpeedOfRotationRoll	 = 0.f;
	DegreesOfRotation	 = 0.f;
	RotationAxis		 = FRotator(0, 0, 0);
	InterpolateRotation  = true;
}

void AGrapplingCustom::BeginPlay()
{
	Super::BeginPlay();
}

void AGrapplingCustom::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AGrapplingCustom::ContactWithCable(FVector& Direction, UPrimitiveComponent* ComponentHit)
{
	GetWorld()->GetTimerManager().ClearTimer(RotateTimer);

	if (IsValid(ComponentHit))
	{
		GetWorldTimerManager().SetTimer(RotateTimer, this, &AGrapplingCustom::RotateElement, 0.01, true);
	}
}

IHookInterface::GrabType AGrapplingCustom::GetGrabType()
{
	return PULLWITHCOMPONENT;
}

void AGrapplingCustom::RotateElement()
{
	auto* Root = GetRootComponent();
	if (!IsValid(Root))
	{
		GetWorld()->GetTimerManager().ClearTimer(RotateTimer);
		return;
	}
	
	const FRotator CurrentRotation	  = Root->GetComponentRotation();
	const FRotator FinalRotation	  =  DegreesOfRotation * RotationAxis;

	const float MaxDeltaPitch	= SpeedOfRotationPitch	* GetWorld()->GetDeltaSeconds();
	const float MaxDeltaYaw		= SpeedOfRotationYaw	* GetWorld()->GetDeltaSeconds();
	const float MaxDeltaZ		= SpeedOfRotationRoll	* GetWorld()->GetDeltaSeconds();

	const float TargetPitch = RotationAxis.Pitch	!= 0.f ? FinalRotation.Pitch	: CurrentRotation.Pitch;
	const float TargetYaw	= RotationAxis.Yaw		!= 0.f ? FinalRotation.Yaw		: CurrentRotation.Yaw;
	const float TargetRoll	= RotationAxis.Roll		!= 0.f ? FinalRotation.Roll		: CurrentRotation.Roll;

	const float NewPitch	= FMath::FixedTurn(CurrentRotation.Pitch, TargetPitch	, MaxDeltaPitch);
	const float NewYaw		= FMath::FixedTurn(CurrentRotation.Yaw  , TargetYaw		, MaxDeltaYaw);
	const float NewRoll 	= FMath::FixedTurn(CurrentRotation.Roll , TargetRoll	, MaxDeltaZ);
	
	const FRotator NewRotation = FRotator(NewPitch, NewYaw, NewRoll);

	Root->SetRelativeRotation(NewRotation);
	
	const bool bDone =
			FMath::IsNearlyEqual(NewPitch, FinalRotation.Pitch, 1.0f) &&
			FMath::IsNearlyEqual(NewYaw,   FinalRotation.Yaw,   1.0f) &&
			FMath::IsNearlyEqual(NewRoll,  FinalRotation.Roll,  1.0f);
	
	if (bDone)
	{
		GetWorld()->GetTimerManager().ClearTimer(RotateTimer);
		Root->SetRelativeRotation(FinalRotation);
	}
}

