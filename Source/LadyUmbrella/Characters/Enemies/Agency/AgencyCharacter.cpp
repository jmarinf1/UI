// Fill out your copyright notice in the Description page of Project Settings.


#include "AgencyCharacter.h"
#include "Components/CapsuleComponent.h"

// void AAgencyCharacter::HandleShieldDestroyed(AActor* actor)
// {
// 	OnShieldDestroyed.Broadcast();
// 	bHasShield = false;
// }

// AActor* AAgencyCharacter::GetShield() const
// {
// 	if (IsValid(Shield))
// 	{
// 		return Shield;
// 	}
// 	return nullptr;
// }

FOnElectricBulletTokenRequest AAgencyCharacter::OnElectricBulletTokenRequest;

AAgencyCharacter::AAgencyCharacter()
{
	WeaponType = EWeaponType::Pistol;
	MinNumRafagues = 1;
	MaxNumRafagues = 2;
	SetRandomNumRafagues();
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Block); // Hook
	SpeedOnMoveBetweenCovers = 450.f;
	SpeedOnCloseCombat = 200.f;
	SpeedOnMoveAroundCover = 350.f;
	bHasElectricBullet = false;
}

void AAgencyCharacter::SetRandomNumRafagues()
{
	NumRafaguesRemaining = FMath::RandRange(MinNumRafagues,MaxNumRafagues);
}

// void AAgencyCharacter::BeginPlay()
// {
// 	Super::BeginPlay();
//
// 	if (bHasShield)
// 	{
// 		Shield = GetWorld()->SpawnActor<AAgencyShield>(ShieldSubclass);
// 		if (IsValid(Shield))
// 		{
// 			if (USkeletalMeshComponent* Comp = FindComponentByClass<USkeletalMeshComponent>())
// 			{
// 				Shield->AttachToComponent(
// 					Comp,
// 					FAttachmentTransformRules(
// 						EAttachmentRule::KeepRelative,
// 						EAttachmentRule::KeepRelative,
// 						EAttachmentRule::KeepRelative,
// 						true),
// 					"EnemyShieldSocket"
// 					);
// 			}
// 			Shield->OnDestroyed.AddDynamic(this, &AAgencyCharacter::HandleShieldDestroyed);
// 		}	
// 	}
// }
