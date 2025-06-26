#include "PuddleElement.h"
#include "LadyUmbrella/Weapons/Umbrella/Gadgets/ElectricProjectile.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LadyUmbrella/Characters/GenericCharacter.h"

APuddleElement::APuddleElement()
{
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> ObjectMeshAsset(TEXT("/Game/Blueprints/LevelElements/PlaceholderMeshes/Primitives/SM_Cylinder.SM_Cylinder"));
	if (ObjectMeshAsset.Succeeded())
	{
		ObjectMesh->SetStaticMesh(ObjectMeshAsset.Object);
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> SparksEffectAsset(TEXT("/Game/Particles/P_Sparks.P_Sparks"));
	if (SparksEffectAsset.Succeeded())
	{
		SparksEffect = SparksEffectAsset.Object;
	}

	MeshPosition = FVector(0.f, 0.f, 0.0f);
	MeshScale = FVector(1.f, 1.f, 0.1f);
	MeshRotation = FVector::Zero();
}

void APuddleElement::BeginPlay()
{
	Super::BeginPlay();

	if (BoxRoot)
	{
		BoxRoot->OnComponentBeginOverlap.AddDynamic(this, &APuddleElement::OnOverlap);
	}
}

void APuddleElement::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APuddleElement::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// Set mesh transformations
	if (ObjectMesh)
	{
		ObjectMesh->SetRelativeLocation(MeshPosition);
		ObjectMesh->SetRelativeRotation(FRotator(MeshRotation.X, MeshRotation.Y, MeshRotation.Z));
		ObjectMesh->SetRelativeScale3D(MeshScale);
	}
}

void APuddleElement::SetElectrified(bool bNewElectrified)
{
	Super::SetElectrified(bNewElectrified);

	if (bElectrified && SparksEffect && !SparksComponent)
	{
		SparksComponent = UGameplayStatics::SpawnEmitterAttached(SparksEffect, RootComponent);
	}
	else if (!bElectrified && SparksComponent)
	{
		SparksComponent->Deactivate();
		SparksComponent->DestroyComponent();
		SparksComponent = nullptr;
	}
}

void APuddleElement::ResetElectrifiedState()
{
	Super::ResetElectrifiedState();
	SetElectrified(false);
}

void APuddleElement::OnOverlap(UPrimitiveComponent* OverlappedComp,
							 AActor* OtherActor,
							 UPrimitiveComponent* OtherComp,
							 int32 OtherBodyIndex,
							 bool bFromSweep,
							 const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->IsA(AElectricProjectile::StaticClass()))
	{
		SetElectrified(true);
		AElectricProjectile* OverlappedProjectile = Cast<AElectricProjectile>(OtherActor);
		if (OverlappedProjectile)
		{
			OverlappedProjectile->Destroy();
		}
	}

	if (OtherActor && OtherActor->IsA(AGenericCharacter::StaticClass()) && bElectrified)
	{
		Cast<AGenericCharacter>(OtherActor)->SetElectrified(true);
	}
}