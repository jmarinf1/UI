#include "ElectricProjectile.h"
#include "LadyUmbrella/Weapons/GenericProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "LadyUmbrella/Characters/GenericCharacter.h"

#include "LadyUmbrella/Weapons/Umbrella/Umbrella.h"


AElectricProjectile::AElectricProjectile()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CMesh(TEXT("StaticMesh'/Game/Meshes/StaticMeshes/Placeholder/Primitives/SM_Sphere.SM_Sphere'"));
	
	if (IsValid(CMesh.Object))
    {
		StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SphereObject"));
		StaticMeshComponent->SetStaticMesh(CMesh.Object);
		StaticMeshComponent->SetRelativeScale3D(FVector(0.175f,0.175f,0.175f));
		StaticMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -8.0f));
		StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		static ConstructorHelpers::FObjectFinder<UMaterial> CMat(TEXT("/Script/Engine.MaterialInstanceConstant'/Game/Materials/Instancias/Editor/Gradient/Reactive/MI_GradientElectrifiableBlue.MI_GradientElectrifiableBlue_C'"));

		if (IsValid(CMat.Object))
		{
			StaticMeshComponent->SetMaterial(0, CMat.Object);
		}
    }
	StaticMeshComponent->SetupAttachment(SphereComponent.Get());
	SphereComponent->InitSphereRadius(15.0f);
	GetSphereComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetSphereComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetSphereComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	GetSphereComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	if (IsValid(GetProjectileMovementComponent().Get()))
	{
		GetProjectileMovementComponent().Get()->bShouldBounce = false;
		GetProjectileMovementComponent().Get()->Bounciness = 0.0f;
		GetProjectileMovementComponent().Get()->ProjectileGravityScale = 0.2f;
	}

	DamageToUmbrella = 20;
}

void AElectricProjectile::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("ElectricProjectile spawned at location: %s"), *GetActorLocation().ToString());
}
void AElectricProjectile::ProcessCollision(UPrimitiveComponent* OverlappedComponent, 
									   AActor* OtherActor, 
									   UPrimitiveComponent* OtherComp, 
									   int32 OtherBodyIndex, 
									   bool bFromSweep, 
									   const FHitResult& SweepResult)
{
	if (OtherActor && !OtherActor->ActorHasTag(FName("AgencyShield")))
	{
		//Destroy
		this->Destroy();
	}
}
