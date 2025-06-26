#include "ZiplineElement.h"

#include "EnhancedInputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
#include "Components/SceneComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/UI/Basics/InteractiveIcon.h"
#include "LadyUmbrella/Util/Logger.h"

AZiplineElement::AZiplineElement()
{
	PrimaryActorTick.bCanEverTick = true;

	DefaultRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultRoot"));
	RootComponent = DefaultRoot;

	Cube1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cube1"));
	Cube1->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Game/Meshes/StaticMeshes/Placeholder/Primitives/Shape_Cube.Shape_Cube"));
	if (CubeMesh.Succeeded())
	{
		Cube1->SetStaticMesh(CubeMesh.Object);
	}

	Cube2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cube2"));
	Cube2->SetupAttachment(RootComponent);
	if (CubeMesh.Succeeded())
	{
		Cube2->SetStaticMesh(CubeMesh.Object);
	}

	StartPoint = CreateDefaultSubobject<USceneComponent>(TEXT("StartPoint"));
	StartPoint->SetupAttachment(RootComponent);

	EndPoint = CreateDefaultSubobject<USceneComponent>(TEXT("EndPoint"));
	EndPoint->SetupAttachment(RootComponent);

	
	ZipLineSpline = CreateDefaultSubobject<USplineComponent>(TEXT("ZipLineSpline"));
	ZipLineSpline->SetupAttachment(RootComponent);

	ZiplineMesh = CreateDefaultSubobject<USplineMeshComponent>(TEXT("ZipLineMesh"));
	ZiplineMesh->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SplineMesh(TEXT("/Game/Blueprints/LevelElements/PlaceholderMeshes/Primitives/SM_Cylinder.SM_Cylinder"));
	if (SplineMesh.Succeeded())
	{
		ZiplineMesh->SetStaticMesh(SplineMesh.Object);
		ZiplineMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		ZiplineMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ZiplineMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		ZiplineMesh->SetGenerateOverlapEvents(true);
	}

	Cube1CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Cube1CollisionBox"));
	Cube1CollisionBox->SetupAttachment(RootComponent);
	Cube1CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Cube1CollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	Cube1CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Cube1CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	Cube1CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnCube1Overlap);
	Cube1CollisionBox->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OffCube1Overlap);


	Cube1Scale = FVector(0.4f, 0.4f, 3.0f);
	Cube2Scale = FVector(0.4f, 0.4f, 3.0f);
	Cube1Rotation = FRotator(0.0f, 0.0f, 0.0f);
	Cube2Rotation = FRotator(0.0f, 0.0f, 0.0f);
	Box1Extent = FVector(300.0f, 500.0f, 50.0f);
	CollisionOffset = FVector(0.0f, 0.0f, 0.0f);
	ZipLineSpeed = 300.0f;
	V_MeshScale = FVector2D(0.1f, 0.1f);
	UIHeightOffset = 10.0f;
	SnappingInterpSpeed = 5.0f;
	SplineTimeOffset = 0.02f;
	IsInRange = false;
	IsSnapping = false;
	ZiplineEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ZiplineEffect"));
	ZiplineEffect->SetupAttachment(RootComponent);
	ZiplineEffect->bAutoActivate = false;
	SnapMinDistanceThreshold = 5.0f;
	CameraLookClampMin = 0.0f;
	CameraLookClampMax = 1.0f;
	MaxStartTime = 0.9;
	
#if WITH_EDITORONLY_DATA
	StartPoint->bVisualizeComponent = true;
	EndPoint->bVisualizeComponent = true;
#endif
	
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAsset(TEXT("/Game/Particles/P_Sparks.P_Sparks"));
	if (ParticleAsset.Succeeded())
	{
		ZiplineEffect->SetTemplate(ParticleAsset.Object);
	}

	ConstructorHelpers::FClassFinder<UInteractiveIcon> WidgetFinder(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Widgets/WidgetBlueprints/Components/WBP_JumpIcons.WBP_JumpIcons_C'"));

	if (WidgetFinder.Succeeded())
	{
		SetWidgetClass(WidgetFinder.Class);
	}

	// WORKAROUND to make zipline compatible with generic interactable.
	GetSphereComponent()->SetupAttachment(RootComponent);
}

void AZiplineElement::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// Set Cube transforms relative to the actor
	Cube1->SetRelativeLocation(Cube1Position);
	Cube1->SetRelativeScale3D(Cube1Scale);
	Cube1->SetRelativeRotation(Cube1Rotation);

	Cube2->SetRelativeLocation(Cube2Position);
	Cube2->SetRelativeScale3D(Cube2Scale);
	Cube2->SetRelativeRotation(Cube2Rotation);

	// Compute StartPoint and EndPoint world positions from Cube transforms + offsets
	const FVector StartWorld = Cube1->GetComponentTransform().TransformPosition(StartOffset);
	const FVector EndWorld = Cube2->GetComponentTransform().TransformPosition(EndOffset);

	StartPoint->SetWorldLocation(StartWorld);
	EndPoint->SetWorldLocation(EndWorld);

	// Setup the spline between StartPoint and EndPoint
	ZipLineSpline->ClearSplinePoints(false);
	ZipLineSpline->AddSplinePoint(StartWorld, ESplineCoordinateSpace::World, false);
	ZipLineSpline->AddSplinePoint(EndWorld, ESplineCoordinateSpace::World, false);
	ZipLineSpline->UpdateSpline();

	// Configure the spline mesh
	const FVector StartMeshPos = StartWorld + FVector(0, 0, CableHeightOffset);
	const FVector EndMeshPos = EndWorld + FVector(0, 0, CableHeightOffset);

	ZiplineMesh->SetStartPosition(StartMeshPos);
	ZiplineMesh->SetEndPosition(EndMeshPos);

	ZiplineMesh->SetStartTangent(ZipLineSpline->GetTangentAtSplinePoint(0, ESplineCoordinateSpace::World));
	ZiplineMesh->SetEndTangent(ZipLineSpline->GetTangentAtSplinePoint(1, ESplineCoordinateSpace::World));

	ZiplineMesh->SetStartScale(V_MeshScale);
	ZiplineMesh->SetEndScale(V_MeshScale);

	// Position Cube1's collision box
	const FVector CollisionBoxWorld = Cube1->GetComponentTransform().TransformPosition(CollisionOffset);
	Cube1CollisionBox->SetWorldLocation(CollisionBoxWorld);
	Cube1CollisionBox->SetBoxExtent(Box1Extent);
}


void AZiplineElement::BeginPlay()
{
	Super::BeginPlay();
	ZiplineMesh->OnComponentBeginOverlap.AddDynamic(this, &AZiplineElement::OnSplineOverlapBegin);
	
	PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	AdjustSnappingPoint();
}

void AZiplineElement::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!PlayerCharacter) return;

	if (PlayerCharacter->IsInZipline() && CurrentSplineTime <= 1.0f)
	{
		MoveCharacterAlongSpline();
	}

	if (IsInRange && !IsSnapping && !PlayerCharacter->IsInZipline())
	{
		AdjustSnappingPoint();
	}

	if (IsSnapping && !PlayerCharacter->IsInZipline())
	{
		const FVector CurrentLocation = PlayerCharacter->GetActorLocation();
		const FVector InterpolatedLocation = FMath::VInterpTo(CurrentLocation, SnapLocation, DeltaTime, SnappingInterpSpeed);
		PlayerCharacter->SetActorLocation(InterpolatedLocation);

		if (FVector::Dist(InterpolatedLocation, SnapLocation) <= SnapMinDistanceThreshold)
		{
			PlayerCharacter->SetActorLocation(SnapLocation);
			IsSnapping = false;
		}
	}
}

void AZiplineElement::AdjustSnappingPoint()
{
	const float CameraLookOffset = FMath::Clamp(FVector::DotProduct(PlayerCharacter->GetFollowCamera()->GetRightVector(),
													 ZipLineSpline->GetForwardVector()), CameraLookClampMin, CameraLookClampMax);
	const FVector ClosestPoint = ZipLineSpline->FindLocationClosestToWorldLocation(PlayerCharacter->GetActorLocation(), ESplineCoordinateSpace::Local);
	const float SplinePoint = ZipLineSpline->GetDistanceAlongSplineAtLocation(ClosestPoint, ESplineCoordinateSpace::Local);
	CurrentSplineTime = ZipLineSpline->GetTimeAtDistanceAlongSpline(SplinePoint) + SplineTimeOffset*CameraLookOffset;
	SnapLocation = ZipLineSpline->GetLocationAtTime(CurrentSplineTime, ESplineCoordinateSpace::World) + FVector(0, 0, CableHeightOffset);
	GetWidgetComponent()->SetWorldLocation(SnapLocation + FVector(0, 0, UIHeightOffset));
}

void AZiplineElement::OnCube1Overlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (APlayerCharacter* PC = Cast<APlayerCharacter>(OtherActor))
	{
		PlayerCharacter = PC;
		if (!PlayerCharacter->IsInZipline())
		{
			// PickupWidgetComponent->SetVisibility(true);

			if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
			{
				EnableInput(PlayerController);

				if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
				{
					EnhancedInput->BindAction(JumpAction, ETriggerEvent::Started, this, &ThisClass::Jump);
				}

				IsInRange = true;
			}
		}
	}
}

void AZiplineElement::OffCube1Overlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA(APlayerCharacter::StaticClass()))
	{
		// PickupWidgetComponent->SetVisibility(false);
		IsInRange = false;
	}
}

void AZiplineElement::OnSplineOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (APlayerCharacter* PC = Cast<APlayerCharacter>(OtherActor))
	{
		PlayerCharacter = PC;
		if (!PlayerCharacter->IsInZipline())
		{
			IsInRange = true;
			const FVector ClosestPoint = ZipLineSpline->FindLocationClosestToWorldLocation(OtherActor->GetActorLocation(), ESplineCoordinateSpace::Local);
			float SplinePoint = ZipLineSpline->GetDistanceAlongSplineAtLocation(ClosestPoint, ESplineCoordinateSpace::Local);
			CurrentSplineTime = ZipLineSpline->GetTimeAtDistanceAlongSpline(SplinePoint);
			if (CurrentSplineTime <= MaxStartTime)
			{
				ProcessOverlap(OtherActor);
			}
		}
	}
}

void AZiplineElement::MoveCharacterAlongSpline()
{
	if (!PlayerCharacter) return;

	const FVector NewPosition = ZipLineSpline->GetLocationAtTime(CurrentSplineTime, ESplineCoordinateSpace::World);
	const FVector SplineTangent = ZipLineSpline->GetTangentAtTime(CurrentSplineTime, ESplineCoordinateSpace::World);
	const FRotator NewRotation = SplineTangent.Rotation();

	GetInteractiveIcon()->SetShouldShow(false);
	PlayerCharacter->SetActorLocation(NewPosition);
	PlayerCharacter->SetActorRotation(FRotator(PlayerCharacter->GetActorRotation().Pitch, NewRotation.Yaw, PlayerCharacter->GetActorRotation().Roll));

	CurrentSplineTime += (ZipLineSpeed * GetWorld()->DeltaTimeSeconds) / ZipLineSpline->GetSplineLength();

	if (CurrentSplineTime >= SplineMaxTime)
	{
		// reset all when finished
		GetWorld()->GetTimerManager().ClearTimer(MovementTimerHandle);
		PlayerCharacter->SetInZipline(false);
		CurrentSplineTime = 0.0f;
		IsSnapping = false;
		IsInRange = false;
		GetInteractiveIcon()->SetShouldShow(true);
		if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
		{
			DisableInput(PlayerController);
		}
		if (ZiplineEffect)
		{
			ZiplineEffect->DeactivateSystem();
			ZiplineEffect->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		}
	}
}

void AZiplineElement::ProcessOverlap(AActor* OtherActor)
{
	if (APlayerCharacter* PC = Cast<APlayerCharacter>(OtherActor))
	{
		PlayerCharacter = PC;
		PlayerCharacter->SetInZipline(true);

		GetWorld()->GetTimerManager().SetTimer(MovementTimerHandle, this, &AZiplineElement::MoveCharacterAlongSpline, 0.01f, true);

		if (ZiplineEffect)
		{
			ZiplineEffect->AttachToComponent(PlayerCharacter->GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			ZiplineEffect->SetRelativeLocation(FVector(0.0f, 0.0f, CableHeightOffset));
			ZiplineEffect->ActivateSystem(true);
		}
	}
}

void AZiplineElement::Jump()
{
	if(IsValid(PlayerCharacter) && IsInRange) IsSnapping = true;
}
