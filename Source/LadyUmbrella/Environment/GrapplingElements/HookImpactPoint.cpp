
#include "HookImpactPoint.h"
#include "Components/BoxComponent.h"

void AHookImpactPoint::ContactWithCable(FVector& Direction)
{
	OnHookImpactDynamic.Broadcast(DegreesOfRotationForParent, SpeedOfRotationForParent);
}

// Sets default values
AHookImpactPoint::AHookImpactPoint()
{
	RootComponent		= CreateDefaultSubobject<USceneComponent>("Root");
	ImpactCollisionBox	= CreateDefaultSubobject<UBoxComponent>("ImpactCollisionBox");
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");

	ImpactCollisionBox->SetupAttachment(RootComponent);
	StaticMeshComponent->SetupAttachment(RootComponent);
}