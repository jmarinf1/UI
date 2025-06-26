#include "HitReactionComponent.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "LadyUmbrella/Characters/HealthComponent.h"

UHitReactionComponent::UHitReactionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;

	HitMarkerWidgetSlotName = TEXT("GroupSlot");
	HitBorderImageName = TEXT("Image");
	RotationOffsetDegrees = -90.0f;
	MinSaturation = 0.05f;
	MaxSaturation = 0.65f;
	MaxBorderOpacity = 0.75f;
	BorderFadeSpeed = 1.0f;
	CurrentBorderOpacity = 0.0f;
	TargetBorderOpacity = 0.0f;
	BorderFadeRate = 2.f;
	CachedHealth  = -1;

	ConstructorHelpers::FClassFinder<UUserWidget> BPHitMarkerWidgetClass(TEXT("/Game/Widgets/WidgetBlueprints/HUD/WBP_HitMarker"));
	if (BPHitMarkerWidgetClass.Succeeded())
	{
		HitMarkerWidgetClass = BPHitMarkerWidgetClass.Class;
	}
	ConstructorHelpers::FClassFinder<UUserWidget> BPHitBorderWidgetClass(TEXT("/Game/Widgets/WidgetBlueprints/HUD/WBP_HitBorder"));
	if (BPHitBorderWidgetClass.Succeeded())
	{
		HitBorderWidgetClass = BPHitBorderWidgetClass.Class;
	}
	ConstructorHelpers::FObjectFinder<UCurveFloat> SaturationCurveObject(TEXT("/Script/Engine.CurveFloat'/Game/DataStructures/Curves/Systems/C_UIDesaturationRate.C_UIDesaturationRate'"));
	if (SaturationCurveObject.Succeeded())
	{
		SaturationCurve = SaturationCurveObject.Object;
	}
}

void UHitReactionComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* Owner = GetOwner())
	{
		HealthComponent = Owner->FindComponentByClass<UHealthComponent>();
		CachedCameraComponent = Owner->FindComponentByClass<UCameraComponent>();
	}
	
	if (IsValid(HitBorderWidgetClass))
	{
		InitializeHitBorderWidget();
	}
}

void UHitReactionComponent::OnPlayerHit()
{
	SetComponentTickEnabled(true);
    
	TargetBorderOpacity = MaxBorderOpacity;
	
	// Make it NOT fade, but rather pop in
	FLinearColor NewColor = HitBorderImage->GetColorAndOpacity();
	NewColor.A = TargetBorderOpacity;
	HitBorderImage->SetColorAndOpacity(NewColor);
	CurrentBorderOpacity = TargetBorderOpacity;
	
	FTimerHandle FadeOutTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
	FadeOutTimerHandle,
	[this]() { TargetBorderOpacity = 0.0f; },
	BorderFadeRate,
	false
);
}

void UHitReactionComponent::ShowHitMarker(FVector Origin)
{
	if (!IsValid(HitMarkerWidgetClass)) return;

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!IsValid(PC) || !IsValid(GEngine) || !IsValid(GEngine->GameViewport)) return;

	OnPlayerHit();

	FVector CameraLocation;
	FRotator CameraRotation;
	PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

	// Get current camera rotation
	PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

	// Only recalculate vectors if camera rotated significantly
	if (!CameraRotation.Equals(LastCameraRotation, 1.0f)) // 1-degree tolerance
	{
		// Update cached vectors
		FVector CameraForward = CameraRotation.Vector();
		FVector CameraRight = FRotationMatrix(CameraRotation).GetUnitAxis(EAxis::Y);
		// Flatten direction vectors to XY plane
		CachedCameraForward2D = FVector2D(CameraForward.X, CameraForward.Y).GetSafeNormal();
		CachedCameraRight2D = FVector2D(CameraRight.X, CameraRight.Y).GetSafeNormal();
		LastCameraRotation = CameraRotation;
	}


	
	FVector2D ToEnemy2D = FVector2D((Origin - CameraLocation).GetSafeNormal2D());
	// Dot products for screen-space direction
	float RightDot = FVector2D::DotProduct(ToEnemy2D, CachedCameraRight2D);
	float ForwardDot = FVector2D::DotProduct(ToEnemy2D, CachedCameraForward2D);

	// Invert Y (screen coordinates are flipped)
	FVector2D ToTarget(RightDot, -ForwardDot);

	// Handle dead center / zero vector
	ToTarget = ToTarget.IsNearlyZero() ? FVector2D(0.f, -1.f) : ToTarget.GetSafeNormal();
	
	FVector2D FinalPosition = ScreenCenter + ToTarget * WidgetCanvasSize;
	FinalPosition.X = FMath::Clamp(FinalPosition.X, -WidgetCanvasSize.X / 2.f, WidgetCanvasSize.X / 2.f);
	FinalPosition.Y = FMath::Clamp(FinalPosition.Y, -WidgetCanvasSize.Y / 2.f, WidgetCanvasSize.Y / 2.f);

	// Rotate Marker to face center
	FVector2D DirectionToCenter = (ScreenCenter - FinalPosition).GetSafeNormal();
	float AngleRadians = FMath::Atan2(DirectionToCenter.Y, DirectionToCenter.X);
	float AngleDegrees = FMath::RadiansToDegrees(AngleRadians) + RotationOffsetDegrees;

	// Spawn and locate the widget
	// Could totally make an object pool instead of spawning them runtime. Beta wishlist?
	if (UUserWidget* HitMarker = CreateWidget<UUserWidget>(GetWorld(), HitMarkerWidgetClass))
	{
		HitMarker->AddToViewport();

		if (UObject* GroupSlotObj = HitMarker->GetWidgetFromName(HitMarkerWidgetSlotName))
		{
			if (UCanvasPanel* GroupSlot = Cast<UCanvasPanel>(GroupSlotObj))
			{
				FWidgetTransform Transform;
				Transform.Angle = AngleDegrees;
				Transform.Translation = FinalPosition;
				GroupSlot->SetRenderTransform(Transform);
			}
		}
	}
}


void UHitReactionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateCameraSaturation();

	// Fade border effect
	if (IsValid(HitBorderImage) && !FMath::IsNearlyEqual(CurrentBorderOpacity, TargetBorderOpacity, 0.001f))
	{
		CurrentBorderOpacity = FMath::FInterpTo(CurrentBorderOpacity, TargetBorderOpacity, DeltaTime, BorderFadeSpeed);
        
		FLinearColor NewColor = HitBorderImage->GetColorAndOpacity();
		NewColor.A = CurrentBorderOpacity;
		HitBorderImage->SetColorAndOpacity(NewColor);
	}
}

void UHitReactionComponent::UpdateCameraSaturation()
{
	if (!IsValid(HealthComponent) || !IsValid(SaturationCurve)) return;

	const int32 CurrentHealth = HealthComponent->GetHealth();
	if (CurrentHealth == CachedHealth) return;

	CachedHealth = CurrentHealth;

	float HealthRatio = FMath::Clamp(
		static_cast<float>(CurrentHealth) / static_cast<float>(HealthComponent->GetMaxHealth()), 0.f, 1.f);

	float CurveValue = SaturationCurve->GetFloatValue(HealthRatio);
	float Saturation = FMath::Lerp(MinSaturation, MaxSaturation, CurveValue);

	if (CachedCameraComponent)
	{
		CachedCameraComponent->PostProcessSettings.bOverride_ColorSaturation = true;
		CachedCameraComponent->PostProcessSettings.ColorSaturation = FVector4(Saturation, Saturation, Saturation, 1.0f);
	}
	
}


void UHitReactionComponent::InitializeHitBorderWidget()
{
	if (!IsValid(HitBorderWidgetClass))
		return;

	HitBorderWidget = CreateWidget<UUserWidget>(GetWorld(), HitBorderWidgetClass);
	if (!IsValid(HitBorderWidget))
		return;

	HitBorderWidget->AddToViewport();

	SetHitBorderColorAndOpacity();
}

void UHitReactionComponent::SetHitBorderColorAndOpacity()
{
	if (!IsValid(HitBorderWidget))
		return;

	if (UWidget* ImageBorder = HitBorderWidget->GetWidgetFromName(HitBorderImageName))
	{
		HitBorderImage = Cast<UImage>(ImageBorder);
		if (IsValid(HitBorderImage))
		{
			FLinearColor InitialColor = HitBorderImage->GetColorAndOpacity();
			InitialColor.A = 0.0f;
			HitBorderImage->SetColorAndOpacity(InitialColor);
		}
	}
}