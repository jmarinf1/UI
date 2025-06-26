// 
// GrapplingObject.cpp
// 
// Implementation of the GrapplingObject class. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
//
#include "GrapplingObject.h"
#include "CableComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "LadyUmbrella/Weapons/Umbrella/Umbrella.h"
#include "LadyUmbrella/Util/SaveSystem/LU_GameInstance.h"

AGrapplingObject::AGrapplingObject()
{
	PrimaryActorTick.bCanEverTick = true;
	BoxComponent			  = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Box"));
	FallingDirection		  = CreateDefaultSubobject<UArrowComponent>(TEXT("End Falling Location"));
	SceneRootComponent		  = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Root"));
	SkeletalMeshComponent	  = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh"));
	StaticMeshComponent		  = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	bIsSolved				  = false;

	
	BoxComponent->SetBoxExtent(FVector(200, 200, 200));
	
	// Setup Hierarchy
	SetRootComponent(SceneRootComponent);
	StaticMeshComponent->SetupAttachment(SceneRootComponent);
	BoxComponent->SetupAttachment(SceneRootComponent);
	FallingDirection->SetupAttachment(SceneRootComponent);
	SkeletalMeshComponent->SetupAttachment(SceneRootComponent);

}

void AGrapplingObject::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

// Called when the game starts or when spawned
void AGrapplingObject::BeginPlay()
{
	Super::BeginPlay();
}

void AGrapplingObject::ContactWithCable(FVector& Direction)
{
	if (!bIsSolved)
	{
		if (IsValid(SkeletalMeshComponent))
		{
			SkeletalMeshComponent->PlayAnimation(SkeletalMeshComponent->AnimationData.AnimToPlay, false);
			bIsSolved = true;	
			SolvedTransform = GetActorTransform();

			TArray<AActor*> FoundActors;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), AUmbrella::StaticClass(), FoundActors);
			AUmbrella* Umbrella = Cast<AUmbrella>(FoundActors[0]);
	
			if (IsValid(Umbrella))
			{
				//FLogger::DebugLog("Setting cable positions");
				Umbrella->SetCableEndLocation( UKismetMathLibrary::InverseTransformLocation(Umbrella->GetActorTransform(), GetActorLocation()) );
				Umbrella->GetCableComponent()->SetVisibility(true);
				Umbrella->GetCableComponent()->SetComponentTickEnabled(true);
				Umbrella->GetCableComponent()->EndLocation = FVector::ZeroVector;
				Umbrella->GetCableComponent()->SetVisibility(false);
				Umbrella->GetCableComponent()->SetComponentTickEnabled(false);
			}	
		}
	}
}

IHookInterface::GrabType AGrapplingObject::GetGrabType()
{
	return PULL;
}

void AGrapplingObject::LoadGame()
{
	if (const ULU_GameInstance* GameInstance = Cast<ULU_GameInstance>(GetGameInstance()))
	{
		bIsSolved = GameInstance->GI_SavePuzzle.bIsSolved;
		//SetActorTransform(GameInstance->GI_SavePuzzle.WorldTransform);
	}
}

void AGrapplingObject::SaveGame()
{
	if (const ULU_GameInstance* GameInstance = Cast<ULU_GameInstance>(GetGameInstance()))
	{
		bIsSolved = GameInstance->GI_SavePuzzle.bIsSolved;
		//SetActorTransform(GameInstance->GI_SavePuzzle.WorldTransform);
	}
}

void AGrapplingObject::SaveDataCheckPoint()
{
	if (ULU_GameInstance* GameInstance = Cast<ULU_GameInstance>(GetGameInstance()))
	{
		GameInstance->GI_SavePuzzle.bIsSolved	   = bIsSolved;
		GameInstance->GI_SavePuzzle.WorldTransform = GetActorTransform();
	}
}

void AGrapplingObject::LoadDataCheckPoint()
{
	if (const ULU_GameInstance* GameInstance = Cast<ULU_GameInstance>(GetGameInstance()))
	{
		bIsSolved = GameInstance->GI_SavePuzzle.bIsSolved;
	//	SetActorTransform(GameInstance->GI_SavePuzzle.WorldTransform);
	}
}
