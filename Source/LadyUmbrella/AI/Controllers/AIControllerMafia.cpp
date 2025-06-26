// 
// AIController_generic.cpp
// 
// Implementation of class AIController generic
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 


#include "AIControllerMafia.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "GameFramework/Pawn.h"   // Necesario para usar APawn
#include "LadyUmbrella/AI/Components/EnemyZonesComponent.h"
#include "LadyUmbrella/AI/Managers/AISpawnPoint.h"
#include "LadyUmbrella/AI/Managers/ArenaManager.h"
#include "LadyUmbrella/Util/Logger.h"


AAIControllerMafia::AAIControllerMafia(FObjectInitializer const& ObjectInitializer)
{
	//SetupPerceptionSystem();
	FleeRadius = 1000;
	CloseRadius = 400;
}

// void AAIControllerMafia::InitializeCoordination()
// {
// 	CurrentArenaManager->
// }

// int AAIControllerMafia::GetFreeCovers(int zone)
// {
// 	// if (CurrentArenaManager->GetZonesPool()->Find(zone))
// 	// {
// 	// 	return CurrentArenaManager->GetZonesPool()->Find(zone)->FreeCovers;
// 	// }
// 	return -1;
// }

// void AAIControllerMafia::SetZone(const int NewZone)
// {
// 	CurrentArenaManager->ReduceEnemyInZone(CurrentZone);
// 	CurrentZone = NewZone;
// 	Blackboard->SetValueAsInt(TEXT("CurrentZone"), CurrentZone);
// 	CurrentArenaManager->AddEnemyZone(CurrentZone);
//
// 	int Nextzone = CurrentZone + 1;
// 	Blackboard->SetValueAsInt(TEXT("NextZone"), Nextzone);
//
// }

void AAIControllerMafia::OnPossess(APawn* InPawn)
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
		Blackboard->SetValueAsInt(TEXT("IndexNextWaypoint"), 0);
		Blackboard->SetValueAsEnum(TEXT("States"),Patrol);

		Blackboard->SetValueAsFloat(TEXT("FleeRadius"),FleeRadius);
		Blackboard->SetValueAsFloat(TEXT("CloseRadius"),CloseRadius);

		Blackboard->SetValueAsBool(TEXT("IsCovered"),false);

		Blackboard->SetValueAsBool("FleeingNextZone",false);	
		//Run Behaviour Tree
		RunBehaviorTree(behaviourTree);
	}
}

void AAIControllerMafia::BeginPlay()
{
	Super::BeginPlay();
}

void AAIControllerMafia::OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus)
{
	if (Actor->ActorHasTag("Player"))
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			DetectPlayer(Actor);
		}
	}
}

void AAIControllerMafia::DetectPlayer(AActor* PlayerCharacter)
{
	Super::DetectPlayer(PlayerCharacter);
	Blackboard->SetValueAsEnum(TEXT("States"), Attack);
}

void AAIControllerMafia::InitializeController(AArenaManager* ArenaManager,
	const AAISpawnPoint& SpawnPoint)
{
	Super::InitializeController(ArenaManager, SpawnPoint);
	if (SpawnPoint.EnterArenaGroup != -1 || SpawnPoint.bIsReinforcement)
	{
		Blackboard->SetValueAsEnum(TEXT("States"),EnterArena);
	}
}
