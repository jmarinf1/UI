// 
// GenericInteractable.cpp
// 
// Implementation of the GenericInteractable class.
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#include "GenericInteractable.h"

#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/UI/Basics/InteractiveIcon.h"
#include "LadyUmbrella/Util/Logger.h"

AGenericInteractable::AGenericInteractable()
{
	PrimaryActorTick.bCanEverTick = true;
	/*
	SceneRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	SetRootComponent(SceneRootComponent);
	*/
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->InitSphereRadius(150.0f);
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComponent->SetGenerateOverlapEvents(true);

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractWidget"));
	WidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WidgetComponent->SetCastShadow(false);
	WidgetComponent->SetVisibility(true);
	WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);

	InnerIconSize  = 1.5f;
	OuterIconSize  = 0.5f;
	AnimationSpeed = 0.1f;

	SphereComponent->SetupAttachment(GetRootComponent());
	WidgetComponent->SetupAttachment(GetRootComponent());
	
	WidgetOffset = WidgetComponent->GetComponentLocation();
	InnerSphereTriggerRadius = SphereComponent->GetUnscaledSphereRadius();
	OuterSphereTriggerRadius = SphereComponent->GetUnscaledSphereRadius() * 6.0f;

	bCanInteract = true;
}

void AGenericInteractable::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	if (IsValid(WidgetComponent))
	{
		WidgetComponent->SetWidgetClass(InteractiveIcon);
	}
}

void AGenericInteractable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsValid(PlayerCharacter) || !IsValid(WidgetComponent))
	{
		FLogger::ErrorLog("Bad pointers in AGenericInteractable.Tick()");
		return;
	}
	
	if (IsValid(PlayerCharacter))
	{
		SetDeviceName(PlayerCharacter->GetPlayerDevice());
	}

	UInteractiveIcon* Icon = Cast<UInteractiveIcon>(WidgetComponent->GetWidget());

	if (!IsValid(Icon))
	{
		FLogger::ErrorLog("Invalid InteractiveIcon pointer in AGenericInteractable.Tick()");
		return;
	}

	Icon->Update(this, OuterIconSize, OuterSphereTriggerRadius, InnerIconSize,
		InnerSphereTriggerRadius, AnimationSpeed, bCanInteract, DeviceName);
}

int32 AGenericInteractable::Interacting()
{
	return 0;
}

int32 AGenericInteractable::Interacting(APlayerCharacter* Interactor)
{
	return 0;
}

USphereComponent* AGenericInteractable::GetSphereComponent() const
{
	return SphereComponent;
}

UWidgetComponent* AGenericInteractable::GetWidgetComponent() const
{
	return WidgetComponent;
}

UInteractiveIcon* AGenericInteractable::GetInteractiveIcon() const
{
	return Cast<UInteractiveIcon>(GetWidgetComponent()->GetWidget());
}

FString AGenericInteractable::GetDeviceName()
{
	return DeviceName;
}

void AGenericInteractable::SetDeviceName(FString Name)
{
	DeviceName = Name;
}

void AGenericInteractable::SetWidgetClass(TSubclassOf<UInteractiveIcon> WidgetClass)
{
	InteractiveIcon = WidgetClass;
}
