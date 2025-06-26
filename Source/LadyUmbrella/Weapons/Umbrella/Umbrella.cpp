// 
// Umbrella.h
// 
// Implementation of the Umbrella class. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#include "Umbrella.h"

#include "CableComponent.h"
#include "InterchangeResult.h"
#include "NiagaraComponent.h"
#include "UpgradeComponent/UpgradeComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/TimelineComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "LadyUmbrella/Characters/Enemies/EnemyCharacter.h"
#include "LadyUmbrella/Interfaces/HitReactiveInterface.h"
#include "LadyUmbrella/Interfaces/HookInterface.h"
#include "LadyUmbrella/Weapons/GenericProjectile.h"
#include "LadyUmbrella/Weapons/Umbrella/Gadgets/ElectricProjectile.h"
#include "LadyUmbrella/UI/Basics/NumericProgressBar.h"
#include "LadyUmbrella/Util/TransformUtils.h"
#include "MeleeComponent/MeleeComponent.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

#pragma region BASIC FUNCTIONS

AUmbrella::AUmbrella()
{
	static const wchar_t* CloseAnimationAsset = TEXT("/Script/Engine.AnimSequence'/Game/Animations/AnimationSequences/AS_UmbrellaClosed.AS_UmbrellaClosed'");
	static const wchar_t* OpenAnimationAsset  = TEXT("/Script/Engine.AnimSequence'/Game/Animations/AnimationSequences/AS_UmbrellaOpen.AS_UmbrellaOpen'");
	static const wchar_t* SkeletalMeshAsset   = TEXT("/Script/Engine.SkeletalMesh'/Game/Meshes/SkeletalMeshes/SK_Umbrella.SK_Umbrella'");
	
	PrimaryActorTick.bCanEverTick = true;

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	SkeletalMeshComponent->SetSkeletalMesh(ConstructorHelpers::FObjectFinder<USkeletalMesh>(SkeletalMeshAsset).Object);
 
	CloseAnimSequence = ConstructorHelpers::FObjectFinder<UAnimSequence>(CloseAnimationAsset).Object;
	OpenAnimSequence = ConstructorHelpers::FObjectFinder<UAnimSequence>(OpenAnimationAsset).Object;
	
	ProtectionCollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("ProtectionCollisionComponent"));
	ProtectionCollisionComponent->SetupAttachment(SkeletalMeshComponent);
	ProtectionCollisionComponent->SetRelativeTransform(TransformUtils::TransformLocation(0.0f, 127.0f, 0.0f));
	ProtectionCollisionComponent->SetBoxExtent(FVector(191, 17, 170));
	ProtectionCollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ProtectionCollisionComponent->SetNotifyRigidBodyCollision(true);
	ProtectionCollisionComponent->SetHiddenInGame(true);
	
	bOpen    	= true;
	bOnEnemy    = true;
	bIsShooting = false;
	bCanShootElectric = true;
	bInfiniteAmmo = false;
	bCanShootGrapple = true;
	bDrawDebugBulletDispersion = false;

	AmmoCurrent        = 8;
	AmmoCurrentMax     = 8;
	AmmoReserve        = 32;
	AmmoReserveMax     = 32;
	BulletDamage       = 2.0f;
	NotAimingBulletDispersion = 500;
	BulletRange        = 4000.0f;
	ReloadTime		   = 0.5f;
	ShootingCadenceTime = 0.5f;
	MaxNrBulletPerShot = 6;
	BulletPatternSize  = 3.0f;
	VolumeMultiplier   = 1.0f;
	WidgetTraceRadius  = 15.0f;
	HookRange          = 1000;
	GadgetType		   = EGadgetType::Grapple;
	bCanShootGrapple   = true;
	bIsGrapplingCharacter = false;
	
	CableComponent = CreateDefaultSubobject<UCableComponent>(TEXT("CableComponent"));
	CableComponent->SetupAttachment(SkeletalMeshComponent);
	CableComponent->SetVisibility(false);
	CableComponent->SetComponentTickEnabled(false);
	CableComponent->SetRelativeTransform(TransformUtils::TransformLocation(0.0f, 130.0f, 1.0f));
	CableComponent->EndLocation = FVector::ZeroVector;
	
	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	Arrow->SetupAttachment(SkeletalMeshComponent);
	Arrow->SetRelativeLocation(FVector(0.0f,145.0f,0.0f));
	Arrow->SetRelativeRotation(FRotator(0,90.0f,90.0f));

	EnergyBarComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Energy Bar"));

	if (SkeletalMeshComponent)
	{
		GetEnergyBarComponent()->SetupAttachment(SkeletalMeshComponent);
		GetEnergyBarComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	static ConstructorHelpers::FClassFinder<AElectricProjectile> ProjectileBPClass(TEXT("/Script/Engine.Blueprint'/Game/Blueprints/Spawnables/Projectiles/BP_ElectricProjectile.BP_ElectricProjectile_C'"));

	if (ProjectileBPClass.Succeeded())
	{
		ElectricProjectileClass = ProjectileBPClass.Class;
	}
	
	static ConstructorHelpers::FObjectFinder<UCurveFloat> CurveAsset(TEXT("/Script/Engine.CurveFloat'/Game/DataStructures/Curves/Combat/C_DistanceDamageModifier.C_DistanceDamageModifier'"));

	if (CurveAsset.Succeeded())
	{
		DistanceDamageModifier = CurveAsset.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterial> CMat(TEXT("/Script/Engine.Material'/Game/Materials/RegularMaterials/Placeholder/M_Black.M_Black'"));

	if (IsValid(CMat.Object))
	{
		MaterialClosed = CMat.Object;
		SkeletalMeshComponent->SetMaterial(0, CMat.Object);
		SkeletalMeshComponent->SetMaterial(1, CMat.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> CShield(TEXT("/Script/Engine.MaterialInstanceConstant'/Game/Materials/Instancias/Translucent/UmbrellaShield/MI_Shield.MI_Shield'"));
	if (IsValid(CShield.Object))
	{
		MaterialOpened = CShield.Object;
	}
	
	FSoftObjectPath SoundCuePath(TEXT("SoundCue'/Game/Sounds/Rifle/SC_ShootRifle.SC_ShootRifle'"));
	SCShoot = Cast<USoundCue>(SoundCuePath.TryLoad());

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	HealthComponent->SetCanRegenerate(true);
	
	UpgradeComponent = CreateDefaultSubobject<UUpgradeComponent>(TEXT("UpgradeComponent"));

	MeleeComponent = CreateDefaultSubobject<UMeleeComponent>(TEXT("MeleeComponent"));
	
	ElectricEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ElectricEffect"));
	ElectricEffect->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> ElectricFXObj(TEXT("/Game/Particles/NiagaraSystems/NS_Electric.NS_Electric"));
	if (ElectricFXObj.Succeeded())
	{
		ElectricNiagaraSystemAsset = ElectricFXObj.Object;
	}

	ConstructorHelpers::FClassFinder<UNumericProgressBar> HealthBarWidget(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Widgets/WidgetBlueprints/Components/WBP_UmbrellaHealth.WBP_UmbrellaHealth_C'"));
	if (HealthBarWidget.Succeeded())
	{
		UmbrellaHealthWidget = HealthBarWidget.Class;
	}

	//to be moved to a grapple component
	GrappleStartTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("GrappleStartTimeline"));
}

void AUmbrella::BeginPlay()
{
	Super::BeginPlay();
	
	Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(),0));
	if (IsValid(Player))
	{
		CameraComponent = Player->FindComponentByClass<UCameraComponent>();
	}

	static const FString ImpactParticlePath = TEXT("/Game/Particles/P_Explosion.P_Explosion");
	ImpactParticleSystem = Cast<UParticleSystem>(StaticLoadObject(UParticleSystem::StaticClass(), nullptr, *ImpactParticlePath));
	
	SkeletalMeshComponent->PlayAnimation(CloseAnimSequence, true);
	ProtectionCollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	bOpen = false;
	bOnEnemy = false;

	if (IsValid(ElectricNiagaraSystemAsset))
	{
		ElectricEffect->SetAsset(ElectricNiagaraSystemAsset);
		ElectricEffect->SetActive(false);
	}

	UWidgetComponent* EnergyBar = GetEnergyBarComponent();
	if (IsValid(EnergyBar) && IsValid(UmbrellaHealthWidget))
	{
		EnergyBar->SetWidgetClass(UmbrellaHealthWidget);
		EnergyBar->SetRelativeLocation_Direct(FVector(100.f, 80.f, 0.f));
		EnergyBar->SetRelativeRotation_Direct(FRotator(180.f, 90.f, -28.f));
		EnergyBar->SetVisibility(false);
		EnergyBar->CastShadow = false;
			
		if (UNumericProgressBar* NumericHealthBar = Cast<UNumericProgressBar>(EnergyBar->GetWidget()))
		{
			NumericHealthBar->SetMaximumValue(GetHealth());
			NumericHealthBar->UpdateProgressBar(GetHealth());
		}
	}

	// Subscribing to changes in our Umbrella Health.
	if (IsValid(HealthComponent))
	{
		HealthComponent->OnHealthAmmountChanged.AddDynamic(this, &AUmbrella::UmbrellaHealthChangedUpdate);
	}

	// To be moved to a grapple component
	if (IsValid(GrappleStartTimeline))
	{
		GrappleStartTimeline->SetPlayRate(1.0f);
		
		FOnTimelineFloat GrappleStartProgressUpdate;
		GrappleStartProgressUpdate.BindUFunction(this, FName("GrappleStartTimelineProgress"));

		FOnTimelineEvent SwingFinishedEvent;
		SwingFinishedEvent.BindUFunction(this, FName("GrappleStartTimelineFinish"));
		
		if (IsValid(GrappleStartCurve))
		{
			GrappleStartTimeline->AddInterpFloat(GrappleStartCurve, GrappleStartProgressUpdate);
			GrappleStartTimeline->SetTimelineFinishedFunc(SwingFinishedEvent);
		}
	}
	ResetGrapple();
	RecalculateBulletSpreadPositions();
}

void AUmbrella::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsAiming() && !bOpen)
	{
		TArray<FHitResult> Hits = PerformSphereTrace(WidgetTraceRadius, BulletRange);
		bOnEnemy = IsEnemyInHit(Hits);
	}
	if ((bIsShooting || bOpen) && CableComponent->IsVisible())
	{
		if (GrappleStartTimeline->IsPlaying())
		{
			GrappleStartTimeline->Stop();
		}
		if (AEnemyCharacter* Character = Cast<AEnemyCharacter>(GrappleHitActor))
		{
			Character->EndGrapplingPosition();
		}
		ResetGrapple();
	}
	
	(void) OnUmbrellaStateChanged.ExecuteIfBound(IsAiming(), IsOpen(), IsOnEnemy());
}

#pragma endregion

#pragma region PRIVATE FUNCTIONS

void AUmbrella::RecalculateBulletSpreadPositions()
{
	SpawnPattern.Empty();
	
	for (int i = 0; i < MaxNrBulletPerShot; ++i)
	{
		SpawnPattern.Add(FRotator(0, i * (360 / MaxNrBulletPerShot), 0).Vector() * BulletPatternSize);
	}
}

bool AUmbrella::ShootingUmbrellaGrapple()
{
	if (!bCanShootGrapple)
	{
		UE_LOG(LogTemp, Warning, TEXT("Grapple still in cooldown"));
		return false;
	}
	
	bCanShootGrapple = false;
	
	GetWorldTimerManager().SetTimer(GrappleCooldownTimerHandle, this, &AUmbrella::ResetGrappleCooldown, GrappleCooldownDuration, false);
	
	APlayerCameraManager* CameraManager = Cast<APlayerCameraManager>(UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0));

	FVector  CameraLocation	= CameraManager->GetCameraLocation();
	FRotator CameraRotation = CameraManager->GetCameraRotation();

	FVector CameraFrontVector = CameraRotation.Vector();

	const FVector StartPoint = CameraLocation + CameraFrontVector * 100;
	const FVector Endpoint   = CameraLocation + CameraFrontVector * HookRange;
 
	DrawDebugLine(GetWorld(), CameraLocation, Endpoint, FColor::Green, false, 1, 0, 1);

	FHitResult GrappleHit(ForceInit);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor( this );

	// This has to be changed to a reference to the player directly instead, may be inefficient.
	QueryParams.AddIgnoredActor( UGameplayStatics::GetPlayerPawn(GetWorld(), 0) );

	if (GetWorld()->LineTraceSingleByChannel(GrappleHit, StartPoint, Endpoint, ECC_GameTraceChannel3, QueryParams)) // ECC_GameTraceChannel3 is  a custom channel : Hook
	{
		AActor* HitActor = GrappleHit.GetActor();
		
		if (IsValid(HitActor) && HitActor->Implements<UHookInterface>())
		{
			GrappleHitActor = HitActor;
			bIsGrapplingCharacter = GrappleHitActor->IsA(AGenericCharacter::StaticClass());
			GrappleEndPoint = GrappleHit.ImpactPoint;
			GrappleHitComponent = GrappleHit.GetComponent();
			CableComponent->SetVisibility(true);
			CableComponent->SetComponentTickEnabled(true);
			//move cable until contact point
				
			if (IsValid(GrappleStartTimeline))
			{
				GrappleStartTimeline->PlayFromStart();
				return true;
			}
		}
	}
	return true;
}

TArray<FHitResult> AUmbrella::PerformSphereTrace(float TraceRadius, float TraceLength)
{
	TArray<FHitResult> HitResults;

	if (TraceRadius == 0 || TraceLength == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("TraceRadius or TraceLength Missing! in Umbrella.cpp // PerformSphereTrace"));
		return HitResults;
	}

	Owner = GetOwner();
	APawn* OwnerPawn = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(),0));
	APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController());

	if (OwnerPawn && PC)
	{
		FVector CameraLocation;
		FRotator CameraRotation;
		PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

		FVector TraceStart = CameraLocation;
		FVector TraceEnd = TraceStart + (CameraRotation.Vector() * TraceLength);

		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

		TArray<AActor*> IgnoredActors;
		IgnoredActors.Add(OwnerPawn);

		EDrawDebugTrace::Type DebugDrawType = EDrawDebugTrace::None;

		UKismetSystemLibrary::SphereTraceMultiForObjects(
			this,
			TraceStart,
			TraceEnd,
			TraceRadius,
			ObjectTypes,
			false,
			IgnoredActors,
			DebugDrawType,
			HitResults,
			true
		);
	}
	return HitResults;
}

void AUmbrella::UmbrellaHealthChangedUpdate(int32 NewHealth, bool bLostHealth)
{
	// Updating the Umbrella Health Widget.
	UNumericProgressBar* NumericHealthBar = Cast<UNumericProgressBar>(GetEnergyBarComponent()->GetWidget());
	if (IsValid(NumericHealthBar))
	{
		NumericHealthBar->UpdateProgressBar(HealthComponent->GetHealth());
	}

	// Closing the umbrella if our health is over.
	if (GetHealth() <= 0)
	{
		Close();
	}
}

#pragma endregion

#pragma region PUBLIC FUNCTIONS

void AUmbrella::ToggleOpenClose()
{
	if (IsAiming())
	{
		if (bOpen)
		{
			Close();
		}
		else
		{
			Open();
		}
	}
}

void AUmbrella::Open()
{
	if (IsValid(HealthComponent) && GetHealth() > 0)
	{
		if (IsValid(MaterialOpened))
		{
			SkeletalMeshComponent->SetMaterial(1, MaterialOpened);
		}
		
		if (GetEnergyBarComponent())
		{
			GetEnergyBarComponent()->SetVisibility(true);
		}
	
		bOpen = true;

		SkeletalMeshComponent->PlayAnimation(OpenAnimSequence, true);
		ProtectionCollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ProtectionCollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
		ProtectionCollisionComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
		ProtectionCollisionComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		ProtectionCollisionComponent->SetCollisionResponseToChannel(ECC_Camera, ECR_Block);
		ProtectionCollisionComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
		
		OnIsCoveredByUmbrellaChanged.Broadcast(bOpen);
	}
}
 
void AUmbrella::Close()
{
	if (GetEnergyBarComponent())
	{
		GetEnergyBarComponent()->SetVisibility(false);
	}
	if (IsValid(MaterialClosed))
	{
		SkeletalMeshComponent->SetMaterial(1, MaterialClosed);
	}
	SkeletalMeshComponent->PlayAnimation(CloseAnimSequence, true);
	ProtectionCollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision );
	bOpen = false;
	
	OnIsCoveredByUmbrellaChanged.Broadcast(bOpen);
}

bool AUmbrella::TryToShoot()
{
	// Nothing happens if we were already shooting or the umbrella is Open.
	if (bIsShooting || IsOpen())
	{
		return false;
	}

	// If we try to shoot but have no bullets, then we try to Reload.
	if (AmmoCurrent <= 0)
	{
		StartReloading();
		return false;
	}

	// Blocking our ability to shoot for the cadence duration.
	bIsShooting = true;

	// Setting the timer to Reset our bIsShooting variable to false.
	FTimerHandle ShootingTimer;
	GetWorldTimerManager().SetTimer(
		ShootingTimer,
		this,
		&AUmbrella::ResetShooting,
		ShootingCadenceTime,
		false,
		ShootingCadenceTime
	);

	// Playing the shoot sound.
	if (IsValid(SCShoot))
	{
		UGameplayStatics::PlaySoundAtLocation(this, Cast<USoundBase>(SCShoot), GetActorLocation(), VolumeMultiplier);
	}

	const FVector StartCamera = CameraComponent->GetComponentLocation();
	const FVector EndCamera = StartCamera + (CameraComponent->GetForwardVector() * BulletRange);
	const FVector StartMuzzle = SkeletalMeshComponent->GetSocketLocation(TEXT("WeaponMuzzle"));

	FCollisionQueryParams ShootingCollisionQueryParams;
	ShootingCollisionQueryParams.AddIgnoredActor(this);
	if (IsValid(Player))
	{
		ShootingCollisionQueryParams.AddIgnoredActor(Player);
	}

	// Reducing our ammo count.
	if (!bInfiniteAmmo)
	{
		AmmoCurrent--;
		(void) OnCurrentBulletsChanged.ExecuteIfBound(AmmoCurrent);
	}

	// for each bullet, we spawn and line trace.
	for (int i = 0; i < MaxNrBulletPerShot; ++i)
	{
		FVector StartHit = StartCamera + BulletPatternSize * (SpawnPattern[i].X * CameraComponent->GetRightVector() + SpawnPattern[i].Y * CameraComponent->GetUpVector());
		FVector EndHit = EndCamera + (IsAiming() ? AimingBulletDispersion : NotAimingBulletDispersion) *
									   (SpawnPattern[i].X * CameraComponent->GetRightVector() + SpawnPattern[i].Y * CameraComponent->GetUpVector());

		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, StartHit, EndHit, ECC_Pawn, ShootingCollisionQueryParams) && Hit.bBlockingHit)
		{
			// If the actor is no longer valid, we stop.
			AActor* HitActor = Hit.GetActor();
			if (!IsValid(HitActor))
			{
				continue;
			}

			if (bDrawDebugBulletDispersion)
			{
				DrawDebugLine(GetWorld(), StartMuzzle, Hit.Location, FColor::Red, false, 1, 0, 2);
			}

			// Spawning hit particles.
			if (UParticleSystemComponent* PSC = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticleSystem, Hit.Location))
			{
				PSC->SetWorldScale3D(FVector(0.2f));
			}

			// Dealing damage if the actor had a HealthInterface.
			if (IHealthInterface* HealthInterface = Cast<IHealthInterface>(HitActor))
			{
				float DistanceMod = DistanceDamageModifier ? DistanceDamageModifier->GetFloatValue(FVector::Dist(Hit.Location, GetActorLocation()) / BulletRange) : 1.0f;
				HealthInterface->SubtractHealth(BulletDamage * DistanceMod, EWeaponType::Umbrella);
			}

			// If the actor can react to damage.
			if (IHitReactiveInterface* HitReactiveInterface = Cast<IHitReactiveInterface>(HitActor))
			{
				HitReactiveInterface->Reaction(HitSource::PLAYER_B);
			}
		}
		else if (bDrawDebugBulletDispersion) // We draw a green line to indicate we didn't hit anything.
		{
			DrawDebugLine(GetWorld(), StartHit, EndHit, FColor::Green, false, 5, 0, 0.5);
		}
	}

	return true; // We return true since we did shoot bullets. 
}

bool AUmbrella::GadgetShoot()
{
	switch (GadgetType)
	{
	case EGadgetType::Grapple:
		return ShootingUmbrellaGrapple();

	case EGadgetType::Electric:
		return OnElectricShoot();

	default:
		return false;
	}
}

bool AUmbrella::OnElectricShoot()
{
	if (!bCanShootElectric)
	{
		UE_LOG(LogTemp, Warning, TEXT("Dart still in cooldown"));
		return false;
	}

	bCanShootElectric = false;

	GetWorldTimerManager().SetTimer(ElectricCooldownTimerHandle, this, &AUmbrella::ResetElectricCooldown, ElectricCooldownDuration, false);
	
	FVector StartCamera = CameraComponent->GetComponentLocation();
	FVector EndCamera = StartCamera + (CameraComponent->GetForwardVector() * BulletRange);
	// Need this so the projectile doesn't collide with the shield (Surely there is a more elegant solution)
	const float SpawnForwardMargin = 20.f; 
	FVector SpawnLocation = Arrow->GetComponentLocation() + Arrow->GetForwardVector() + SpawnForwardMargin;
	FRotator SpawnRotation = Arrow->GetComponentRotation();

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(Player);

	FHitResult Hit;
	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, StartCamera, EndCamera, ECC_Pawn, QueryParams);

	FVector ProjectileDirection = bHit ? (Hit.Location - SpawnLocation).GetSafeNormal() : CameraComponent->GetForwardVector();

	if (AElectricProjectile* Projectile = GetWorld()->SpawnActor<AElectricProjectile>(AElectricProjectile::StaticClass(), SpawnLocation, SpawnRotation))
	{
		if (IsValid(Projectile->GetProjectileMovementComponent().Get()))
		{
			Projectile->GetProjectileMovementComponent()->Velocity = ProjectileDirection * Projectile->GetProjectileMovementComponent()->InitialSpeed;
		}
	}
	
	return true;
}

void AUmbrella::CycleBulletType()
{
	GadgetType = GadgetType == EGadgetType::Grapple ? EGadgetType::Electric : EGadgetType::Grapple;
	
	if (GadgetType == EGadgetType::Electric)
	{
		ElectricEffect->SetActive(true);
	}
	else
	{
		ElectricEffect->SetActive(false);
	}
}

void AUmbrella::SetBulletType(EGadgetType TargetGadget)
{
	GadgetType = TargetGadget;
	if (GadgetType == EGadgetType::Electric)
	{
		ElectricEffect->SetActive(true);
	}
	else
	{
		ElectricEffect->SetActive(false);
	}
}

bool AUmbrella::IsEnemyInHit(const TArray<FHitResult>& HitResults)
{
	for (const FHitResult& Hit : HitResults)
	{
		AGenericCharacter* Enemy = Cast<AGenericCharacter>(Hit.GetActor());
		if (IsValid(Enemy))
		{
			return true;
		}
	}

	return false;
}

void AUmbrella::SetAmmoCurrent(const int32 Amount)
{
	AmmoCurrent = Amount;
	(void) OnCurrentBulletsChanged.ExecuteIfBound(GetAmmoCurrent());
}

void AUmbrella::SetAmmoReserve(const int32 Amount)
{
	AmmoReserve = Amount;
	(void) OnTotalBulletsChanged.ExecuteIfBound(GetAmmoCurrent());
}

void AUmbrella::AutoReload()
{
	if (AmmoCurrent < AmmoCurrentMax)
	{
		int aux = AmmoCurrent;
		AmmoCurrent = AmmoCurrentMax;
		AmmoReserve = AmmoReserve - (AmmoCurrentMax - aux);
	}
}

#pragma endregion


//To be moved to grapple component
void AUmbrella::GrappleStartTimelineProgress(float Value)
{
	if (IsValid(GrappleHitActor))
	{
		if (bIsGrapplingCharacter)
		{
			GrappleEndPoint = GrappleHitActor->GetActorLocation();
		}
		FVector StartPoint = SkeletalMeshComponent->GetSocketLocation("WeaponMuzzle");
		FVector NewPoint = FMath::Lerp(StartPoint,GrappleEndPoint , Value);
		SetCableEndLocation(NewPoint);
	}
}

void AUmbrella::GrappleStartTimelineFinish()
{
	APlayerCharacter* OwnerPawn = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(),0));
	if (IsValid(OwnerPawn))
	{
		OwnerPawn->PlayGrappleAnim();
	}
						
	IHookInterface* HookInterface = Cast<IHookInterface>(GrappleHitActor);

	FVector SocketLocation = FVector::ZeroVector;
	if (IsValid(SkeletalMeshComponent))
	{
		SocketLocation = SkeletalMeshComponent->GetSocketLocation("WeaponMuzzle"); 
	}

	if (HookInterface)
	{
		switch ( HookInterface->GetGrabType() )
		{
		case IHookInterface::PULL:
			HookInterface->ContactWithCable(SocketLocation);
			break;
		case IHookInterface::PUSH:
			if (Player)
			{ 
				Player->MovePlayerViaGrapple(GrappleEndPoint);	
			}
			break;
		case IHookInterface::PULLWITHCOMPONENT:
			HookInterface->ContactWithCable(SocketLocation, GrappleHitComponent);
			break;
				
		default:
			break;
		}
	}
	if (IsValid(this))
	{
		ResetGrapple();
	}
}

void AUmbrella::ResetGrapple()
{
	bIsGrapplingCharacter = false;
	if (IsValid(CableComponent))
	{
		CableComponent->SetVisibility(false);
		CableComponent->SetComponentTickEnabled(false);
		if (IsValid(SkeletalMeshComponent))
		{
			SetCableEndLocation(SkeletalMeshComponent->GetSocketLocation("WeaponMuzzle"));
		}
	}
}

void AUmbrella::SetCableEndLocation(const FVector& EndPoint) const
{
	CableComponent->EndLocation = UKismetMathLibrary::InverseTransformLocation(this->GetActorTransform(), EndPoint);
}

void AUmbrella::StartReloading()
{
	const bool bAtMaxBullets = GetAmmoCurrent() == GetAmmoCurrentMax();
	const bool bHasBulletsToReload = GetAmmoReserve() <= 0;
	
	if (!GetWorldTimerManager().IsTimerActive(ReloadTimerHandle)
		&& !bIsShooting && !bAtMaxBullets && !bHasBulletsToReload)
	{
		GetWorldTimerManager().SetTimer(
			ReloadTimerHandle,
			this,
			&AUmbrella::ReloadOneShell,
			ReloadTime,
			true,
			ReloadTime
		);		
	}
}

void AUmbrella::ReloadOneShell()
{
	const bool bAtMaxBullets = GetAmmoCurrent() == GetAmmoCurrentMax();
	const bool bHasBulletsToReload = GetAmmoReserve() <= 0;
	
	if (bIsShooting || bAtMaxBullets || bHasBulletsToReload)
	{
		// INTERRUPTING RELOAD
		GetWorldTimerManager().ClearTimer(ReloadTimerHandle);
	}
	else
	{
		SetAmmoCurrent(GetAmmoCurrent() + 1);
		SetAmmoReserve(GetAmmoReserve() - 1);

		(void) OnCurrentBulletsChanged.ExecuteIfBound(GetAmmoCurrent());
		(void) OnTotalBulletsChanged.ExecuteIfBound(GetAmmoReserve());
	}
}
