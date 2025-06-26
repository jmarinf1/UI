// Fill out your copyright notice in the Description page of Project Settings.


#include "AIControllerRegularAgency.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "LadyUmbrella/AI/Managers/ArenaManager.h"

void AAIControllerRegularAgency::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	//biter data
	Blackboard->SetValueAsFloat(TEXT("CloseRadius"),CloseRadius);
	Blackboard->SetValueAsFloat(TEXT("AdvanceRadius"),AdvanceRadius);
	//flanker data
	Blackboard->SetValueAsEnum(FName("StatesFlank"), SearchFlankCover);
	Blackboard->SetValueAsFloat(TEXT("ShootRadius"),ShootRadius);

}

void AAIControllerRegularAgency::InitializeController(AArenaManager* ArenaManager,
	const AAISpawnPoint& SpawnPoint)
{
	Super::InitializeController(ArenaManager, SpawnPoint);
	Blackboard->SetValueAsInt("CurrentZone", ArenaManager->GetHighestZone());
}

void AAIControllerRegularAgency::SetZone(int Newzone)
{
	Super::SetZone(Newzone);
}

void AAIControllerRegularAgency::AddCoverAsSelected(AAICoverEntryPoint* cover)
{
	CoversSelected.Add(cover);
}
