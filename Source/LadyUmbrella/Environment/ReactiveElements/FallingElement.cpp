// Fill out your copyright notice in the Description page of Project Settings.


#include "FallingElement.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"

AFallingElement::AFallingElement()
{
	Super::BoxRoot->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
	Super::BoxRoot->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("Skeletal Mesh Component");
	bHasToBeDestroyed = false;
}

void AFallingElement::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (Super::Reacted && SimulationSeconds > 0.f) 
	{
		SimulationSeconds -= DeltaTime;
		if(SimulationSeconds > 0.f)
		{
			CurrentSpeed += -(GravityForce * DeltaTime);
			ObjectMesh->SetWorldLocation(Super::ObjectMesh->GetComponentLocation() + FVector(0.f, 0.f, CurrentSpeed * DeltaTime), false, nullptr, ETeleportType::None);
		}
	}
	if (Super::Reacted && SimulationSeconds <= 0.f)
	{
		LingeringSeconds -= DeltaTime;
		if (LingeringSeconds <= 0.f) 
		{
			if (bHasToBeDestroyed)
			{
				Destroy();
			}
		}
	}
}

void AFallingElement::Reaction(HitSource Source)
{
	// I leave this line here for the future when we have to swap to animations
	// SkeletalMeshComponent->PlayAnimation(SkeletalMeshComponent->AnimationData.AnimToPlay, false);
	
	if(Super::ShouldReact(Source))
	{
		FHitResult Hit(ForceInit);
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredComponent(Super::BoxRoot);
		QueryParams.AddIgnoredComponent(Super::ObjectMesh);
		if(GetWorld()->LineTraceSingleByChannel(Hit, Super::ObjectMesh->GetComponentLocation(), Super::ObjectMesh->GetComponentLocation() + FVector(0, 0, -10000.f), ECC_GameTraceChannel1/*ECC_WorldStatic*/, QueryParams)) {
			
			FallingDistance = Hit.Distance;
			GravityForce = 2.f*Hit.Distance/FMath::Pow(SimulationSeconds,2); // Derived from free fall formula -> h = (gt^2)/2
			CurrentSpeed = 0.f;
		}
	}
}

