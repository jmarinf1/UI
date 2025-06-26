// Fill out your copyright notice in the Description page of Project Settings.


#include "AIControllerShieldAgency.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "LadyUmbrella/Characters/Enemies/Agency/ShieldAgencyCharacter.h"
#include "LadyUmbrella/Weapons/AgencyShield/AgencyShield.h"


void AAIControllerShieldAgency::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	RadiusStop = 650.f;
	Blackboard->SetValueAsBool(TEXT("IsShieldBroken"),false);
	
	// if (AShieldAgencyCharacter* MyCharacter = Cast<AShieldAgencyCharacter>(InPawn))
	// {
	// 	MyCharacter->OnShieldDestroyed.AddDynamic(this, &AAIControllerShieldAgency::OnShieldDestroyedHandler);
	// }
}

void AAIControllerShieldAgency::InitializeController(AArenaManager* ArenaManager,
	const AAISpawnPoint& SpawnPoint)
{
	Super::InitializeController(ArenaManager, SpawnPoint);

	// if (Blackboard)
	// {
	// 	Blackboard->SetValueAsFloat(FName("RadiusStop"), RadiusStop);
	// 	Blackboard->SetValueAsObject(FName("ShieldAgent"), ControlledPawn);
	// }
}

void AAIControllerShieldAgency::NotifyShieldIsBroken() const
{
	Blackboard->SetValueAsBool(TEXT("IsShieldBroken"),true);
}
