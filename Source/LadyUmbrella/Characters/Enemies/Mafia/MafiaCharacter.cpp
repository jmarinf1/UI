// 
// MafiaCharacter.cpp
// 
// Implementation of the Mafia character class
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#include "MafiaCharacter.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Weapons/Grenade.h"

#define ANIM_CLASS_ASSET TEXT("/Script/Engine.AnimBlueprint'/Game/Animations/AnimationBlueprints/Enemies/ABP_Mafia.ABP_Mafia'")
#define SKELETAL_MESH_ASSET TEXT("/Script/Engine.SkeletalMesh'/Game/Meshes/SkeletalMeshes/SK_BasicMesh.SK_BasicMesh'") 

FOnGrenadeTokenReleased AMafiaCharacter::OnGrenadeTokenReleased;

void AMafiaCharacter::OnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{	
	if (NotifyName.IsEqual("ThrowGrenade"))
	{
		Grenade->Destroy();

		FVector GrenadeVelocity;
		//FVector SpawnLocation = GetMesh()->GetSocketLocation("GrenadeSocket");
		
		APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

		GrenadeEndPosition = Player->GetActorLocation();
		GrenadeEndPosition.Z += GrenadeEndLocationZOffset;
	
		GrenadeEndPosition += (GetActorLocation() - UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation()).GetSafeNormal() * GrenadeEndLocationForwardOffset;

		if (Player->IsInTallCover())
		{
			GrenadeEndLocationLateralOffset = FMath::RandRange(GrenadeMinLateralOffset, GrenadeEndLocationLateralOffset);

			if (FMath::RandBool())
			{
				GrenadeEndLocationLateralOffset *= -1;
			}
		}
		else
		{
			GrenadeEndLocationLateralOffset = 1.0f;
		}

		GrenadeEndPosition += (GetActorLocation() - Player->GetActorLocation()).GetSafeNormal().Cross(GetActorUpVector()) * GrenadeEndLocationLateralOffset;
	
		UGameplayStatics::SuggestProjectileVelocity_CustomArc(
			GetWorld(),
			GrenadeVelocity,
			Grenade->GetActorLocation(),
			//SpawnLocation,
			GrenadeEndPosition,
			0,
			GrenadeArc
		);

		FTransform GrenadeSpawnTransform;
		GrenadeSpawnTransform.SetLocation(Grenade->GetActorLocation());
		GrenadeSpawnTransform.SetRotation(FQuat(FRotator::ZeroRotator));
		GrenadeSpawnTransform.SetScale3D(FVector(1, 1, 1));

		if (GrenadeBP != nullptr)
		{
			Grenade = GetWorld()->SpawnActor<AGrenade>(GrenadeBP, GrenadeSpawnTransform);
			Grenade->SetActorEnableCollision(false);
			Grenade->Throw(GrenadeVelocity);
			GetWorld()->GetTimerManager().SetTimer(ResetCollisionTimerHandler, this, &AMafiaCharacter::ResetCollision, .05f, false);
		}
	}
}

void AMafiaCharacter::ResetCollision()
{
	if (IsValid(Grenade))
	{
		Grenade->SetActorEnableCollision(true);
		GetWorld()->GetTimerManager().ClearTimer(ResetCollisionTimerHandler);
	}
}

AMafiaCharacter::AMafiaCharacter()
	: bHasGrenadeToken(false)
	, GrenadeEndLocationZOffset(-70)
	, GrenadeEndLocationForwardOffset(200)
	, GrenadeEndLocationLateralOffset(200)
	, GrenadeMinLateralOffset(100)
	, GrenadeArc(0.75)
{
	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -80.0f));
	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetSkeletalMesh(ConstructorHelpers::FObjectFinder<USkeletalMesh>(SKELETAL_MESH_ASSET).Object);
	GetMesh()->SetAnimInstanceClass(ConstructorHelpers::FObjectFinder<UClass>(ANIM_CLASS_ASSET).Object);
	//GetMesh()->SetAnimInstanceClass(ConstructorHelpers::FObjectFinder<UClass>(ANIM_CLASS_ASSET).Object);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Block); // for the hook
	
	TSoftObjectPtr<UAnimBlueprintGeneratedClass> AnimBlueprint = TSoftObjectPtr<UAnimBlueprintGeneratedClass>(
	FSoftObjectPath(TEXT("/Game/StarterContent/Characters/Mannequins/Animations/ABP_Manny.ABP_Manny_C")));

	if (AnimBlueprint.IsValid()) 
	{
		GetMesh()->SetAnimInstanceClass(AnimBlueprint.Get());
	} 
	else 
	{
		AnimBlueprint.LoadSynchronous();
		if (AnimBlueprint.IsValid()) 
		{
			GetMesh()->SetAnimInstanceClass(AnimBlueprint.Get());
		}
	}
	
	WeaponType = EWeaponType::Rifle;
	SpeedOnPatroling = 300.f;
	SpeedOnFleeing = 450.f;
	SpeedOnCloseCombat = 200.f;
	SpeedOnEnterArena = 400.f;
	SpeedOnMoveToCover = 200.f;
}

void AMafiaCharacter::OnThrowGrenadeEnd(UAnimMontage* Montage, bool bInterrupted)
{
	SetHasGrenadeToken(false);
	OnGrenadeTokenReleased.Broadcast();
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (IsValid(Montage) && Montage == ThrowGrenadeMontage)
	{
		AnimInstance->OnMontageEnded.RemoveDynamic(this, &AMafiaCharacter::OnThrowGrenadeEnd);
		AnimInstance->OnPlayMontageNotifyBegin.RemoveDynamic(this, &AMafiaCharacter::OnMontageNotifyBegin);
	}
}

void
AMafiaCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void
AMafiaCharacter::ThrowGrenade()
{
	SetActorRotation(UKismetMathLibrary::FindLookAtRotation(
		GetTransform().GetLocation(),
		UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetTransform().GetLocation()
	));
	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (IsValid(AnimInstance) && IsValid(ThrowGrenadeMontage))
	{
		FVector SpawnLocation = GetMesh()->GetSocketLocation("GrenadeSocket");
		FTransform GrenadeSpawnTransform;
		GrenadeSpawnTransform.SetLocation(SpawnLocation);
		GrenadeSpawnTransform.SetRotation(FQuat(FRotator::ZeroRotator));
		GrenadeSpawnTransform.SetScale3D(FVector(1, 1, 1));

		if (GrenadeBP != nullptr)
		{
			Grenade = GetWorld()->SpawnActor<AGrenade>(GrenadeBP, GrenadeSpawnTransform);
			if (IsValid(Grenade))
			{
				//Grenade->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				Grenade->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "GrenadeSocket");
			}
		}
		
		AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &AMafiaCharacter::OnMontageNotifyBegin);
		AnimInstance->OnMontageEnded.AddDynamic(this, &AMafiaCharacter::OnThrowGrenadeEnd);
		AnimInstance->Montage_Play(ThrowGrenadeMontage, 1);
	}
	
}

void AMafiaCharacter::SetHasGrenadeToken(bool hasGrenadeToken)
{
	bHasGrenadeToken = hasGrenadeToken;
}

bool AMafiaCharacter::GetHasGrenadeToken() const
{
	return bHasGrenadeToken;
}
