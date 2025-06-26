// 
// Grenade.cpp
// 
// Implementation of the Grenade class.
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#include "Grenade.h"

#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "Sound/SoundCue.h" 
#include "LadyUmbrella/Weapons/Umbrella/Umbrella.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Environment/CoverSystem/CoverParent.h"
#include "LadyUmbrella/Util/Logger.h"

#define GRENADE_STATIC_MESH_ASSET TEXT("/Script/Engine.StaticMesh'/Game/Blueprints/LevelElements/PlaceholderMeshes/Primitives/SM_Sphere.SM_Sphere'")
#define GRENADE_MATERIAL_ASSET TEXT("/Script/Engine.MaterialInstanceConstant'/Game/Materials/Instancias/Opaque/MI_PlaceholderWhite.MI_PlaceholderWhite'")
#define GRENADE_EXPLOSION_ASSET TEXT("/Script/Niagara.NiagaraSystem'/Game/Particles/NiagaraSystems/NS_GrenadeExplosion.NS_GrenadeExplosion'")
#define GRENADE_EXPLOSION_SOUND TEXT("/Script/Engine.SoundWave'/Game/Sounds/SoundWaves/Explosions/SW_Explode02.SW_Explode02'")

AGrenade::AGrenade()
{
	SphereComponent  = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollisionComponent"));
	SphereComponent->InitSphereRadius(10.0f);
	// SphereComponent->SetSimulatePhysics(true);
	// SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SphereComponent->SetCollisionResponseToAllChannels(ECR_Block);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(SphereComponent);
	StaticMeshComponent->SetStaticMesh(ConstructorHelpers::FObjectFinder<UStaticMesh>(GRENADE_STATIC_MESH_ASSET).Object);
	StaticMeshComponent->SetMaterial(0, ConstructorHelpers::FObjectFinder<UMaterial>(GRENADE_MATERIAL_ASSET).Object);
	StaticMeshComponent->SetWorldScale3D(FVector(0.1f, 0.1f, 0.1f));

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bShouldBounce = true;
	ProjectileMovementComponent->Bounciness = 0.25f;

	NS_Explosion		= LoadObject<UNiagaraSystem>(nullptr, GRENADE_EXPLOSION_ASSET, nullptr, LOAD_None, nullptr);
	ExplosionRange		= 500.0f;
	MaxDamage           = 100.0f;
	MinDamage           = 0.0f;
	GrenadeRestitution	= 0.75f;
	DamagedActors       = TArray<AActor*>();
	
	ExplosionAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Explosion Sound Component"));
	ExplosionAudioComponent->SetupAttachment(RootComponent);
	//ExplosionAudioComponent->bAutoActivate = false;
	//ExplosionAudioComponent->SetAutoActivate(false);

	ConstructorHelpers::FObjectFinder<USoundCue> ExplosionCueFinder(TEXT("/Script/Engine.SoundCue'/Game/Sounds/SoundCues/SC_GrenadeExplosion.SC_GrenadeExplosion'"));
	if (ExplosionCueFinder.Succeeded())
	{
		ExplosionAudioComponent->SetSound(ExplosionCueFinder.Object);
	}

	const ConstructorHelpers::FObjectFinder<UCurveFloat> ExplosionDamageFalloff(TEXT("/Script/Engine.CurveFloat'/Game/DataStructures/Curves/Combat/C_GrenadeDamageFalloff.C_GrenadeDamageFalloff'"));
	if (ExplosionDamageFalloff.Succeeded())
	{
		DamageFalloff = ExplosionDamageFalloff.Object;
	}
}

void
AGrenade::BeginPlay()
{
	Super::BeginPlay();
	ExplosionAudioComponent->Stop();
	SetLifeSpan(5);

	SphereComponent->OnComponentHit.AddDynamic(this, &AGrenade::OnHit);
}

void
AGrenade::Throw(const FVector Impulse)
{
	ProjectileMovementComponent->Velocity = Impulse;
	
	GetWorldTimerManager().SetTimer(
		UnusedHandle,
		this,
		 &AGrenade::Explode, 
		3.f,
		true,
		3.f
	);
}

void
AGrenade::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse,
	const FHitResult& Hit)
{
	if (OtherActor->IsA(AUmbrella::StaticClass()))
	{
		ProjectileMovementComponent->Velocity = ProjectileMovementComponent->Velocity * -1;
	}
	else
	{
		Cast<AGrenade>(HitComponent->GetOwner())->GetProjectileMovementComponent()->Velocity *= GrenadeRestitution;
	}
}

void
AGrenade::Explode()
{
	GetWorld()->GetTimerManager().ClearTimer(UnusedHandle);
	
	if (NS_Explosion)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NS_Explosion, GetActorLocation());
	}

	if (ExplosionAudioComponent)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionAudioComponent->Sound, GetActorLocation());
	}
	
	TArray<AActor*> ActorsToIgnore;
	TArray<FHitResult> HitResults;
	ActorsToIgnore.Add(this);
	
	UKismetSystemLibrary::SphereTraceMulti
	(
		GetWorld(),
		GetActorLocation(),
		GetActorLocation(),
		ExplosionRange,
		UEngineTypes::ConvertToTraceType(ECC_Visibility),
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForDuration,
		HitResults,
		true,
		FLinearColor::White,
		FLinearColor::Red,
		2.f
	);
	
	for (FHitResult Result : HitResults)
	{
		AActor* HitActor = Result.GetActor();
		
		if (!IsValid(HitActor))
			continue;
		
		if (ShouldDamageActor(HitActor))
		{
			IHealthInterface* HealthInterface = Cast<IHealthInterface>(HitActor);
			
			if (HealthInterface)
			{
				const float DamageCurveValue = DamageFalloff->GetFloatValue(GetDistanceTo(HitActor) / ExplosionRange);
				
				HealthInterface->SubtractHealth(FMath::Clamp(DamageCurveValue * MaxDamage, MinDamage, MaxDamage), EWeaponType::Grenade);
			}
		}

		DamagedActors.Add(HitActor);
	}

	Destroy();
}

bool
AGrenade::ShouldDamageActor(const AActor* HitActor) const
{
	if (DamagedActors.Contains(HitActor))
	{
		return false;
	}
	
	if (HitActor->IsA(APlayerCharacter::StaticClass()))
	{
		return ShouldDamagePlayer(Cast<APlayerCharacter>(HitActor));
	} 
	
	if (HitActor->IsA(AGenericCharacter::StaticClass()))
	{
		return !Cast<AGenericCharacter>(HitActor)->IsMovementControlledByCover();
	}
	
	return true;
}

bool AGrenade::ShouldDamagePlayer(const APlayerCharacter* PlayerCharacter) const
{
	FHitResult Obstacle;

	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(this);

	GetWorld()->LineTraceSingleByChannel(
		Obstacle,
		GetActorLocation(),
		PlayerCharacter->GetActorLocation(),
		ECC_GameTraceChannel1,
		CollisionQueryParams
	);
	
	const AActor* ObstacleActor = Obstacle.GetActor();

	if (IsValid(ObstacleActor))
	{
		if (PlayerCharacter->IsMovementControlledByCover())
		{
			return !ObstacleActor->IsA(ACoverParent::StaticClass());
		}
		
		if (ObstacleActor->IsA(AUmbrella::StaticClass()))
		{
			return !Cast<AUmbrella>(ObstacleActor)->IsOpen();
		}
	}

	return true;
}
