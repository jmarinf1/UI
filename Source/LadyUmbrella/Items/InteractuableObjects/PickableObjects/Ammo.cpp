// 
// Ammo.cpp
// 
// Implementation of the Ammo class.
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#include "Ammo.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Weapons/Umbrella/Umbrella.h"
#include "LadyUmbrella/UI/Basics/InteractiveIcon.h"
#include "LadyUmbrella/Util/Logger.h"

#define M_AMMO TEXT("/Script/Engine.MaterialInstanceConstant'/Game/Materials/Instancias/Editor/ReactiveElements/MI_LDGrid_Local_Interactable.MI_LDGrid_Local_Interactable'")
#define SM_AMMO TEXT("StaticMesh'/Game/Blueprints/LevelElements/PlaceholderMeshes/Primitives/SM_Cube.SM_Cube'")
#define WG_PROMPT TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Widgets/WidgetBlueprints/Components/WBP_InteractiveIcons.WBP_InteractiveIcons'")

AAmmo::AAmmo()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("MeshComponent");
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticMeshComponent->SetMaterial(0, ConstructorHelpers::FObjectFinder<UMaterial>(M_AMMO).Object);
	StaticMeshComponent->SetStaticMesh(ConstructorHelpers::FObjectFinder<UStaticMesh>(SM_AMMO).Object);
	StaticMeshComponent->SetupAttachment(GetSphereComponent());
	StaticMeshComponent->SetWorldScale3D(FVector(0.2f, 0.2f, 0.2f));

	ConstructorHelpers::FClassFinder<UInteractiveIcon> WidgetFinder(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Widgets/WidgetBlueprints/Components/WBP_InteractiveIcons.WBP_InteractiveIcons_C'"));

	if (WidgetFinder.Succeeded())
	{
		SetWidgetClass(WidgetFinder.Class);
	}

	SetAmountToPickup(5);
}

void
AAmmo::BeginPlay()
{
	Super::BeginPlay();
}

void
AAmmo::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!PlayerCharacter)
	{
		// FLogger::ErrorLog("Could not find the PlayerCharacter instance in Ammo.Tick()");
		return;
	}
	
	AUmbrella* Umbrella = PlayerCharacter->GetUmbrella();

	if (!Umbrella)
	{
		// FLogger::ErrorLog("Could not find the Umbrella instance in Ammo.Tick()");
		return;
	}

	SetCanInteract(!Umbrella->IsAmmoReserveAtMaxValue());
}

int32
AAmmo::Interacting()
{
	Super::Interacting();

	if (!PlayerCharacter)
	{
		// FLogger::ErrorLog("Could not find the PlayerCharacter instance");
		return false;
	}
	
	AUmbrella* Umbrella = PlayerCharacter->GetUmbrella();

	if (!Umbrella)
	{
		// FLogger::ErrorLog("Could not find the Umbrella instance in Ammo.Tick()");
		return false;
	}	

	if (CanInteract())
	{
		const float MissingReserve = Umbrella->GetAmmoReserveMax() - Umbrella->GetAmmoReserve();

		if (GetAmountToPickup() > MissingReserve)
		{
			SetAmountToPickup(MissingReserve);
		}
	
		Umbrella->SetAmmoReserve(Umbrella->GetAmmoReserve() + GetAmountToPickup());
		(void) Umbrella->OnTotalBulletsChanged.ExecuteIfBound(Umbrella->GetAmmoReserve());

		Destroy();
	}
	
	return GetAmountToPickup();
}

int32 AAmmo::Interacting(APlayerCharacter* Interactor)
{
	Super::Interacting(Interactor);
	
	return 0;
}
