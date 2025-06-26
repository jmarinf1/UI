// 
// EnemyCharacter.cpp
// 
// Implementation of the Enemy Character class.
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#include "EnemyCharacter.h"

#include "CableComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include <LadyUmbrella/Weapons/GenericWeapon.h>
#include "Components/CapsuleComponent.h"
#include "LadyUmbrella/AI/SplinePath.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Weapons/Umbrella/Umbrella.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "LadyUmbrella/AI/IAConstants.h"
#include "LadyUmbrella/Environment/ReactiveElements/ReactiveElement.h"
#include "LadyUmbrella/Items/InteractuableObjects/PickableObjects/Ammo.h"
#include "LadyUmbrella/Util/TransformUtils.h"
#include "LadyUmbrella/Weapons/Umbrella/Gadgets/ElectricProjectile.h"
#include "BrainComponent.h"
#include "Algo/RandomShuffle.h"
#include "Components/WidgetComponent.h"
#include "LadyUmbrella/AI/Controllers/AIControllerGeneric.h"
#include "LadyUmbrella/UI/Basics/EnemyStateIcon.h"


AEnemyCharacter::AEnemyCharacter()
    : currentZone(0)
    , CurrentTokenArrayPosition(-1)
    , GrapplingMaxTime(2.0f)
	, F_LaunchSpeedFactor(1000.f)
	, bPlayerLuckActive(false)
	, bHasToken(false)
	, StunDuration(2.f)
	, ReboundParticleRange(1000.0f)
	, DoesParticleRebound(false)
	, DefaultMissChance(0)
	, MissChance(DefaultMissChance)
	, ForgivenessTime(0.5f)
	, AimCoverForgivenessTime(2)
	, bCanInteractWithReactiveElements(true)
	, MinLeftRightOffsetLongDistance(100)
	, MaxLeftRightOffsetLongDistance(250)
	, MinLeftRightOffsetCloseDistance(250)
	, MaxLeftRightOffsetCloseDistance(350)
	, MinUpOffsetLateralBullet(-250)
	, MaxUpOffsetLateralBullet(250)
	, MinLeftRightOffsetOverhead(-150)
	, MaxLeftRightOffsetOverhead(150)
	, MinUpOffsetOverhead(50)
	, MaxUpOffsetOverhead(150)
	, EnemyKickedImpulse(3000)
	, ParticleExplosionScale(FVector(0.1f,0.1f,0.1f))
	, bIsReloading(false)
{
    PrimaryActorTick.bCanEverTick = true;

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>("EnemyStateIcon");
	WidgetComponent->SetupAttachment(GetRootComponent());
	WidgetComponent->AddRelativeLocation(FVector(0, 0, 100));
	WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);

	UCapsuleComponent* Capsule = GetCapsuleComponent();
	Capsule->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	Capsule->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Block); // for the hook
	
	AmmoDropAmountCurve = ConstructorHelpers::FObjectFinder<UCurveFloat>(TEXT("/Script/Engine.CurveFloat'/Game/DataStructures/Curves/Systems/C_AmmoDropAmount.C_AmmoDropAmount'")).Object;
	AmmoDropRateCurve = ConstructorHelpers::FObjectFinder<UCurveFloat>(TEXT("/Script/Engine.CurveFloat'/Game/DataStructures/Curves/Systems/C_AmmoDropRate.C_AmmoDropRate'")).Object;
	DeadForceCurve = ConstructorHelpers::FObjectFinder<UCurveFloat>(TEXT("/Script/Engine.CurveFloat'/Game/DataStructures/Curves/Combat/C_DeadImpulseForce.C_DeadImpulseForce'")).Object;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleSystemAsset(TEXT("/Game/Particles/P_Explosion.P_Explosion"));
	if (ParticleSystemAsset.Succeeded())
	{
		ParticleExplosionSystem = ParticleSystemAsset.Object;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load enemy particle system asset!"));
	}
}

void AEnemyCharacter::Initialize()
{
    const FTransform WeaponSocket = GetMesh()->GetSocketTransform(socketName, ERelativeTransformSpace::RTS_World);
    //FRotator Rotator = FRotator(-44.0,76.0,-145.0);
    //WeaponSocket.TransformRotation(UE::Math::TQuat<double>(Rotator));
    EquippedWeapon = GetWorld()->SpawnActor<AGenericWeapon>(WeaponBP, WeaponSocket);

    if (EquippedWeapon)
    {
        EquippedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, socketName);
        EquippedWeapon->SetOwner(this);
    }


    PlayerRef = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (IsValid(PlayerRef->GetUmbrella()))
	{
		PlayerRef->GetUmbrella()->OnIsCoveredByUmbrellaChanged.AddDynamic(this, &AEnemyCharacter::ModifyMissChanceDependingOnUmbrellaState);
	}
	
    //PlayerRef->OnIsCoveredChanged.AddDynamic(this, &AEnemyCharacter::ChangeEnemiesMissChance);
	if (IsValid(PlayerRef->GetCoverMovementComponent()))
	{
		PlayerRef->GetCoverMovementComponent()->OnIsCoveredChanged.AddDynamic(this, &AEnemyCharacter::ChangeEnemiesMissChance);
	}
	
	MissChance = DefaultMissChance;
}

AController* AEnemyCharacter::GetAIController() const
{
	return GetController();
}

void AEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();

	if (IsValid(EnemyStateWidget))
	{
		WidgetComponent->SetWidgetClass(EnemyStateWidget);
	}
}

void AEnemyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

ASplinePath* AEnemyCharacter::GetSplinePath() const
{
    return splinepath;
}

void AEnemyCharacter::SetSplinePath(ASplinePath* NewSplinePath)
{
    if (IsValid(NewSplinePath))
    {
        splinepath = NewSplinePath;
    }
}

void AEnemyCharacter::SetDead()
{
    Super::SetDead();
	//clean timer
	GetWorld()->GetTimerManager().ClearTimer(ActivatingElectricDardDelegate);
	
	//weapon simulate physics and destroy
	if (GetEquippedWeapon())
	{
		GetEquippedWeapon()->SimulatePhysicsBeforeDestroy();
	}

	if (IsValid(WidgetComponent))
	{
		WidgetComponent->DestroyComponent();
	}
	
	//Stop behaviour tree
	AAIControllerGeneric* AIController = Cast<AAIControllerGeneric>(GetAIController());
	if (IsValid(AIController))
	{
		AIController->GetBrainComponent()->StopLogic("DEAD");
		AIController->UnPossess();
	}
	//Drop ammo
	DropAmmo();

	//Ragdoll ----------------------------------------------------------
	SetReplicateMovement(false);
	DetachFromControllerPendingDestroy();

	//Disable collision in capsule
	UCapsuleComponent* Capsule = GetCapsuleComponent();
	Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Capsule->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetMesh()->SetCollisionProfileName(TEXT("RagDoll"));
	SetActorEnableCollision(true);
	
	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->WakeAllRigidBodies();
	GetMesh()->bBlendPhysics = true;

	UCharacterMovementComponent* CharacterComp = Cast<UCharacterMovementComponent>(GetMovementComponent());
	if (CharacterComp)
	{
		CharacterComp->StopMovementImmediately();
		CharacterComp->DisableMovement();
		CharacterComp->SetComponentTickEnabled(false);
	}
	//-------------------------------------------------------------------

	//Prepare to Destroy
	SetLifeSpan(10.f);
	
	if (LastDamageType == EWeaponType::Umbrella)
	{
		if (PlayerRef)
		{
			//Add Impulse
			FVector EnemyPlayerVector = (GetActorLocation() - PlayerRef->GetActorLocation()); //.GetSafeNormal();
			float DistanceEnemyPlayer = FVector::Distance(GetActorLocation(),PlayerRef->GetActorLocation());
			//fuerza horizontal
			const float HorizontalForce = DeadForceCurve->GetFloatValue(DistanceEnemyPlayer);
			//fuerza vertical
			const float VerticalForce = 15050.f;
			GetMesh()->AddImpulse(EnemyPlayerVector * HorizontalForce + GetActorUpVector() * VerticalForce);//GetActorLocation());
		}
	}
	else
	{
		//Anadir impulse para granade
		//GetMesh()->AddImpulse();
	}
}

void AEnemyCharacter::DropAmmo()
{
	// 
	// The drop system follows an exponential drop rate, in which getting an ammo drop gets more likely
	// the less ammo you currently have.
	//
	// This is done by checking the percentage of missing ammo, and plugging them into the two curve
	// assets requested by Design. These curves then give two additional percentage values, which tell
	// how much ammo should be dropped, and the probability of dropping the ammo.
	// 
	// Finally, the drop amount is calculated by taking this drop amount percentage and converting it
	// back into a discrete value, making sure we don't exceed the allowed limit.
	//
	if (IsValid(PlayerRef))
	{
		Umbrella = PlayerRef->GetUmbrella();
	}

	if (!IsValid(Umbrella))
	{
		FLogger::ErrorLog("[AEnemyCharacter::DropAmmo]: Invalid Umbrella Ref");
		return;
	}

	const int AmmoTotal = Umbrella->GetAmmoCurrent() + Umbrella->GetAmmoReserve();
	const int AmmoMax   = Umbrella->GetAmmoCurrentMax() + Umbrella->GetAmmoReserveMax();

	const float AmmoDelta = AmmoMax - AmmoTotal;
	const float AmmoDeltaPercentage = 1 - static_cast<float>(AmmoTotal) / AmmoMax;
	
	const int AmmoDropAmount = AmmoDropAmountCurve->GetFloatValue(AmmoDeltaPercentage) * AmmoDelta;
	const float AmmoDropRate = AmmoDropRateCurve->GetFloatValue(AmmoDeltaPercentage);

	/*
	FLogger::DebugLog("AmmoTotal: %d, AmmoMax: %d, AmmoDeltaPercentage: %f%%, AmmoDropAmount: %d, AmmoDropRate: %f%%",
		AmmoTotal, AmmoMax, AmmoDeltaPercentage * 100, AmmoDropAmount, AmmoDropRate * 100);
	*/

	if (AmmoDropAmount > 1)
	{
		if (FMath::RandRange(0.0f, 1.0f) <= AmmoDropRate)
		{
			const FTransform SpawnTransform = TransformUtils::TransformLocation(GetActorLocation());
			AAmmo* SpawnedAmmo = GetWorld()->SpawnActor<AAmmo>(AAmmo::StaticClass(), SpawnTransform);
			
			SpawnedAmmo->SetAmountToPickup(AmmoDropAmount);
		}
	}
}

void AEnemyCharacter::ContactWithCable(FVector& Direction)
{
    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying, 0);
    //GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    FTimerDelegate Delegate;
	Delegate.BindUFunction(this, "SetGrapplingPosition", Direction);
    GetWorld()->GetTimerManager().SetTimer(TimerHandle_GrapplingDelay, Delegate, GetWorld()->GetDeltaSeconds(), true);
	if (!IsValid(Umbrella) && IsValid(PlayerRef))
	{
		Umbrella = Cast<AUmbrella>(PlayerRef->GetUmbrella());
	}
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	
	// Setup delegate with parameters for EndGrapplingPosition
	if (IsValid(MovementComponent) && IsValid(Umbrella))
	{
		FTimerDelegate DelegateMaxTime;
		DelegateMaxTime.BindUFunction(this, "EndGrapplingPosition");
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_GrapplingMaxTime, DelegateMaxTime, GrapplingMaxTime, false);
	}
}

IHookInterface::GrabType AEnemyCharacter::GetGrabType()
{
    return GrabType::PULL;
}

void AEnemyCharacter::PlayReactMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	
	if (AnimInstance->Montage_IsPlaying(CurrentDamageAnimMontage) || AnimInstance->OnMontageEnded.IsBound())
	{
		return;
	}

	if (DamagedAnimMontages.Num() <= 0)
	{
		return;
	}
	
	Algo::RandomShuffle(DamagedAnimMontages);
	CurrentDamageAnimMontage = DamagedAnimMontages[0];
	
	if (IsValid(AnimInstance) && IsValid(CurrentDamageAnimMontage))
	{
		AnimInstance->Montage_Play(CurrentDamageAnimMontage);
		AnimInstance->Montage_SetPlayRate(CurrentDamageAnimMontage, 1.0f);
		AnimInstance->OnMontageEnded.AddDynamic(this, &AEnemyCharacter::OnDamagedMontageCompleted);
	}
}

void AEnemyCharacter::PlayKickedMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (DamagedAnimMontages.Num() <= 0)
	{
		return;
	}
	
	Algo::RandomShuffle(DamagedAnimMontages);
	CurrentDamageAnimMontage = DamagedAnimMontages[0];
	
	if (IsValid(AnimInstance) && IsValid(CurrentDamageAnimMontage))
	{
		AnimInstance->Montage_Play(CurrentDamageAnimMontage);
		AnimInstance->Montage_SetPlayRate(CurrentDamageAnimMontage, 1.0f);
		AnimInstance->OnMontageEnded.AddDynamic(this, &AEnemyCharacter::OnKickedMontageCompleted);
	}
}

void AEnemyCharacter::LaunchActor(FVector Direction)
{
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	float UpForce = 10.f;
	if (MovementComponent)
	{
		MovementComponent->Launch(Direction * EnemyKickedImpulse + GetActorUpVector() * UpForce);
	}
	PlayKickedMontage();
}

void AEnemyCharacter::KnockBackActor()
{
	PauseAILogic("Kicked");
	
	if (IsValid(EnemyStunTexture))
	{
		GetWidget()->SetImageState(EnemyStunTexture);
	}
	
	NotifyExitCover();
}

void AEnemyCharacter::ChangeEnemiesMissChance(const bool NewState)
{
	// If not in cover
    if (!NewState)
    {
    	GetWorld()->GetTimerManager().ClearTimer(MissChanceTimer);
    	GetWorld()->GetTimerManager().ClearTimer(CoverMissChanceTimer);
    	
    	if (bPlayerLuckActive)
    	{
    		if (!GetWorldTimerManager().IsTimerActive(PlayerLuckTimer))
    		{
    			GetWorldTimerManager().SetTimer(PlayerLuckTimer, this, &AEnemyCharacter::LosePlayerLuck, ForgivenessTime, false);
    		}
    	}
    	else
    	{
    		SetMissChance(DefaultMissChance);
    	}
    }
    else
    {
    	bPlayerLuckActive = true;
    	GetWorld()->GetTimerManager().ClearTimer(PlayerLuckTimer);
    	if (PlayerRef->IsAiming())
    	{
    		GetWorldTimerManager().SetTimer(MissChanceTimer, this, &AEnemyCharacter::WaitToChangeEnemiesMissChance, AimCoverForgivenessTime, false);
			GetWorld()->GetTimerManager().ClearTimer(CoverMissChanceTimer);
    	}
	    else
	    {
    		GetWorld()->GetTimerManager().ClearTimer(MissChanceTimer);
    		CheckLineofSightWithPlayer();
    		GetWorldTimerManager().SetTimer(CoverMissChanceTimer, this, &AEnemyCharacter::CheckLineofSightWithPlayer, EquippedWeapon->GetMinWeaponCadence() / 2, true);
	    }
    }
}

void AEnemyCharacter::ModifyMissChanceDependingOnUmbrellaState(const bool IsShieldOpen)
{
	if (IsShieldOpen)
	{
		SetMissChance(0);
	}
	else
	{
		if (bPlayerLuckActive && GetWorldTimerManager().IsTimerActive(PlayerLuckTimer))
		{
			return;
		}
		SetMissChance(DefaultMissChance);
	}
}

void AEnemyCharacter::WaitToChangeEnemiesMissChance()
{
    //FLogger::DebugLog("Cambiando la probabilidad de fallar a DefaultMissChance: %f", DefaultMissChance);
    SetMissChance(DefaultMissChance);
}

void AEnemyCharacter::LosePlayerLuck()
{
	bPlayerLuckActive = false;
	SetMissChance(DefaultMissChance);
}

void AEnemyCharacter::CheckLineofSightWithPlayer()
{
	FVector LineTraceStart;
	FVector LineTraceEnd;
	FHitResult OutHit;
	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(this);
	IgnoredActors.Add(PlayerRef->GetUmbrella());
	
	FVector EnemyViewLocation;
	FRotator EnemyRotation;
	GetActorEyesViewPoint(EnemyViewLocation, EnemyRotation);
	LineTraceStart = EnemyViewLocation;
	
	FVector VerticalLinetraceOffset = FVector(0, 0, 20);;
	LineTraceEnd = PlayerRef->GetActorLocation() - VerticalLinetraceOffset;

	if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), LineTraceStart, LineTraceEnd, ETraceTypeQuery::TraceTypeQuery2, false, IgnoredActors, EDrawDebugTrace::None, OutHit, true, FColor::Yellow))
	{
		AActor* ManagingActor = OutHit.HitObjectHandle.GetManagingActor();
		if (ManagingActor)
		{
			if (ManagingActor->IsA(APlayerCharacter::StaticClass()))
			{
				SetMissChance(DefaultMissChance);
			}
			else
			{
				SetMissChance(1);
			}
		}
	}
}

void AEnemyCharacter::PauseAILogic(FString Cause)
{
	AAIController* AIController = Cast<AAIController>(GetController());
	if (IsValid(AIController))
	{
		AIController->StopMovement();
		if (IsValid(AIController->BrainComponent))
		{
			AIController->BrainComponent->PauseLogic(Cause);
		}
	}
}

void AEnemyCharacter::Fire()
{
    if (!EquippedWeapon)
    {
        return;
    }

	 //if shoot from cover
	 if (IsAiming() && IsMovementControlledByCover())
	 {
	 	FVector AimDirection = PlayerRef->GetActorLocation() - GetActorLocation();
	 	SetInCoverAimingDirection(AimDirection);
	 }
	
	if (EquippedWeapon->GetWeaponBulletType() == EBulletType::Electric)
	{
		OnElectricShoot();
		return;
	}
    float RandomValue = FMath::FRandRange(0.f, 1.f);
    bool bMiss;
    if (RandomValue <= MissChance)
    {
        bMiss = true;
    }
    else
    {
        bMiss = false;
    }

	FHitResult OutHit;
	GetFireHitResult(OutHit, bMiss);

	if (EquippedWeapon->GetAmmoCurrent() > 0 && !bIsReloading)
	{
		EquippedWeapon->Fire();
	}
}

AGenericWeapon* AEnemyCharacter::GetEquippedWeapon() const
{
    return EquippedWeapon;
}

bool AEnemyCharacter::GetIsReloading() const
{
    return bIsReloading;
}

void AEnemyCharacter::SetMissChance(float newMissChance)
{
    MissChance = newMissChance;
}

float AEnemyCharacter::GetMissChance() const
{
    return MissChance;
}

UEnemyStateIcon* AEnemyCharacter::GetWidget() const
{
	UEnemyStateIcon* EnemyStateIcon = Cast<UEnemyStateIcon>(WidgetComponent->GetUserWidgetObject());
	return EnemyStateIcon;
}

void AEnemyCharacter::Reload()
{
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

    bIsReloading = true;

    if (AnimInstance && ReloadMontage)
    {
        AnimInstance->OnMontageEnded.AddDynamic(this, &AEnemyCharacter::OnReloadCompleted);
        AnimInstance->Montage_Play(ReloadMontage);
    }
    else
    {
        EquippedWeapon->Reload(false);
    }
}

void AEnemyCharacter::OnReloadCompleted(UAnimMontage* Montage, bool bInterrupted)
{
    if (Montage == ReloadMontage)
    {
        bIsReloading = false;
        GetMesh()->GetAnimInstance()->OnMontageEnded.RemoveDynamic(this, &AEnemyCharacter::OnReloadCompleted);
        EquippedWeapon->Reload(false);
    }
}

void AEnemyCharacter::SetWeaponBP(const TSubclassOf<AGenericWeapon>& NewWeaponBP)
{
    WeaponBP = NewWeaponBP;
}

void AEnemyCharacter::SetHasToken(const bool newTokenState)
{
    bHasToken = newTokenState;
}

bool AEnemyCharacter::GetHasToken() const
{
    return bHasToken;
}

void AEnemyCharacter::SubtractHealth(const float _HealthToSubtract,enum EWeaponType DamageType)
{
    Super::SubtractHealth(_HealthToSubtract,DamageType);
	
	if (!HealthComponent->IsDead())
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
				
		PlayReactMontage();
	}
}

void AEnemyCharacter::SetGrapplingPosition(FVector& EndPoint)
{
    FVector ActorLocation = GetActorLocation();
    FVector SpeedToMoveActor = UKismetMathLibrary::GetDirectionUnitVector(ActorLocation, EndPoint);

    if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
    {
        LaunchCharacter(SpeedToMoveActor * F_LaunchSpeedFactor, true, true);
        FVector CalculusVector = ActorLocation - EndPoint;
        
        // Update cable position
        if (IsValid(Umbrella))
        {
            Umbrella->SetCableEndLocation(GetActorLocation());
            Umbrella->GetCableComponent()->SetVisibility(true);
            Umbrella->GetCableComponent()->SetComponentTickEnabled(true);
        }
        bool NearlyEqual = UKismetMathLibrary::NearlyEqual_FloatFloat(CalculusVector.Length(),0,100);
        if (NearlyEqual || !GetCoverMovementComponent()->CanCharacterBePulledByHook())
        {
            // Reset movement for the player and clear timers
        	EndGrapplingPosition();
        }
    }
}

void AEnemyCharacter::EndGrapplingPosition()
{
	if (UCharacterMovementComponent* MovementComponent = this->GetCharacterMovement(); IsValid(MovementComponent) && IsValid(Umbrella))
	{
		// Reset movement for the player and clear timer
		MovementComponent->StopMovementImmediately();
		MovementComponent->SetMovementMode(EMovementMode::MOVE_Walking);
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_GrapplingDelay);

		if (IsValid(Umbrella->GetCableComponent()))
		{
			Umbrella->GetCableComponent()->EndLocation = FVector::ZeroVector;
			Umbrella->GetCableComponent()->SetVisibility(false);
			Umbrella->GetCableComponent()->SetComponentTickEnabled(false);
		}
	}
}

// Should really move all that to a component
void AEnemyCharacter::SpawnBulletNiagaraSystem(FVector WeaponTraceStart, FVector WeaponTraceEnd)
{
	if (GetEquippedWeapon()->GetBulletNiagaraSystem().IsValid())
	{
		GetEquippedWeapon()->SetBulletParticles(UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, GetEquippedWeapon()->GetBulletNiagaraSystem().Get(), WeaponTraceStart));
		if (IsValid(GetEquippedWeapon()->GetBulletParticles().Get()))
		{
			GetEquippedWeapon()->GetBulletParticles().Get()->SetVectorParameter("start", WeaponTraceStart);
			GetEquippedWeapon()->GetBulletParticles().Get()->SetVectorParameter("end", WeaponTraceEnd);
			if (IsValid(this) && GetEquippedWeapon()->GetBulletParticles().IsValid())
			{
			    GetEquippedWeapon()->GetBulletParticles()->OnSystemFinished.AddDynamic(this, &ThisClass::OnNiagaraSystemFinished);
			}
		}
	}
}
// Should really move all that to a component
void AEnemyCharacter::OnNiagaraSystemFinished(UNiagaraComponent* PSystem)
{
	if (IsValid(GetEquippedWeapon()))
	{
		if (DoesParticleRebound)
		{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleExplosionSystem,WeaponTraceEndPoint , FRotator::ZeroRotator, ParticleExplosionScale);
		GetEquippedWeapon()->SetBulletParticles(UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, GetEquippedWeapon()->GetBulletNiagaraSystem().Get(), WeaponTraceEndPoint));
		GetEquippedWeapon()->GetBulletParticles().Get()->SetVectorParameter("start", WeaponTraceEndPoint);
		FVector RandomVector = FVector(
		FMath::FRandRange(-ReboundParticleRange, ReboundParticleRange),
		FMath::FRandRange(-ReboundParticleRange, ReboundParticleRange),
		FMath::FRandRange(-ReboundParticleRange, ReboundParticleRange)
		);
		GetEquippedWeapon()->GetBulletParticles().Get()->SetVectorParameter("end", WeaponTraceEndPoint + RandomVector);
		}
	}
	// else
	//{}
	// Any behavior we want when the particle doesn't bounce off the shield
}

FVector AEnemyCharacter::GetBulletMissedDirection(const FVector& WeaponTraceDirection, const FVector& WeaponTraceEnd)
{
	FHitResult PawnTraceHit;
	FVector PlayerRight = FVector::CrossProduct(WeaponTraceDirection, FVector::UpVector);
	float RandomOffsetLeftRight;
	float RandomOffsetUp;

	float BulletMissingLeftRightProbability = 66;
			
	if (FMath::RandRange(0, 100) < BulletMissingLeftRightProbability)
	{
		if (PawnTraceHit.Distance > IAConstants::PLAYER_DISTANCE_CHANGE_MISS_BULLET_LATERAL_OFFSET)
		{			
			RandomOffsetLeftRight = FMath::RandRange(MinLeftRightOffsetLongDistance, MaxLeftRightOffsetLongDistance);
		}
		else
		{
			RandomOffsetLeftRight = FMath::RandRange(MinLeftRightOffsetCloseDistance, MaxLeftRightOffsetCloseDistance);
		}

		if (FMath::RandBool())
		{
			RandomOffsetLeftRight = -RandomOffsetLeftRight;
		}

		RandomOffsetUp = FMath::RandRange(MinUpOffsetLateralBullet, MaxUpOffsetLateralBullet);
	}
	else
	{
		RandomOffsetLeftRight = FMath::RandRange(MinLeftRightOffsetOverhead, MaxLeftRightOffsetOverhead);

		RandomOffsetUp = FMath::RandRange(MinUpOffsetOverhead, MaxUpOffsetOverhead);
	}
			
	FVector RandomLeftOrRightVector = PlayerRight * RandomOffsetLeftRight;
			
	FVector RandomUpVector = FVector::UpVector * RandomOffsetUp;
			
	return (WeaponTraceEnd + RandomUpVector + RandomLeftOrRightVector);
}

void AEnemyCharacter::BulletInteractionWithReactiveElements(const FHitResult& OutHit)
{
	if (!bCanInteractWithReactiveElements || !IsValid(OutHit.GetActor()))
	{
		return;
	}
	if (OutHit.GetActor()->GetClass()->ImplementsInterface(UHitReactiveInterface::StaticClass()))
	{
		Cast<AReactiveElement>(OutHit.GetActor())->Reaction(HitSource::ENEMY);
	}
}

void AEnemyCharacter::OnElectricShoot()
{
	// if (EquippedWeapon->IsElectricBulletActivated()) return;
	//
	// EquippedWeapon->SetElectricBulletActivated(true);
	FVector PlayerLocation;
	FRotator PlayerRotation;
	PlayerRef->GetActorEyesViewPoint(PlayerLocation, PlayerRotation);
	USkeletalMeshComponent* WeaponMesh = GetEquippedWeapon()->GetSkeletalMeshComponent().Get();
	if (!IsValid(WeaponMesh))
	{
		return;
	}

	EquippedWeapon->SpawnElectricEmmiter();
	GetWorld()->GetTimerManager().SetTimer(ActivatingElectricDardDelegate, [this,PlayerLocation,WeaponMesh]()
	{
		FVector WeaponTraceStart = WeaponMesh->GetSocketLocation(TEXT("MuzzleSocket"));//WeaponMuzzle->GetSocketLocation(WeaponMesh);
		FVector WeaponTraceDirection = (PlayerLocation - WeaponTraceStart).GetSafeNormal();
		//save projectile data
		ProjectileSpawnLocation = WeaponTraceStart + WeaponTraceDirection * 10;
		ProjectileSpawnRotation = WeaponMesh->GetSocketRotation(TEXT("MuzzleSocket"));
		ProjectileDirection = WeaponTraceDirection;
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this; // Example: set owner to the current actor

		// DrawDebugSphere(GetWorld(), PlayerRef->GetActorLocation(), 10, 10, FColor::Red, false, 10);
		// DrawDebugSphere(GetWorld(), ProjectileSpawnLocation, 10, 10, FColor::Blue, false, 10);

		if (AElectricProjectile* Projectile = GetWorld()->SpawnActor<AElectricProjectile>(ElectricProjectileClass, ProjectileSpawnLocation, ProjectileSpawnRotation,SpawnParams))
		{
			//FLogger::DebugLog("Spawneo Dardo Electrico");
			if (IsValid(Projectile->GetProjectileMovementComponent().Get()))
			{
				//FLogger::DebugLog("Disparo Dardo Electrico");
				Projectile->GetProjectileMovementComponent()->Velocity = ProjectileDirection * Projectile->GetProjectileMovementComponent()->InitialSpeed;
			}
			EquippedWeapon->StopElectricEmmiter();
		}
	
		EquippedWeapon->SetElectricBulletShooted(true);
		EquippedWeapon->SetElectricBulletActivated(false);
		EquippedWeapon->SetWeaponBulletType(EBulletType::Normal);
	}, EquippedWeapon->GetElectricBulletCooldown(), false);
}

void AEnemyCharacter::BulletInteraction(const FHitResult& OutHit)
{
	if (!IsValid(OutHit.GetActor()))
	{
		return;
	}
	BulletInteractionWithReactiveElements(OutHit);
	if (IHealthInterface* HealthInterface = Cast<IHealthInterface>(OutHit.GetActor()))
	{
		if (OutHit.GetActor()->IsA(APlayerCharacter::StaticClass()))
		{
			HealthInterface->SubtractHealth(GetEquippedWeapon()->GetWeaponDamage(),WeaponType, this->GetActorLocation());
		}

		if (AUmbrella* UmbrellaObj = Cast<AUmbrella>(OutHit.GetActor()))
		{
			UmbrellaObj->SubtractHealth(GetEquippedWeapon()->GetWeaponDamage(), WeaponType);
		}
	}
}

void AEnemyCharacter::GetFireHitResult(FHitResult &OutHit, bool bMiss)
{
	USkeletalMeshComponent* WeaponMesh = GetEquippedWeapon()->GetSkeletalMeshComponent().Get();
	
	if (!IsValid(WeaponMesh))
	{
		return;
	}
	
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	//if has shield , ignore
	if (bHasShield)
	{
		if (AActor* ShieldActor = GetShield())
		{
			CollisionParams.AddIgnoredActor(ShieldActor);
		}
	}

	// Linetrace from Enemy
	FVector EnemyLinetraceStart;
	FRotator EnemyRotation;
	if (IsValid(GetController()))
	{
		GetController()->GetPlayerViewPoint(EnemyLinetraceStart, EnemyRotation);
	}

	FVector EnemyLinetraceEnd;
	//FVector vPawnTraceEnd = vStart + oRotation.Vector() * iWeaponRange;
	UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetActorEyesViewPoint(EnemyLinetraceEnd, EnemyRotation);
	FHitResult EnemyTraceOutHit;

	GetWorld()->LineTraceSingleByChannel(EnemyTraceOutHit, EnemyLinetraceStart, EnemyLinetraceEnd, ECC_Camera, CollisionParams);
	
	FVector PawnTraceHitPoint = EnemyTraceOutHit.bBlockingHit ? EnemyTraceOutHit.ImpactPoint : EnemyLinetraceEnd;

	DrawDebugLine(GetWorld(), EnemyLinetraceStart, EnemyLinetraceEnd, FColor::Blue, false, -1, 0, 1);
	
	// Linetrace from the gun
	if (WeaponMesh->DoesSocketExist(TEXT("MuzzleSocket")))
	{
		FVector WeaponTraceStart = WeaponMesh->GetSocketLocation(TEXT("MuzzleSocket"));//WeaponMuzzle->GetSocketLocation(WeaponMesh);
		FVector WeaponTraceDirection = (PawnTraceHitPoint - WeaponTraceStart).GetSafeNormal();
		FVector WeaponTraceEnd = PawnTraceHitPoint + WeaponTraceDirection * EquippedWeapon->GetWeaponRange();
		WeaponTraceEndPoint = WeaponTraceEnd; // reset rebound position
		DoesParticleRebound = false;
		
		if (!bMiss)
		{
			GetWorld()->LineTraceSingleByChannel(OutHit, WeaponTraceStart, WeaponTraceEnd, ECC_Camera, CollisionParams);

			DrawDebugLine(GetWorld(), WeaponTraceStart, WeaponTraceEnd, FColor::Red, false, -1, 0, 1);

			BulletInteraction(OutHit);

			if (PlayerRef->GetHealth() <= 0)
			{
				SpawnBulletNiagaraSystem(WeaponTraceStart, WeaponTraceEnd);
				return;
			}
			if (IsValid(PlayerRef->GetUmbrella()))
			{
				if (PlayerRef->GetUmbrella()->IsOpen())
				{
					DoesParticleRebound = true;
					WeaponTraceEndPoint = OutHit.Location; // set rebound position
					// tiny problem with setting an end point for those particles: When you're really close to the enemy, the bullet goes super slow!
					// this is due to the fact that its lifetime remain constant, while the distance changes, will look at modifying the lifetime based on distance travelled.
					// AP
					SpawnBulletNiagaraSystem(WeaponTraceStart, WeaponTraceEndPoint);
					return;
				}
			}
		}

		
		FVector WeaponTraceEndMiss = GetBulletMissedDirection(WeaponTraceDirection, WeaponTraceEnd);

		GetWorld()->LineTraceSingleByChannel(OutHit, WeaponTraceStart, WeaponTraceEndMiss, ECC_Camera, CollisionParams);
		DrawDebugLine(GetWorld(), WeaponTraceStart, WeaponTraceEndMiss, FColor::Red, false, -1, 0, 1);

		SpawnBulletNiagaraSystem(WeaponTraceStart, WeaponTraceEndMiss);

		
		

		if (OutHit.bBlockingHit && OutHit.GetActor())
		{
			BulletInteractionWithReactiveElements(OutHit);
		}
	}
	
	return;
}

void AEnemyCharacter::ApplyStun(float Duration)
{
	Super::ApplyStun(Duration);

	AAIController* AIController = Cast<AAIController>(GetController());
	if (IsValid(AIController))
	{
		AIController->StopMovement();
	}
	//PauseAILogic("Stunned");
}

void AEnemyCharacter::OnDamagedMontageCompleted(UAnimMontage* Montage, bool bInterrupted)
{
	if (IsValid(Montage) && Montage == CurrentDamageAnimMontage)
	{
		GetMesh()->GetAnimInstance()->OnMontageEnded.RemoveDynamic(this, &AEnemyCharacter::OnDamagedMontageCompleted);
	}
}

void AEnemyCharacter::OnKickedMontageCompleted(UAnimMontage* Montage, bool bInterrupted)
{
	if (IsValid(Montage) && Montage == CurrentDamageAnimMontage)
	{
		GetMesh()->GetAnimInstance()->OnMontageEnded.RemoveDynamic(this, &AEnemyCharacter::OnKickedMontageCompleted);
		AAIController* AIController = Cast<AAIController>(GetController());
		if (IsValid(AIController))
		{
			AIController->BrainComponent->ResumeLogic(TEXT("EndStun"));
		}
	}
}

void AEnemyCharacter::EndStun()
{
	Super::EndStun();
}