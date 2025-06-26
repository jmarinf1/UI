#pragma once
 
#include "CoreMinimal.h"
#include "FCameraMultipliers.generated.h"

USTRUCT(BlueprintType, meta = (DisplayName = "FCameraMultipliers"))
struct FCameraMultipliers
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera Sensibility", meta = (AllowPrivateAccess = "true", NoResetToDefault, Units = "Times"))
	FVector2D SensibilityRange = FVector2D(0.f,2.f);
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(Units = "Times"))
	float Sensibility = 1.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bSensibilityFitsCurve = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera Sensibility", meta = (AllowPrivateAccess = "true", NoResetToDefault, Units = "Times"))
	FVector2D MultiplierRange = FVector2D(0.f,2.f);
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(Units = "Times"))
	float AimMultiplier = 1.f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(Units = "Times"))
	float DefaultMultiplier = 1.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool InvertYaw = false;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool InvertPitch = false;
};
