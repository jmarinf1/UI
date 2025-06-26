#pragma once
 
#include "CoreMinimal.h"
#include "CameraEnums.generated.h"

UENUM(BlueprintType, meta = (DisplayName = "FShakeSource"))
enum class FShakeSource : uint8
{
	PLAYER			UMETA(DisplayName = "Player"),
	ENVIRONMENT		UMETA(DisplayName = "Environment"),
	UNKNOWN			UMETA(DisplayName = "Unknown")
};

UENUM(BlueprintType, meta = (DisplayName = "FCurrentHitEnvironment"))
enum class FCurrentHitEnvironment : uint8
{
	WALL			UMETA(DisplayName = "Wall"),
	CORNER			UMETA(DisplayName = "Corner"),
	HALLWAY			UMETA(DisplayName = "Hallway"),
	HWY_CORNER		UMETA(DisplayName = "Hallway Corner"),
	NONE			UMETA(DisplayName = "No Hit")
};

UENUM(BlueprintType, meta = (DisplayName = "FCameraStateKey"))
enum class FCameraStateKey : uint8
{
	DEFAULT		UMETA(DisplayName = "Default"),			// Basic over-the-shoulder camera (Rule of Thirds alignment)
	CINEMATIC	UMETA(DisplayName = "Cinematic Acion"),	// Scripted camera movement for events (Uses cubic splines)
	LOOKAT		UMETA(DisplayName = "LookAt"),			// Contextual focus on objects/areas (Temporarily overrides player input)
	COVER		UMETA(DisplayName = "Cover"),			// Zooms slightly, keeps reticule consistent (Lateral offset preserved)
	AIM			UMETA(DisplayName = "Aim"),				// Snaps to nearest valid shoulder (Adapts to in cover movement)
};


UENUM(BlueprintType, meta = (DisplayName = "EModifierType"))
enum class EModifierType : uint8
{
	DEFAULT			UMETA(DisplayName = "Default Sensibility"), // Multiplier applied to Master Sensibility
	AIM				UMETA(DisplayName = "Aim Sensibility"),		// Multiplier applied to Master Sensibility
	YAW				UMETA(DisplayName = "Horizontal Inversion"),
	PITCH			UMETA(DisplayName = "Vertical Inversion"),
};

UENUM(BlueprintType, meta = (DisplayName = "ECameraInterpolation"))
enum class ECameraInterpolation : uint8
{
	CUSTOM		= 0		UMETA(DisplayName = "Custom"),		
	EASE		= 1		UMETA(DisplayName = "Basic Ease"),
	EASE_I		= 2		UMETA(DisplayName = "Ease-In"),
	EASE_O		= 3		UMETA(DisplayName = "Ease-Out"),
	EASE_IO		= 4		UMETA(DisplayName = "Ease-In-Out"),
	SPRING_I	= 5		UMETA(DisplayName = "Elastic Ease-In"),
	SPRING_O	= 6		UMETA(DisplayName = "Elastic Ease-Out"),
	SPRING_IO	= 7		UMETA(DisplayName = "Elastic Ease-In-Out"),
	LINEAR		= 8		UMETA(DisplayName = "Linear")
};

