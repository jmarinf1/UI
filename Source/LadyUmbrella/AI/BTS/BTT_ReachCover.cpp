// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_ReachCover.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "LadyUmbrella/Characters/GenericCharacter.h"

UBTT_ReachCover::UBTT_ReachCover()
{
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTT_ReachCover::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = Cast<AAIController>(OwnerComp.GetAIOwner());
	UBlackboardComponent* BB = Controller->GetBlackboardComponent();
	if (Controller)
	{
		auto GenericCharacter = Cast<AGenericCharacter>(Controller->GetPawn());
		GenericCharacter->NotifyReachedCoverTravellingTowards();
		BB->SetValueAsBool(TEXT("IsCovered"),true);	
		// if (BB)
		// {
		// 	if (BB->GetValueAsBool(TEXT("IsFleeing")))
		// 	{
		// 		//update currentzone and nextzone
		// 		int Currentzone = BB->GetValueAsInt(TEXT("CurrentZone"));
		// 		int NextZone = BB->GetValueAsInt(TEXT("NextZone"));
		// 		//BB->SetValueAsInt(TEXT("NextZone"),++Currentzone);
		// 		//BB->SetValueAsInt(TEXT("CurrentZone"),NextZone);
		// 	}
		// }
	}
	return EBTNodeResult::Succeeded;
}
