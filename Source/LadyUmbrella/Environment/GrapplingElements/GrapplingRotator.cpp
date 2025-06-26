// Fill out your copyright notice in the Description page of Project Settings.


#include "GrapplingRotator.h"

#include "HookImpactPoint.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

AGrapplingRotator::AGrapplingRotator()
{
	
	SecondHitPoint = CreateDefaultSubobject<UBoxComponent>("Second Hit Point");
	SecondHitPoint->SetupAttachment(RootComponent);
	SecondHitPoint->AddRelativeLocation(FVector( 0, 350, 55));
	SecondHitPoint->SetBoxExtent(FVector(60, 60, 60 ));
	
	BoxComponent->AddRelativeLocation(FVector(1, -360, 55));
	BoxComponent->SetBoxExtent(FVector(50, 50, 50));

	ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshFinder(TEXT("/Script/Engine.StaticMesh'/Game/Blueprints/LevelElements/PlaceholderMeshes/Primitives/SM_Cube.SM_Cube'"));
	if(StaticMeshFinder.Succeeded() && IsValid(StaticMeshComponent))
	{
		StaticMeshComponent->SetStaticMesh(StaticMeshFinder.Object);
	}
	
	StaticMeshComponent->SetRelativeScale3D(FVector(1, 8, 1));
	StaticMeshComponent->SetRelativeLocation(FVector(-50, -400, 0));
	StaticMeshComponent->SetupAttachment(SceneRootComponent);

	PhysicalCollision = CreateDefaultSubobject<UBoxComponent>("Physical Collision");
	PhysicalCollision->SetRelativeLocation(FVector(0, 0, 50));
	PhysicalCollision->SetupAttachment(SceneRootComponent);
	PhysicalCollision->SetBoxExtent(FVector(50, 400, 50));
		
	static ConstructorHelpers::FClassFinder<AHookImpactPoint> HookImpactPointSubclassFinder(TEXT("/Script/Engine.Blueprint'/Game/Blueprints/LevelElements/Puzzle/HookableElements/BP_HookImpactPoint.BP_HookImpactPoint_C'"));
	if (HookImpactPointSubclassFinder.Succeeded())
	{
		HookImpactPointSubClass = HookImpactPointSubclassFinder.Class;
	}
	static ConstructorHelpers::FObjectFinder<UCurveFloat> CurveAsset(TEXT("/Script/LadyUmbrella.RotationCurveFloat'/Game/DataStructures/Curves/Camera/C_Rotation.C_Rotation'"));
	if (CurveAsset.Succeeded())
	{
		RotationCurve= CurveAsset.Object;
	}
}

void AGrapplingRotator::Destroyed()
{
	Super::Destroyed();
	
	for (const auto ImpactPoint : ImpactPoints)
	{
		ImpactPoint->OnHookImpactDynamic.RemoveAll(this);
		ImpactPoint->OnHookImpactDynamic.Clear();
		ImpactPoint->Destroy();
	}
}

void AGrapplingRotator::RotateFromChildDynamic(const float DegreesOfRotationChild, const float SpeedOfRotationChild)
{
	if (!RotationCurve || DegreesOfRotationChild <= 0.f)
	{
		return;
	}
	
	GetWorld()->GetTimerManager().ClearTimer(RotationTimer);

	const FRotator CurrentRelRot = GetRootComponent()->GetRelativeRotation();
	InitialQuat = CurrentRelRot.Quaternion();

	const FRotator TargetRelRot = FRotator(CurrentRelRot.Pitch, DegreesOfRotationChild, CurrentRelRot.Roll);
	TargetQuat = TargetRelRot.Quaternion();

	const float AngleDelta = InitialQuat.AngularDistance(TargetQuat) * ( 180.f / PI );

	RotationDuration = AngleDelta / SpeedOfRotationChild;
	RotationElapsed = 0.f;

	GetWorld()->GetTimerManager().SetTimer(
		RotationTimer,
		this,
		&AGrapplingRotator::RotateActorDynamic,
		0.01f,
		true
	);

}

void AGrapplingRotator::RotateActorDynamic()
{
	constexpr float DeltaTime = 0.01f;
	RotationElapsed += DeltaTime;

	const float Alpha = FMath::Clamp(RotationElapsed / RotationDuration, 0.f, 1.f);
	const float CurveValue = RotationCurve ? RotationCurve->GetFloatValue(Alpha) : Alpha;

	const FQuat InterpolatedQuat = FQuat::Slerp(InitialQuat, TargetQuat, CurveValue);
	GetRootComponent()->SetRelativeRotation(InterpolatedQuat);

	if (Alpha >= 1.f)
	{
		GetWorld()->GetTimerManager().ClearTimer(RotationTimer);
	}
}

void AGrapplingRotator::AddNewImpactPoint()
{
	FActorSpawnParameters Params;
	AHookImpactPoint* NewImpactPoint = GetWorld()->SpawnActor<AHookImpactPoint>(HookImpactPointSubClass);

	if (IsValid(NewImpactPoint) && NewImpactPoint->OnHookImpactDynamic.IsBound() == false)
	{
		ImpactPoints.Add(NewImpactPoint);
		NewImpactPoint->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		NewImpactPoint->OnHookImpactDynamic.AddDynamic(this, &ThisClass::RotateFromChildDynamic);

		#if WITH_EDITOR
			if (GEditor)
			{
				GEditor->SelectNone(false, true);
				GEditor->SelectActor(NewImpactPoint, true, true);
				GEditor->NoteSelectionChange();
			}
		#endif	
	}
}

IHookInterface::GrabType AGrapplingRotator::GetGrabType()
{
	return PULLWITHCOMPONENT;
}