// 
// InteractiveIcon.cpp
// 
// Implementation of the InteractiveIcon class.
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#include "LadyUmbrella/UI/Basics/InteractiveIcon.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Util/Logger.h"
#include "LadyUmbrella/Util/Controllers/MainController.h"
#include "LadyUmbrella/Util/SaveSystem/LU_GameInstance.h"

bool UInteractiveIcon::Update(const AActor* Owner, const float OuterSphereSize, const float OuterSphereTriggerRadius,
	const float InnerSphereSize, const float InnerSphereTriggerRadius, const float AnimationSpeed,
	const bool bCanInteract, const FString& Device)
{
	const APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	if (!IsValid(PlayerCharacter))
	{
		FLogger::ErrorLog("Invalid Player pointer in InteractiveIcon.Update()");
	}

	GetCircleImage()->SetVisibility(ESlateVisibility::Hidden);
	GetProhibitedImage()->SetVisibility(ESlateVisibility::Hidden);
	GetPCImage()->SetVisibility(ESlateVisibility::Hidden);
	GetXboxImage()->SetVisibility(ESlateVisibility::Hidden);
	GetPlayStationImage()->SetVisibility(ESlateVisibility::Hidden);

	if (!bShouldShow)
	{
		return false;
	}
	
	const float DistanceToActor = FVector::Dist(PlayerCharacter->GetActorLocation(), Owner->GetActorLocation());
		
	if (DistanceToActor <= InnerSphereTriggerRadius)
	{
		return ShowInnerIcon(Owner, PlayerCharacter, InnerSphereSize, AnimationSpeed, bCanInteract, Device);
	}

	if (DistanceToActor <= OuterSphereTriggerRadius)
	{
		return ShowOuterIcon(Owner, PlayerCharacter, OuterSphereSize, AnimationSpeed);
	}
	
	return false;
}

bool UInteractiveIcon::ShowInnerIcon(const AActor* Owner, const APlayerCharacter* PlayerCharacter,
	const float InnerSphereSize, const float AnimationSpeed, const bool bCanInteract, const FString& Device)
{
	if (CheckCollisionBeforeCamera(Owner, PlayerCharacter))
	{
		return false;
	}	
	
	CheckPlatform(Device);
	
	if (!bCanInteract)
	{
		GetProhibitedImage()->SetVisibility(ESlateVisibility::Visible);
	}

	return AnimateIcon(Owner, PlayerCharacter, InnerSphereSize, AnimationSpeed);
}

bool UInteractiveIcon::ShowOuterIcon(const AActor* Owner, const APlayerCharacter* PlayerCharacter,
	const float OuterSphereSize, const float AnimationSpeed)
{
	if (CheckCollisionBeforeCamera(Owner, PlayerCharacter))
	{
		return false;
	}

	return AnimateIcon(Owner, PlayerCharacter, OuterSphereSize, AnimationSpeed);
}

bool UInteractiveIcon::AnimateIcon(const AActor* Owner, const APlayerCharacter* PlayerCharacter,
	const float DesiredSize, const float AnimationSpeed)
{
	GetCircleImage()->SetVisibility(ESlateVisibility::Visible);
	
	InterpolateSize = FMath::Lerp(InterpolateSize, DesiredSize, AnimationSpeed);
	GetCircleImage()->SetRenderScale(FVector2D(InterpolateSize));
	GetProhibitedImage()->SetRenderScale(FVector2D(InterpolateSize));
	GetPCImage()->SetRenderScale(FVector2D(InterpolateSize));
	GetXboxImage()->SetRenderScale(FVector2D(InterpolateSize));
	GetPlayStationImage()->SetRenderScale(FVector2D(InterpolateSize));

	return true;
}

void UInteractiveIcon::CheckPlatform(const FString& Device) const
{
	const ULU_GameInstance* GameInstance = Cast<ULU_GameInstance>(GetGameInstance());

	if (Device.Equals("KBM"))
	{
		return GetPCImage()->SetVisibility(ESlateVisibility::Visible);
	}
	
	if (!IsValid(GameInstance))
	{
		return;
	}
	
	if (GameInstance->GetPlatformName().Equals("XBOX"))
	{
		return GetXboxImage()->SetVisibility(ESlateVisibility::Visible);
	}
	
	if (GameInstance->GetPlatformName().Equals("PLAYSTATION"))
	{
		return GetPlayStationImage()->SetVisibility(ESlateVisibility::Visible);
	}
}

bool UInteractiveIcon::IsOutsideFrustum(const AActor* Owner, const float IconFrustumMargin)
{
	const AMainController* MainController = Cast<AMainController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	FVector2D ScreenLocation;
	const FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());

	MainController->ProjectWorldLocationToScreen(Owner->GetActorLocation(), ScreenLocation);
	
	if ((ScreenLocation.X > IconFrustumMargin) &&
		(ScreenLocation.Y > IconFrustumMargin) &&
		(ScreenLocation.X < ViewportSize.X - IconFrustumMargin) &&
		(ScreenLocation.Y < ViewportSize.Y - IconFrustumMargin))
	{
		return false;
	}

	return true;
}

bool UInteractiveIcon::CheckCollisionBeforeCamera(const AActor* Owner, const APlayerCharacter* PlayerCharacter) const
{
	if (!IsValid(Owner) || !IsValid(PlayerCharacter))
	{
		FLogger::ErrorLog("Invalid pointers in UInteractiveIcon.CheckCollisionBeforeCamera()");
		return true;
	}
	
	TArray<FHitResult> Obstacles;
	FCollisionQueryParams CollisionQueryParams;

	CollisionQueryParams.AddIgnoredActor(Owner);
	CollisionQueryParams.AddIgnoredActors(Owner->Children);
	CollisionQueryParams.AddIgnoredActor(PlayerCharacter);
	
	bool Hit = GetWorld()->LineTraceMultiByChannel(
		Obstacles,
		Owner->GetActorLocation() + TraceOffset,
		PlayerCharacter->GetCameraSystem()->GetFollowCamera()->GetComponentLocation(),
		ECC_Camera,
		CollisionQueryParams
	);

	for (FHitResult ObstacleEntry : Obstacles)
	{
		if (!IsValid(ObstacleEntry.GetActor()))
		{
			continue;
		}

		if (ObstacleEntry.GetActor()->IsA(AGenericCharacter::StaticClass()))
		{
			return true;
		}
	}

	return Hit;
}

UImage* UInteractiveIcon::GetCircleImage() const
{
	return CircleImage;
}

UImage* UInteractiveIcon::GetProhibitedImage() const
{
	return ProhibitedImage;
}

UImage* UInteractiveIcon::GetPCImage() const
{
	return PCImage;
}

UImage* UInteractiveIcon::GetXboxImage() const 
{
	return XboxImage;
}

UImage* UInteractiveIcon::GetPlayStationImage() const
{
	return PlayStationImage;
}


