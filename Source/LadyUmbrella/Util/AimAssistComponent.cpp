#include "AimAssistComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "LadyUmbrella/Characters/GenericCharacter.h"
#include "Camera/CameraComponent.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"

UAimAssistComponent::UAimAssistComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    
    // Snapping
    CapsuleRadius = 150.0f;
    CapsuleLength = 2000.0f;
    AimAssistSpeed = 15.0f;
    AimAssistSpeedMultiplier = 1.0f;
    AimAngleThreshold = 0.5f;
    TargetOffset = FVector(0.f, 0.f, 60.f);

    // Soft Lock
    SoftLockAngle = 5.0f;
    SoftLockSlowMultiplier = 0.5f;
    bShouldSlowAiming = false;
    bIsAiming = false;
    SoftLockRadius = 100.0f;
    SoftLockLength = 2000.0f;

    // Debug
    bShowDebugSnap = false;
    bShowDebugSoftLock = false;
    bShowDebugVisibility = false;
    bDebugMessages = false;

    // Internal
    CurrentTarget = nullptr;
    CameraSnapStartOffset = 350.0f;
}

void UAimAssistComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UAimAssistComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    if (bIsAiming)
    {
        if (CurrentTarget && !CurrentTarget->bIsDead)
        {
            if (APlayerController* PC = Cast<APlayerController>(Cast<APawn>(GetOwner())->GetController()))
            {
                RotateViewToTarget(PC, CurrentTarget->GetActorLocation(), DeltaTime);
            }
        }
        else
        {
            CheckForSoftLock();
        }
    }
}

void UAimAssistComponent::OnStartAiming()
{
    APlayerController* PC = Cast<APlayerController>(Cast<APawn>(GetOwner())->GetController());
    if (PC)
    {
        FVector CameraLocation;
        FRotator CameraRotation;
        PC->GetPlayerViewPoint(CameraLocation, CameraRotation);
        FVector CameraDirection = CameraRotation.Vector();

        TArray<FHitResult> Hits = PerformSphereTrace(CapsuleRadius, CapsuleLength, bShowDebugSnap);
        TArray<AGenericCharacter*> Enemies = FilterUniqueEnemies(Hits);

        if (Enemies.Num() > 0)
        {
            AGenericCharacter* PotentialTarget  = FindBestTarget(Enemies, CameraLocation, CameraDirection);

            // Do a visibility trace before locking on
            FHitResult LineHit;
            FVector TraceStart = CameraLocation;
            FVector TraceEnd = PotentialTarget->GetActorLocation() + TargetOffset;

            FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(AimAssistVisibilityTrace), true, GetOwner());
            TraceParams.AddIgnoredActor(GetOwner());

            bool bHit = GetWorld()->LineTraceSingleByChannel(LineHit, TraceStart, TraceEnd, ECC_Pawn, TraceParams);
            if (!bHit || LineHit.GetActor() == PotentialTarget)
            {
                CurrentTarget = PotentialTarget;
                if (bDebugMessages) UE_LOG(LogTemp, Warning, TEXT("Target visible and set: %s"), *CurrentTarget->GetName());
                if (bShowDebugVisibility) DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Green, false, 1.5f, 0, 2.0f);
            }
            else
            {
                CurrentTarget = nullptr;
                if (bDebugMessages)
                {
                    UPrimitiveComponent* HitComp = LineHit.GetComponent();
                    UE_LOG(LogTemp, Warning, TEXT("Target blocked by: %s"), *GetNameSafe(LineHit.GetActor()));
                    UE_LOG(LogTemp, Warning, TEXT("Blocked by component: %s"), *GetNameSafe(HitComp));
                    UE_LOG(LogTemp, Warning, TEXT("Collision object type: %d"), HitComp ? (int32)HitComp->GetCollisionObjectType() : -1);
                }
                if (bShowDebugVisibility)
                {
                    DrawDebugLine(GetWorld(), TraceStart, LineHit.Location, FColor::Red, false, 1.5f, 0, 2.0f);
                    DrawDebugPoint(GetWorld(), LineHit.Location, 10.0f, FColor::Red, false, 1.5f);
                }
            }
            
            if (bDebugMessages)UE_LOG(LogTemp, Warning, TEXT("Total unique enemies detected: %d"), Enemies.Num());
        }
        else
        {
            if (bDebugMessages)UE_LOG(LogTemp, Warning, TEXT("No enemies detected in aim assist trace."));
            CurrentTarget = nullptr;
        }

        bIsAiming = true; //might have to put it in the tick if there's stuff such as actions that stop aiming without calling the function resetAiming() of the character
    }
}

TArray<FHitResult> UAimAssistComponent::PerformSphereTrace(float TraceRadius, float TraceLength, bool ShowDebug = false)
{
    TArray<FHitResult> HitResults;

    if (TraceRadius == 0 || TraceLength == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("TraceRadius or TraceLength Missing! in AimAssistComponent.cpp // PerformSphereTrace"));
        return HitResults;
    }

    AActor* Owner = GetOwner();
    APawn* OwnerPawn = Cast<APawn>(Owner);
    if (!OwnerPawn) return HitResults;

    APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController());
    if (!PC) return HitResults;

    FVector CameraLocation;
    FRotator CameraRotation;
    PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

    FVector TraceStart = CameraLocation + (CameraRotation.Vector() * CameraSnapStartOffset);
    FVector TraceEnd = TraceStart + (CameraRotation.Vector() * TraceLength);

    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

    TArray<AActor*> IgnoredActors;
    IgnoredActors.Add(OwnerPawn);

    EDrawDebugTrace::Type DebugDrawType = ShowDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

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

    return HitResults;
}


TArray<AGenericCharacter*> UAimAssistComponent::FilterUniqueEnemies(const TArray<FHitResult>& HitResults)
{
    TArray<AGenericCharacter*> UniqueEnemies;

    for (const FHitResult& Hit : HitResults)
    {
        AGenericCharacter* Enemy = Cast<AGenericCharacter>(Hit.GetActor());
        if (Enemy && !UniqueEnemies.Contains(Enemy))
        {
            UniqueEnemies.Add(Enemy);
        }
    }

    return UniqueEnemies;
}

AGenericCharacter* UAimAssistComponent::FindBestTarget(const TArray<AGenericCharacter*>& Enemies, const FVector& ViewLocation, const FVector& ViewDirection)
{
    AGenericCharacter* BestTarget = nullptr;
    float SmallestAngle = MAX_flt;

    for (AGenericCharacter* Enemy : Enemies)
    {
        FVector ToEnemy = (Enemy->GetActorLocation() - ViewLocation).GetSafeNormal();
        float Angle = FMath::RadiansToDegrees(acosf(FVector::DotProduct(ViewDirection, ToEnemy)));

        if (Angle < SmallestAngle)
        {
            SmallestAngle = Angle;
            BestTarget = Enemy;
        }
    }

    return BestTarget;
}

void UAimAssistComponent::RotateViewToTarget(APlayerController* PC, const FVector& TargetLocation, float DeltaTime)
{
    FVector CameraLocation;
    FRotator CameraRotation;
    PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

    FVector FinalTarget = TargetLocation + TargetOffset;

    FVector ToTarget = (FinalTarget - CameraLocation).GetSafeNormal();
    FRotator TargetRotation = ToTarget.Rotation();

    float Dot = FVector::DotProduct(CameraRotation.Vector(), ToTarget);
    Dot = FMath::Clamp(Dot, -1.0f, 1.0f);
    float AngleRadians = FMath::Acos(Dot);
    float AngleDegrees = FMath::RadiansToDegrees(AngleRadians);

    if (AngleDegrees < AimAngleThreshold)
    {
        CurrentTarget = nullptr;
        return;
    }

    float Speed = AimAssistSpeed + (AngleDegrees * AimAssistSpeedMultiplier);
    FRotator NewRotation = FMath::RInterpTo(CameraRotation, TargetRotation, DeltaTime, Speed);
    PC->SetControlRotation(NewRotation);
}

void UAimAssistComponent::StopRotation()
{
    CurrentTarget = nullptr;
    bIsAiming = false;
    if (APlayerController* PC = Cast<APlayerController>(Cast<APawn>(GetOwner())->GetController()))
    {
        SetCameraMultiplier(PC, 1.0f);
    }
}

void UAimAssistComponent::CheckForSoftLock()
{
    APlayerController* PC = Cast<APlayerController>(Cast<APawn>(GetOwner())->GetController());
    if (PC)
    {
        FVector CameraLocation;
        FRotator CameraRotation;
        PC->GetPlayerViewPoint(CameraLocation, CameraRotation);
        FVector ViewDirection = CameraRotation.Vector();

        TArray<FHitResult> Hits = PerformSphereTrace(SoftLockRadius, SoftLockLength, bShowDebugSoftLock);
        TArray<AGenericCharacter*> Enemies = FilterUniqueEnemies(Hits);

        bShouldSlowAiming = false;
        float NewMultiplier = 1.0f;

        for (AGenericCharacter* Enemy : Enemies)
        {
            if (Enemy && !Enemy->bIsDead)
            {
                FVector ToEnemy = (Enemy->GetActorLocation() + TargetOffset - CameraLocation).GetSafeNormal();
                float Dot = FVector::DotProduct(ViewDirection, ToEnemy);
                float Angle = FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(Dot, -1.0f, 1.0f)));

                if (Angle <= SoftLockAngle)
                {
                    bShouldSlowAiming = true;
                    NewMultiplier = SoftLockSlowMultiplier;
                    break;
                }
            }
        }
        SetCameraMultiplier(PC, NewMultiplier);
    }
}

void UAimAssistComponent::SetCameraMultiplier(APlayerController* PC, float NewMult)
{
    if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(PC->GetPawn()))
    {
        PlayerCharacter->SetCameraLookMultiplierAimAssist(NewMult);
    }
}
