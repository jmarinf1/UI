// 
// GenericProjectile.cpp
// 
// Implementation of the GenericProjectile class. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#include "GenericProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "LadyUmbrella/Interfaces/HealthInterface.h"

AGenericProjectile::AGenericProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	ProjectileDamage = 10.0f;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->InitSphereRadius(15.0f);
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AGenericProjectile::ProcessCollision);

	RootComponent = SphereComponent.Get();

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->SetUpdatedComponent(SphereComponent.Get());
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bShouldBounce            = true;
	ProjectileMovementComponent->Bounciness               = 0.3f;
	ProjectileMovementComponent->InitialSpeed             = 3000.0f;
	ProjectileMovementComponent->MaxSpeed                 = 3000.0f;
	ProjectileMovementComponent->ProjectileGravityScale   = 0.0f;
}

void AGenericProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void AGenericProjectile::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGenericProjectile::ProcessCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//  if (OtherActor->Implements<UHealthInterface>())
	// {
	// 	if (IHealthInterface* HealthInterface = Cast<IHealthInterface>(OtherActor))
	// 	{
	// 		HealthInterface->SubtractHealth(ProjectileDamage,);
	// 	}
	// }

	Destroy();
}
