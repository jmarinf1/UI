// 
// GenericWeapon.cpp
// 
// Implementation of the GenericWeapon class. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#include "GenericWeapon.h"

#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"

AGenericWeapon::AGenericWeapon()
{
	static const wchar_t* BulletNiagaraAsset = TEXT("/Script/Niagara.NiagaraSystem'/Game/Particles/NiagaraSystems/NS_BulletTrace.NS_BulletTrace'");
	static const wchar_t* FireSoundAsset     = TEXT("/Script/Engine.SoundCue'/Game/StarterContent/Audio/Explosion_Cue.Explosion_Cue'");
	//static const wchar_t* SkeletalMeshAsset  = TEXT("/Script/Engine.SkeletalMesh'/Game/Weapons/PlaceHolder/Rifle.Rifle'");

	AmmoCurrent    = 20;
	AmmoCurrentMax = 20;
	AmmoReserve    = 20;
	AmmoReserveMax = 20;
	WeaponCadence  = 0.4f;
	MaxWeaponCadence  = 1.f;
	MinWeaponCadence  = 0.2f;
	WeaponDamage   = 20.f;
	WeaponRange    = 1000;
	WeaponBulletType = EBulletType::Normal;
	bIsElectricDardActivated = false;
	bIsElectricDardShooted = false;
	ElectricBulletCooldown = 2.f;
	LifeSpan = 10.f;
	MassKg = 4.f;

	BulletNiagaraSystem = ConstructorHelpers::FObjectFinder<UNiagaraSystem>(BulletNiagaraAsset).Object;
	FireSoundBase = ConstructorHelpers::FObjectFinder<USoundBase>(FireSoundAsset).Object;

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(FName(TEXT("Capsule")));
	Capsule->SetCapsuleHalfHeight(20.f);
	Capsule->SetCapsuleRadius(15.f);
	SetRootComponent(Capsule);
	
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMeshComponent->SetupAttachment(GetRootComponent());
}

void AGenericWeapon::SimulatePhysicsBeforeDestroy()
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	
	SkeletalMeshComponent->SetMassOverrideInKg(NAME_None, MassKg, true);
	Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Capsule->SetCollisionResponseToAllChannels(ECR_Ignore);
	
	SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SkeletalMeshComponent->SetCollisionObjectType(ECC_PhysicsBody);
	SkeletalMeshComponent->SetCollisionResponseToAllChannels(ECR_Block);
	
	SkeletalMeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	
	SkeletalMeshComponent->SetAllBodiesSimulatePhysics(true);
	SkeletalMeshComponent->WakeAllRigidBodies();
	SkeletalMeshComponent->SetSimulatePhysics(true);
	SkeletalMeshComponent->bBlendPhysics = true;
	
	SetLifeSpan(LifeSpan);
}

void AGenericWeapon::SpawnElectricEmmiter()
{
	FVector WeaponTraceStart = SkeletalMeshComponent->GetSocketLocation(TEXT("MuzzleSocket"));
	FRotator WeaponRotation = SkeletalMeshComponent->GetSocketRotation(TEXT("MuzzleSocket"));
	//UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),ElectricEffect,WeaponTraceStart,WeaponRotation);
	//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),ElectricEffect,WeaponTraceStart,WeaponRotation);

	if (ElectricEffect)
	{
		ElectricEffectComponent = UGameplayStatics::SpawnEmitterAttached(
			ElectricEffect,
			RootComponent,
			"MuzzleSocket",
			WeaponTraceStart,
			WeaponRotation,
			EAttachLocation::Type::KeepWorldPosition,
			true
		);
	}
}

void AGenericWeapon::StopElectricEmmiter()
{
	if (ElectricEffectComponent)
	{
		ElectricEffectComponent->DeactivateSystem();
		ElectricEffectComponent->DestroyComponent();
		ElectricEffectComponent = nullptr;
	}
}

void AGenericWeapon::BeginPlay()
{
	Super::BeginPlay();
	//SkeletalMeshComponent->SetSkeletalMesh(SkeletalMesh);
}

void AGenericWeapon::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AGenericWeapon::Fire()
{
	if (AmmoCurrent <= 0)
		return false;

	AmmoCurrent--;

	if (IsValid(FireSoundBase))
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSoundBase, GetActorLocation());
	}
	
	return true;
}

bool AGenericWeapon::Reload(bool ConsumeAmmo)
{
	if (!CanReload())
		return false;

	// Calculates amount to load into the gun by using the formula:
	// 
	//     AmountToLoad = AmmoCurrentMax - AmmoCurrent
	// 
	// However, if the amount to load exceeds the AmmoReserve, then only load as
	// many bullets as the weapon has left.
	// 
	// For example:
	//     - If the weapon is currently at 10, and the max is 32, then the weapon
	//       needs to load 22 bullets, but the weapon only has 10 bullet reserve
	//       remaining. In this case the amount to load gets clamped to 10, and
	//       updates the current bullet amount to 20.
	const int Amount = AmmoCurrentMax - AmmoCurrent;
	const int AmountClamped = FMath::Clamp(Amount, 0, AmmoReserve);

	AmmoCurrent += AmountClamped;

	if (ConsumeAmmo)
	{
		AmmoReserve -= AmountClamped;
	}
	
	return true;
}

bool AGenericWeapon::CanReload() const
{
	const bool HasReserve = AmmoReserve > 0;
	const bool IsAtCurrentMax = AmmoCurrent >= AmmoCurrentMax;
	
	return HasReserve && !IsAtCurrentMax;
}

float AGenericWeapon::GetRandomWeaponCadence()
{
	WeaponCadence = FMath::RandRange(MinWeaponCadence, MaxWeaponCadence);
	return WeaponCadence;
}
