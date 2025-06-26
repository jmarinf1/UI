// Fill out your copyright notice in the Description page of Project Settings.


#include "UpgradePieces.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "LadyUmbrella/UI/Basics/InteractiveIcon.h"

AUpgradePieces::AUpgradePieces()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticMeshComponent->SetupAttachment(RootComponent);

	ConstructorHelpers::FClassFinder<UInteractiveIcon> WidgetFinder(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Widgets/WidgetBlueprints/Components/WBP_InteractiveIcons.WBP_InteractiveIcons_C'"));

	if (WidgetFinder.Succeeded())
	{
		SetWidgetClass(WidgetFinder.Class);
	}
	
	SetAmountToPickup(1);
}

int32 AUpgradePieces::Interacting()
{
	Destroy();

	return GetAmountToPickup();
}

int32 AUpgradePieces::Interacting(APlayerCharacter* Interactor)
{
	Super::Interacting(Interactor);
	
	return 0;
}