#include "ElectricElement.h"
#include "Particles/ParticleSystemComponent.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

AElectricElement::AElectricElement()
{
	PrimaryActorTick.bCanEverTick = true;
	FElecTime = 10.f;
}

void AElectricElement::BeginPlay()
{
	Super::BeginPlay();
}

void AElectricElement::SetElectrified(bool bNewElectrified)
{
	if (bElectrified == bNewElectrified)
		return;

	bElectrified = bNewElectrified;

	if (bElectrified)
	{
		// Run Blueprint Event
		OnElectrified();
		// Start spark effect
		if (SparksEffect)
		{
			SparksComponent = UGameplayStatics::SpawnEmitterAttached(
				SparksEffect,
				RootComponent,
				NAME_None,
				FVector::ZeroVector,
				FRotator::ZeroRotator,
				EAttachLocation::KeepRelativeOffset,
				true
			);
		}
		for (AElectricElement* Connected : ConnectedElectricElements)
		{
			if (Connected && !Connected->bElectrified)
			{
				Connected->SetElectrified(true);
			}
		}

		// Set timer to reset
		GetWorldTimerManager().SetTimer(ElectrifiedTimerHandle, this, &AElectricElement::ResetElectrifiedState, FElecTime, false);
	}
	else
	{
		ResetElectrifiedState();
	}
}

void AElectricElement::ResetElectrifiedState()
{
	bElectrified = false;

	// Clean up effects
	if (SparksComponent)
	{
		SparksComponent->DeactivateSystem();
		SparksComponent->DestroyComponent();
		SparksComponent = nullptr;
	}
}
