#include "SwingRope.h"
#include "Components/BillboardComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/TextRenderComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Util/Logger.h"
#include "Components/TimelineComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LadyUmbrella/Util/MathLibraries/ParabolicMovement.h"

// Sets default values
ASwingRope::ASwingRope()
	: bShowDebugLines(false)
	, SwingAnimationPlayRate(1)
	, SwingTimeLinePlayRate(1)
	, TravelTimeToSwingLocation(1.f)
	, TravelTimeToTargetLocation(1)
	, MinFacingDotThreshold(0.6f)
	, TimelineLaunchMoment(0.4)
	, bEnterPrimarySide(true)
	, Cooldown(0.5)
	, bIsInCooldown(false)
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	
	VisibleRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VisibleRoot"));
	VisibleRoot->SetupAttachment(Root);
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Game/StarterContent/Props/SM_PillarFrame.SM_PillarFrame'"));
	UStaticMesh* Asset = MeshAsset.Object;
	StaticMeshComponent->SetStaticMesh(Asset);
	StaticMeshComponent->AddLocalRotation(FRotator(180, 0, 0));
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	StaticMeshComponent->SetupAttachment(VisibleRoot);

	StaticMeshFinalRotationA = StaticMeshComponent->GetRelativeRotation() + FRotator(0, 0, 40);
	StaticMeshFinalRotationB = StaticMeshComponent->GetRelativeRotation() - FRotator(0, 0, 40);
	
	BoxCollider1 = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider1"));
	BoxCollider1->SetupAttachment(VisibleRoot);
	BoxCollider1->SetRelativeTransform(FTransform(FRotator(180, 0, 0), FVector(0, 100, 200), FVector(1.0, 1.0, 1.0)));
	BoxCollider1->SetBoxExtent(FVector(200, 50, 200), false);
	
	BoxCollider2 = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider2"));
	BoxCollider2->SetupAttachment(VisibleRoot);
	BoxCollider2->SetRelativeTransform(FTransform(FRotator(180, 0, 0), FVector(0, -100, 200), FVector(1.0, 1.0, 1.0)));
	BoxCollider2->SetBoxExtent(FVector(200, 50, 200), false);

	SwingLocation1 = CreateDefaultSubobject<UBillboardComponent>(TEXT("SwingLocation1"));
	SwingLocation1->SetupAttachment(StaticMeshComponent);
	SwingLocation1->SetRelativeTransform(FTransform(FRotator(180, 90, 0), FVector(10, 100, 200), FVector(1.0, 1.0, 1.0)));
	
	SwingLocation2 = CreateDefaultSubobject<UBillboardComponent>(TEXT("SwingLocation2"));
	SwingLocation2->SetupAttachment(StaticMeshComponent);
	SwingLocation2->SetRelativeTransform(FTransform(FRotator(180, -90, 0), FVector(0, -100, 200), FVector(1.0, 1.0, 1.0)));

	SwingLocation1Label = CreateDefaultSubobject<UTextRenderComponent>(TEXT("SwingLocation1Label"));
	SwingLocation1Label->SetupAttachment(SwingLocation1);
	SwingLocation1Label->SetRelativeLocation(FVector(0, 0, 50));
	SwingLocation1Label->SetText(FText::FromString("Swing Location A"));
	SwingLocation1Label->SetHorizontalAlignment(EHTA_Center);
	SwingLocation1Label->SetTextRenderColor(FColor::Blue);
	SwingLocation1Label->SetWorldSize(30.f);
	SwingLocation1Label->AddLocalRotation(FRotator(0, 180, 0));
	SwingLocation1Label->SetHiddenInGame(true);

	SwingLocation2Label = CreateDefaultSubobject<UTextRenderComponent>(TEXT("SwingLocation2Label"));
	SwingLocation2Label->SetupAttachment(SwingLocation2);
	SwingLocation2Label->SetRelativeLocation(FVector(0, 0, 50));
	SwingLocation2Label->SetText(FText::FromString("Swing Location B"));
	SwingLocation2Label->SetHorizontalAlignment(EHTA_Center);
	SwingLocation2Label->SetTextRenderColor(FColor::Red);
	SwingLocation2Label->SetWorldSize(30.f);
	SwingLocation2Label->AddLocalRotation(FRotator(0, 180, 0));
	SwingLocation2Label->SetHiddenInGame(true);

	
	TargetPoint1 = CreateDefaultSubobject<UBillboardComponent>(TEXT("TargetPoint1"));
	TargetPoint1->SetupAttachment(VisibleRoot);
	TargetPoint1->SetRelativeLocation(FVector(0, 300, 300));
	TargetPoint1->AddLocalRotation(FRotator(180, -90, 0));

	TargetPoint2 = CreateDefaultSubobject<UBillboardComponent>(TEXT("TargetPoint2"));
	TargetPoint2->SetupAttachment(VisibleRoot);
	TargetPoint2->SetRelativeLocation(FVector(0, -300, 300));
	TargetPoint2->AddLocalRotation(FRotator(180, 90, 0));

	TargetPoint1Label = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TargetPoint1Label"));
	TargetPoint1Label->SetupAttachment(TargetPoint1);
	TargetPoint1Label->SetRelativeLocation(FVector(0, 0, 50));
	TargetPoint1Label->SetText(FText::FromString("Target B"));
	TargetPoint1Label->SetHorizontalAlignment(EHTA_Center);
	TargetPoint1Label->SetTextRenderColor(FColor::Red);
	TargetPoint1Label->SetWorldSize(30.f);
	TargetPoint1Label->SetHiddenInGame(true);

	TargetPoint2Label = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TargetPoint2Label"));
	TargetPoint2Label->SetupAttachment(TargetPoint2);
	TargetPoint2Label->SetRelativeLocation(FVector(0, 0, 50));
	TargetPoint2Label->SetText(FText::FromString("Target A"));
	TargetPoint2Label->SetHorizontalAlignment(EHTA_Center);
	TargetPoint2Label->SetTextRenderColor(FColor::Blue);
	TargetPoint2Label->SetWorldSize(30.f);
	TargetPoint2Label->SetHiddenInGame(true);

	SwingTimeLine = CreateDefaultSubobject<UTimelineComponent>(TEXT("SwingTimeLine"));
	
	ConstructorHelpers::FObjectFinder<UAnimMontage> MontageAsset(TEXT("AnimMontage'/Game/Animations/Montages/AllCharacters/Movement/AM_Swing.AM_Swing'"));
	if (MontageAsset.Succeeded())
	{
		SwingMontage = MontageAsset.Object;
	}
}

// Called when the game starts or when spawned
void ASwingRope::BeginPlay()
{
	Super::BeginPlay();
	
	StaticMeshOriginalRotation = StaticMeshComponent->GetRelativeRotation();

	if (IsValid(SwingTimeLine))
	{
		SwingTimeLine->SetPlayRate(SwingTimeLinePlayRate);
		
		FOnTimelineFloat SwingProgressUpdate;
		SwingProgressUpdate.BindUFunction(this, FName("SwingTimelineProgress"));

		FOnTimelineEvent SwingFinishedEvent;
		SwingFinishedEvent.BindUFunction(this, FName("SwingTimelineFinish"));
		
		if (IsValid(SwingRopeMovementCurve))
		{
			SwingTimeLine->AddInterpFloat(SwingRopeMovementCurve, SwingProgressUpdate);
			SwingTimeLine->SetTimelineFinishedFunc(SwingFinishedEvent);
		}
	}
}

// Called every frame
void ASwingRope::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ASwingRope::Interact(AActor* OtherActor)
{
	if (SwingTimeLine->IsReversing())
	{
		return false;
	}
	
	if (bIsInCooldown)
	{
		return false;
	}
	
	PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
	if (!PlayerCharacter)
	{
		return false;
	}
	
	MovementComponent = PlayerCharacter->GetCharacterMovement();
	if (!MovementComponent)
	{
		return false;
	}
	
	// CHECK IF PRIMARY SIDE
	FVector PlayerLocation = PlayerCharacter->GetActorLocation();
	float MinDistance = FVector::Dist(PlayerLocation, SwingLocation1->GetComponentLocation());
	UBillboardComponent* CurrentSwingLocation;
	
	if (MinDistance < FVector::Dist(PlayerLocation, SwingLocation2->GetComponentLocation()))
	{
		bEnterPrimarySide = true;
		TargetLocation = TargetPoint2->GetComponentLocation();
		CurrentSwingLocation = SwingLocation1;
	}
	else
	{
		bEnterPrimarySide = false;
		TargetLocation = TargetPoint1->GetComponentLocation();
		CurrentSwingLocation = SwingLocation2;
	}

	if (!CheckIfPlayerFacingBar(CurrentSwingLocation->GetForwardVector()))
	{
		return false;
	}

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PlayerController)
	{
		PlayerCharacter->GetCharacterMovement()->Velocity = FVector(0, 0, 0);
		PlayerController->SetIgnoreMoveInput(true);
	}
	
	MovementComponent->SetMovementMode(MOVE_Flying);
	PlayerCharacter->SetActorRotation(CurrentSwingLocation->GetComponentRotation());
	LaunchParabolicTo(PlayerLocation, CurrentSwingLocation->GetComponentLocation(), TravelTimeToSwingLocation);
	
	PlayerCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	GetWorldTimerManager().SetTimer(MoveToSwingLocationTimerHandler, this, &ASwingRope::MoveComponentToSwingTargetFinished, TravelTimeToSwingLocation, false);

	PlayerCharacter->SetIsSwinging(true);
	return true;
}

void ASwingRope::MoveComponentToSwingTargetFinished() const
{
	MovementComponent->StopMovementImmediately();
	MovementComponent->SetMovementMode(MOVE_Flying);

	if (bEnterPrimarySide)
	{
		PlayerCharacter->AttachToComponent(SwingLocation1, FAttachmentTransformRules::KeepWorldTransform);
	}
	else
	{
		PlayerCharacter->AttachToComponent(SwingLocation2, FAttachmentTransformRules::KeepWorldTransform);
	}
	
	if (IsValid(SwingTimeLine))
	{
		SwingTimeLine->PlayFromStart();
		//Swing();
	}
}

void ASwingRope::SwingTimelineProgress(float Value)
{
	FRotator CurrentRot;
	if (bEnterPrimarySide)
	{
		CurrentRot = FMath::Lerp(StaticMeshOriginalRotation, StaticMeshFinalRotationB, Value);
	}
	else
	{
		CurrentRot = FMath::Lerp(StaticMeshOriginalRotation, StaticMeshFinalRotationA, Value);
	}
	if (SwingTimeLine->GetPlaybackPosition() >= TimelineLaunchMoment)
	{
		if (PlayerCharacter->IsSwinging() && !bIsInCooldown)
		{
			bIsInCooldown = true;
			PlayerCharacter->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			
			LaunchParabolicTo(PlayerCharacter->GetActorLocation(), TargetLocation, TravelTimeToTargetLocation);
			
			APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
			if (PlayerController)
			{
				PlayerController->SetIgnoreMoveInput(false);
			}
			
			PlayerCharacter->SetIsSwinging(false);
			PlayerCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
			GetWorldTimerManager().SetTimer(ParabolicLaunchTimerHandler, this, &ASwingRope::TryToClearParabolicLaunchTimer, TravelTimeToTargetLocation, false);
			GetWorldTimerManager().SetTimer(CooldownTimerHandler, this, &ASwingRope::CooldownReset, Cooldown * 2, false);
		}
	}
	StaticMeshComponent->SetRelativeRotation(CurrentRot);
}

void ASwingRope::SwingTimelineFinish() const
{
	if (SwingTimeLine->GetPlaybackPosition() != 0)
	{
		SwingTimeLine->Reverse();
	}
}

bool ASwingRope::CheckIfPlayerFacingBar(FVector RefLocation)
{
	float DotProduct = FVector::DotProduct(RefLocation, PlayerCharacter->GetActorForwardVector());

	if (DotProduct < MinFacingDotThreshold)
	{
		return false;
	}
	return true;
}


void ASwingRope::CooldownReset()
{
	bIsInCooldown = false;
}

void ASwingRope::Swing()
{
	UAnimInstance* AnimInstance = PlayerCharacter->GetMesh()->GetAnimInstance();

	if (IsValid(AnimInstance) && IsValid(SwingMontage))
	{
		AnimInstance->OnMontageEnded.AddDynamic(this, &ASwingRope::OnSwingCompleted);
		AnimInstance->Montage_Play(SwingMontage, SwingAnimationPlayRate,  EMontagePlayReturnType::MontageLength, 0);
	}
}

void ASwingRope::OnSwingCompleted(UAnimMontage* Montage, bool bInterrupted)
{
	if (IsValid(Montage) && Montage == SwingMontage)
	{
		PlayerCharacter->GetMesh()->GetAnimInstance()->OnMontageEnded.RemoveDynamic(this, &ASwingRope::OnSwingCompleted);
	}
}

void ASwingRope::LaunchParabolicTo(const FVector& Start, const FVector& End, const float Time) const
{
	MovementComponent->SetMovementMode(MOVE_Falling);
	float GravityZ = MovementComponent->GetGravityZ();

	FVector LaunchVelocity = ParabolicMovement::CalculateParabolicLaunchVelocity(Start, End, Time, GravityZ);

	if (bShowDebugLines)
	{
		ParabolicMovement::DrawDebugParabola(GetWorld(), Start, LaunchVelocity, GravityZ, Time);
		UKismetSystemLibrary::DrawDebugArrow(GetWorld(), Start, End, 100.0f, FLinearColor::Green, 2.0f, 3.0f);
	}

	MovementComponent->Launch(LaunchVelocity);
}

void ASwingRope::TryToClearParabolicLaunchTimer()
{
	if (PlayerCharacter->IsSwinging())
	{
		return;
	}
	
	GetWorldTimerManager().ClearTimer(ParabolicLaunchTimerHandler);
}


