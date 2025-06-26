// 
// MeleeComponent.h
// 
// Implementation of the MeleeComponent class. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#include "MeleeComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "LadyUmbrella/Characters/GenericCharacter.h"
#include "Particles/ParticleSystem.h"

UMeleeComponent::UMeleeComponent()
{
	static const wchar_t* ImpactParticleAsset = TEXT("/Game/StarterContent/Particles/P_Explosion.P_Explosion");
	
	bCanDetectMelee = true;
	Damage = 30.0f;
	LaunchFactor = 200.0f;
	LaunchAddedVelocity = FVector(0, 0, 100.0f);
	TraceRadius = 30.0f;

	ImpactParticleSystem = Cast<UParticleSystem>(StaticLoadObject(UParticleSystem::StaticClass(), nullptr, ImpactParticleAsset));
}

void UMeleeComponent::ProcessCollisionMelee(const FVector& TraceStart, const FVector& ActorRightVector)
{
	if (bCanDetectMelee)
	{
		FHitResult HitResult;

		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
		TArray<AActor*> IgnoredActors;
		IgnoredActors.Add(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

		bool bHit = UKismetSystemLibrary::SphereTraceSingleForObjects(
			this,
			TraceStart,
			TraceStart,
			TraceRadius,
			ObjectTypes,
			false,
			IgnoredActors,
			EDrawDebugTrace::None,
			HitResult,
			true
		);

		if (bHit)
		{
			AActor* HitActor = HitResult.GetActor();

			if (HitActor->IsA(AGenericCharacter::StaticClass()))
			{
				AGenericCharacter* Character = Cast<AGenericCharacter>(HitActor);
				Character->SubtractHealth(Damage,EWeaponType::Melee);
				Character->LaunchCharacter(ActorRightVector * LaunchFactor + LaunchAddedVelocity, false, false);
			}
			else if (IHealthInterface* HealthInterface = Cast<IHealthInterface>(HitActor))
			{
				HealthInterface->SubtractHealth(Damage,EWeaponType::Melee);
			}
			
			bCanDetectMelee = false;
			
			if (IsValid(ImpactParticleSystem.Get()))
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticleSystem.Get(), HitResult.ImpactPoint, FRotator::ZeroRotator, true);
			}
		}
	}
}

void UMeleeComponent::SetCanDetectMelee(const bool Value)
{
	bCanDetectMelee = Value;
}
