// Fill out your copyright notice in the Description page of Project Settings.


#include "VerticalHookMovement.h"

#include "Components/SplineComponent.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Util/Logger.h"

AVerticalHookMovement::AVerticalHookMovement()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	
	StaticMeshComponent		      = CreateDefaultSubobject<UStaticMeshComponent>("Static Mesh Component");
	ArrowComponent				  = CreateDefaultSubobject<UArrowComponent>("Arrow Component");
	PlayerMovementSplineComponent = CreateDefaultSubobject<USplineComponent>("Player Movement Spline Component");
	CameraMovingComponent		  = CreateDefaultSubobject<UCameraComponent>("CameraMoving Component");

	SplineMoveSpeed = 1000.0f;
	BlendTime		= 1.5f;
	
	PlayerMovementSplineComponent->SetupAttachment(GetRootComponent());
	ArrowComponent->SetupAttachment(GetRootComponent());
	StaticMeshComponent->SetupAttachment(GetRootComponent());
	CameraMovingComponent->SetupAttachment(GetRootComponent());
}

void AVerticalHookMovement::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AVerticalHookMovement::ClearTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(SplineTimer);
}

void AVerticalHookMovement::BeginPlay()
{
	Super::BeginPlay();
}

bool AVerticalHookMovement::CheckPlayerComponentsAndVisibility(APlayerCharacter*& PlayerInteractor, UCharacterMovementComponent*& CharMovComp,
	APlayerController*& PlayerController)
{
	bool CheckVisibility = PlayerInteractor->GetCameraSystem()->IsComponentInFrustum(ArrowComponent);	
    
	if (!CheckVisibility)
	{
		FLogger::DebugLog("No veo");
		ClearTimer();
		return 0;
	}
	
	if ( !IsValid(PlayerInteractor) )
	{
		ClearTimer();
		return false;
	}
	
	CharMovComp = PlayerInteractor->GetCharacterMovement();
	PlayerController = Cast<APlayerController>(PlayerInteractor->GetController());

	if (!IsValid(CharMovComp) || !IsValid(PlayerController))
	{
		ClearTimer();
		return false;
	}
	return true;
}

int32 AVerticalHookMovement::Interacting(APlayerCharacter* Interactor)
{
	
	UCharacterMovementComponent* CharMovComp = nullptr;
	APlayerController* PlayerController		 = nullptr;

	if (!CheckPlayerComponentsAndVisibility(Interactor, CharMovComp, PlayerController))
	{
		return 0;
	}
	
	PlayerController->SetViewTargetWithBlend(this, BlendTime, VTBlend_EaseInOut, 1, false);
	PlayerController->FlushPressedKeys();
	
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUFunction(this, "MovePlayerToStartOfSpline", Interactor);
	GetWorld()->GetTimerManager().SetTimer(MovementToSplineTimer, TimerDelegate, GetWorld()->DeltaTimeSeconds, true, 1.f);
	
	return 0;
	
}

void AVerticalHookMovement::MovePlayerToStartOfSpline(APlayerCharacter* Interactor)
{
	FVector splineworldpos = PlayerMovementSplineComponent->GetWorldLocationAtTime(0);
	FVector InteractorPosition = Interactor->GetActorLocation(); 
	FVector Speed = InteractorPosition - splineworldpos;
		
	Speed.Normalize();
	
	Interactor->AddMovementInput(Speed, -1,false);
	FVector2d Speed2d(Speed.X, Speed.Y);
	
	if ( Speed2d.IsNearlyZero(0.01) )
	{
		
		Interactor->GetCharacterMovement()->SetMovementMode(MOVE_Flying);

		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUFunction(this, "StartSplineMovement", Interactor);
		GetWorld()->GetTimerManager().ClearTimer(MovementToSplineTimer);
		GetWorld()->GetTimerManager().SetTimer(SplineTimer, TimerDelegate, GetWorld()->DeltaTimeSeconds, true);
	}
}

void AVerticalHookMovement::StartSplineMovement(APlayerCharacter* Interactor)
{
	if (!IsValid(Interactor))
	{
		GetWorld()->GetTimerManager().ClearTimer(SplineTimer);
		return;
	}

	const float DeltaTime = GetWorld()->GetDeltaSeconds();
	SplineProgress += SplineMoveSpeed * DeltaTime;

	const FVector NewLocation = PlayerMovementSplineComponent->GetLocationAtDistanceAlongSpline(SplineProgress, ESplineCoordinateSpace::World);

	Interactor->SetActorLocation(NewLocation);
	
	if (SplineProgress >= PlayerMovementSplineComponent->GetSplineLength())
	{
		SplineProgress = 0;
		Cast<APlayerController>(PlayerCharacter->GetController())->SetViewTargetWithBlend(PlayerCharacter, BlendTime, VTBlend_Linear, 0, false);

		GetWorld()->GetTimerManager().ClearTimer(SplineTimer);
		Interactor->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
}