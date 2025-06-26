#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Blueprint/UserWidget.h"
#include "HitReactionComponent.generated.h"

class UCameraComponent;
class UHealthComponent;
class UCurveFloat;
class UImage;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LADYUMBRELLA_API UHitReactionComponent : public UActorComponent
{
	GENERATED_BODY()

	const FVector2D WidgetCanvasSize = FVector2D(1920.f, 1080.f); //Max Values of the translation sliders of the canvasLayer in the HitMarker Widget
	const FVector2D ScreenCenter = FVector2D::ZeroVector; // Yes
public:
	UHitReactionComponent();

	// Call this to spawn and place a hit marker
	UFUNCTION(BlueprintCallable, Category = "Hit Reaction")
	void ShowHitMarker(FVector Origin);

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY()
	UHealthComponent* HealthComponent;
	UPROPERTY()
	UCameraComponent* CachedCameraComponent;
	
	FTimerHandle SaturationUpdateTimer;

	void UpdateCameraSaturation();
	
	FVector2D CachedCameraForward2D;
	FVector2D CachedCameraRight2D;
	FRotator LastCameraRotation;
    int32 CachedHealth;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> HitMarkerWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	FName HitMarkerWidgetSlotName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit Marker")
	float RotationOffsetDegrees;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visuals")
	float BorderFadeRate;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visuals")
	UCurveFloat* SaturationCurve;

	UPROPERTY(EditAnywhere, Category = "Visuals", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MinSaturation;

	UPROPERTY(EditAnywhere, Category = "Visuals", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MaxSaturation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> HitBorderWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit Border")
	FName HitBorderImageName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit Border", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MaxBorderOpacity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit Border", meta = (ClampMin = "0.01"))
	float BorderFadeSpeed;

	UPROPERTY(Transient)
	UUserWidget* HitBorderWidget;

	UPROPERTY(Transient)
	UImage* HitBorderImage;

	float CurrentBorderOpacity;
	float TargetBorderOpacity;

	UFUNCTION()
	void OnPlayerHit();
	UFUNCTION()
	void InitializeHitBorderWidget();
	UFUNCTION()
	void SetHitBorderColorAndOpacity();
};
