// Fill out your copyright notice in the Description page of Project Settings.


#include "ReactiveElement.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
AReactiveElement::AReactiveElement()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/* MESH */
	BoxRoot = CreateDefaultSubobject<UBoxComponent>(TEXT("MeshRoot"));
	BoxRoot->CanCharacterStepUpOn = ECB_No;
	BoxRoot->SetShouldUpdatePhysicsVolume(true);
	BoxRoot->SetCanEverAffectNavigation(false);
	BoxRoot->bDynamicObstacle = true;
	SetRootComponent(BoxRoot);
	//Root = BoxRoot;
	
	ObjectMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	if (ObjectMesh)
	{
		ObjectMesh->bCastDynamicShadow = true;
		ObjectMesh->bAffectDynamicIndirectLighting = true;
		ObjectMesh->PrimaryComponentTick.TickGroup = TG_DuringPhysics;
		ObjectMesh->SetupAttachment(BoxRoot);
		ObjectMesh->SetGenerateOverlapEvents(false);
		ObjectMesh->SetCanEverAffectNavigation(true);
		ObjectMesh->SetSimulatePhysics(false);
		ObjectMesh->SetCollisionProfileName(TEXT("ReactItem"));
	}
}

// Called when the game starts or when spawned
void AReactiveElement::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AReactiveElement::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AReactiveElement::Reaction(HitSource Source)
{
	ShouldReact(Source);
}

bool AReactiveElement::ShouldReact(HitSource Source)
{
	if (Reacted || HittableBy.Num() < 1 || Source == HitSource::NONE)
	{
		return false;
	}

	if (HittableBy.Contains(Source)) 
	{
		Reacted = true; 
		OnReactedDelegate.Broadcast();
		return true; 
	}

	return false;
}
