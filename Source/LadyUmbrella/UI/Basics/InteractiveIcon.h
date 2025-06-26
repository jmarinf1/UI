// 
// InteractiveIcon.h
// 
// Interactive icons for player interactions. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractiveIcon.generated.h"

class APlayerCharacter;
class UImage;

UCLASS()
class LADYUMBRELLA_API UInteractiveIcon : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Image", meta = (BindWidget, AllowPrivateAccess = "true"))
	UImage* CircleImage;

	UPROPERTY(BlueprintReadWrite, Category = "Image", meta = (BindWidget, AllowPrivateAccess = "true"))
	UImage* ProhibitedImage;
	
	UPROPERTY(BlueprintReadWrite, Category = "Image", meta = (BindWidget, AllowPrivateAccess = "true"))
	UImage* PCImage;

	UPROPERTY(BlueprintReadWrite, Category = "Image", meta = (BindWidget, AllowPrivateAccess = "true"))
	UImage* XboxImage;
	
	UPROPERTY(BlueprintReadWrite, Category = "Image", meta = (BindWidget, AllowPrivateAccess = "true"))
	UImage* PlayStationImage;

	// NOTE: The default values cannot be inlined in the constructor as UUserWidget demands control of the constructor.
	bool bShouldShow      = true;
	float InterpolateSize = 1.0f;
	FVector TraceOffset   = FVector(0.0f, 0.0f, 10.0f); 
	
public:

	bool Update(const AActor* Owner, const float OuterSphereSize, const float OuterSphereTriggerRadius,
		const float InnerSphereSize, const float InnerSphereTriggerRadius, const float AnimationSpeed,
		const bool bCanInteract, const FString& Device);

	UImage* GetCircleImage() const;
	UImage* GetProhibitedImage() const;
	UImage* GetPCImage() const;
	UImage* GetXboxImage() const;
	UImage* GetPlayStationImage() const;

	FORCEINLINE bool ShouldShow() const { return bShouldShow; }
	FORCEINLINE void SetShouldShow(const bool Value) { bShouldShow = Value; }

private:

	bool ShowInnerIcon(const AActor* Owner, const APlayerCharacter* PlayerCharacter, const float InnerSphereSize,
		const float AnimationSpeed, const bool bCanInteract, const FString& Device);
	bool ShowOuterIcon(const AActor* Owner, const APlayerCharacter* PlayerCharacter, const float OuterSphereSize,
		const float AnimationSpeed);

	bool AnimateIcon(const AActor* Owner, const APlayerCharacter* PlayerCharacter, const float DesiredSize,
		const float AnimationSpeed);
	void CheckPlatform(const FString& Device) const;

	bool CheckCollisionBeforeCamera(const AActor* Owner, const APlayerCharacter* PlayerCharacter) const;
	bool IsOutsideFrustum(const AActor* Owner, const float IconFrustumMargin);
	
};
