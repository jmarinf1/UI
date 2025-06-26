#include "LaserElement.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

ALaserElement::ALaserElement()
	: CurrentState(ELaserState::Deactivated)
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	LaserBaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LaserBaseMesh"));
	LaserBaseMesh->SetupAttachment(RootComponent);
	LaserBaseMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LaserBaseMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	LaserBaseMesh->SetRelativeLocation(FVector::ZeroVector);
	LaserBaseMesh->SetRelativeRotation(FRotator(90.0f, 90.0f, 90.0f));

	DetectionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("DetectionBox"));
	DetectionBox->SetupAttachment(LaserBaseMesh);
	DetectionBox->OnComponentBeginOverlap.AddDynamic(this, &ALaserElement::OnPlayerDetected);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Game/Blueprints/LevelElements/PlaceholderMeshes/Primitives/SM_Cylinder.SM_Cylinder"));
	if (CubeMesh.Succeeded())
	{
		LaserBaseMesh->SetStaticMesh(CubeMesh.Object);
		LaserBaseMesh->SetWorldScale3D(FVector(0.5f));
	}

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> LaserNiagaraSystemFinder(TEXT("/Game/Particles/NiagaraSystems/NS_Beam.NS_Beam"));
	if (LaserNiagaraSystemFinder.Succeeded())
	{
		LaserNiagaraSystem = LaserNiagaraSystemFinder.Object;
	}
	LaserDamage = 1.0f;
	LaserRange = 10000.0f;
	BoxLocation = FVector(0.0f, 0.0f, -500.0f);
	BoxExtent = FVector(100.0f, 100.0f, 500.0f);
}

void ALaserElement::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (DetectionBox)
	{
		DetectionBox->SetRelativeLocation(BoxLocation);
		DetectionBox->SetBoxExtent(BoxExtent);
	}
}

void ALaserElement::BeginPlay()
{
	Super::BeginPlay();
	// Design Wanted a constantly firing laser, so ActivateLaser on begin play
	ActivateLaser();
}

void ALaserElement::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentState == ELaserState::Activated)
	{
		FireLaser();
	}
}

void ALaserElement::OnPlayerDetected(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Design Wanted a constantly firing laser, so logic for the detection can be ignored
	// Not deleted in case they want the detection back
	//if (OtherActor && OtherActor->IsA<APlayerCharacter>())
	//{
		//if (CurrentState == ELaserState::Deactivated)
		//{
		//	ActivateLaser();
		//}
	//}
}

void ALaserElement::ActivateLaser()
{
	CurrentState = ELaserState::Activated;
	// Design Wanted a constantly firing laser, so logic for the detection can be ignored
	// Not deleted in case they want the detection back
	//GetWorld()->GetTimerManager().SetTimer(DeactivateTimerHandle, this, &ALaserElement::DeactivateLaser, 2.0f, false);
}

void ALaserElement::DeactivateLaser()
{
	CurrentState = ELaserState::Deactivated;
}

void ALaserElement::FireLaser() const
{

	FVector Start = GetActorLocation();
	FRotator Rotation = GetActorRotation();
	FVector End = Start + (Rotation.Vector() * LaserRange);

	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Camera, CollisionParams);

	FColor TraceColor = bHit ? FColor::Red : FColor::Green;

	if (bHit)
	{
		End = HitResult.ImpactPoint;
	}
	FVector ParticlePoint = End;
	if (bHit)
	{
		if (APlayerCharacter* HitPlayer = Cast<APlayerCharacter>(HitResult.GetActor()))
		{
			if (IHealthInterface* HealthInterface = Cast<IHealthInterface>(HitPlayer))
			{
				ParticlePoint = HitResult.ImpactPoint;
				HealthInterface->SubtractHealth(LaserDamage,EWeaponType::Laser);
			}
		}
	}


	if (LaserNiagaraSystem)
	{
		UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			LaserNiagaraSystem,
			Start,
			FRotator::ZeroRotator,
			FVector(1.0f),
			true
		);
		if (NiagaraComponent)
		{
			NiagaraComponent->SetVariableVec3(FName("beam end"), ParticlePoint);
		}
	}
}
