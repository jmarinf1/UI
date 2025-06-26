// Fill out your copyright notice in the Description page of Project Settings.


#include "AIControllerGeneric.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "LadyUmbrella/AI/Managers/AISpawnPoint.h"
#include "LadyUmbrella/AI/Managers/ArenaManager.h"
#include "LadyUmbrella/Util/Logger.h"
#include "Navigation/CrowdFollowingComponent.h"

void AAIControllerGeneric::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (InPawn && behaviourTree)
	{
		OwnerPawn = InPawn;
		
		//Set blackboard
		UBlackboardComponent* BB;
		UseBlackboard(behaviourTree->BlackboardAsset, BB);
		Blackboard = BB;

		//Initialize keys
		// Blackboard->SetValueAsInt(TEXT("IndexNextWaypoint"), 0);
		// Blackboard->SetValueAsEnum(TEXT("States"),Patrol);
		//
		// Blackboard->SetValueAsFloat(TEXT("FleeRadius"),FleeRadius);
		// Blackboard->SetValueAsFloat(TEXT("CloseRadius"),CloseRadius);
		//
		// Blackboard->SetValueAsBool(TEXT("IsCovered"),false);
		//
		// Blackboard->SetValueAsBool("FleeingNextZone",false);	
		//Run Behaviour Tree
		RunBehaviorTree(behaviourTree);
		bShowDebugIcons = false;
	}
}

void AAIControllerGeneric::BeginPlay()
{
	Super::BeginPlay();

	UCrowdFollowingComponent* CrowdFollowingComponent = FindComponentByClass<UCrowdFollowingComponent>();
	if (CrowdFollowingComponent)
	{
		CrowdFollowingComponent->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::Good);
		CrowdFollowingComponent->SetCrowdSeparation(true);
		CrowdFollowingComponent->SetCrowdSeparationWeight(50.f);
		CrowdFollowingComponent->SetCrowdAvoidanceRangeMultiplier(1.1f);	
	}
}

AAIControllerGeneric::AAIControllerGeneric(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
	
}

void AAIControllerGeneric::InitializeController(AArenaManager* ArenaManager,
	const AAISpawnPoint& SpawnPoint)
{
	CurrentArenaManager = ArenaManager;
	SetZone(SpawnPoint.GetZone());
}

void AAIControllerGeneric::DetectPlayer(AActor* PlayerCharacter)
{
	Blackboard->SetValueAsObject(FName("Player"), PlayerCharacter);
}

void AAIControllerGeneric::SetZone(int Newzone)
{
	//Blackboard->SetValueAsInt(TEXT("CurrentZone"), Newzone);
	//CurrentArenaManager->ReduceEnemyInZone(CurrentZone);
	CurrentZone = Newzone;
	Blackboard->SetValueAsInt(TEXT("CurrentZone"), CurrentZone);
	CurrentArenaManager->AddEnemyZone(CurrentZone);
	// if (CurrentZone != 0)
	// {
	int Nextzone = CurrentZone + 1;
	Blackboard->SetValueAsInt(TEXT("NextZone"), Nextzone);
	//}
}

void AAIControllerGeneric::StopBehaviourTree()
{
	UnPossess();
}

AArenaManager* AAIControllerGeneric::GetArena() const
{
	return CurrentArenaManager;
}

bool AAIControllerGeneric::GetShowDebugIcon()
{
	return bShowDebugIcons;
}

