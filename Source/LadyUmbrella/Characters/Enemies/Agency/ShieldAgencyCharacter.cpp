// Fill out your copyright notice in the Description page of Project Settings.


#include "ShieldAgencyCharacter.h"

#include "Components/CapsuleComponent.h"
#include "LadyUmbrella/AI/Controllers/AIControllerShieldAgency.h"
#include "LadyUmbrella/Weapons/AgencyShield/AgencyShield.h"


AShieldAgencyCharacter::AShieldAgencyCharacter()
{
	WeaponType = EWeaponType::Shield;
	SpeedAproachPlayer = 300.f;
	SpeedOnBeingAttacked = 100.f;
	SpeedOnFindPlayer = 100.f;
}

void AShieldAgencyCharacter::HandleShieldDestroyed()
{
	WeaponType = EWeaponType::Pistol;
	//Notify AI Controller
	if (AAIControllerShieldAgency* AIController = Cast<AAIControllerShieldAgency>(GetAIController()))
	{
		AIController->NotifyShieldIsBroken();
	}
}

AActor* AShieldAgencyCharacter::GetShield() const
{
	if (IsValid(Shield))
	{
		return Shield;
	}
	return nullptr;
}

void AShieldAgencyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	Shield = GetWorld()->SpawnActor<AAgencyShield>(ShieldSubclass);
	if (IsValid(Shield))
	{
		if (USkeletalMeshComponent* Comp = FindComponentByClass<USkeletalMeshComponent>())
		{
			Shield->AttachToComponent(
				Comp,
				FAttachmentTransformRules(
					EAttachmentRule::KeepRelative,
					EAttachmentRule::KeepRelative,
					EAttachmentRule::KeepRelative,
					true),
				"EnemyShieldSocket"
				);
		}
		//Listen when shield is destroyed
		Shield->OnShieldDestroyedDelegate.BindUFunction(this,FName("HandleShieldDestroyed"));
		GetCapsuleComponent()->IgnoreActorWhenMoving(Shield, true);
		//Shield->OnDestroyed.AddDynamic(this, &AShieldAgencyCharacter::HandleShieldDestroyed);
	}	
}