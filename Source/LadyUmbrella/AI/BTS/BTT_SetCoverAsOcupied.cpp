// 
// BTT_SetCoverAsOcupied.cpp
// 
// Task executed by Behaviour Tree : SetCoverAsOcupied
// 
// Copyright Zulo Interactive. All Rights Reserved.
//


#include "BTT_SetCoverAsOcupied.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "LadyUmbrella/AI/Controllers/AIControllerMafia.h"
#include "LadyUmbrella/AI/Managers/ArenaManager.h"
#include "LadyUmbrella/Characters/GenericCharacter.h"
#include "LadyUmbrella/Characters/Enemies/Agency/AgencyCharacter.h"
#include "LadyUmbrella/Environment/CoverSystem/AICoverEntryPoint.h"

EBTNodeResult::Type UBTT_SetCoverAsOcupied::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIControllerGeneric* Controller = Cast<AAIControllerGeneric>(OwnerComp.GetAIOwner());

	if (!IsValid(Controller)) return EBTNodeResult::Failed;
	
	UBlackboardComponent* Blackboard = Controller->GetBlackboardComponent();

	if (!IsValid(Blackboard)) return EBTNodeResult::Failed;
	
	AAICoverEntryPoint* SelectedCoverEntryPoint =  Cast<AAICoverEntryPoint>(Blackboard->GetValueAsObject(cover.SelectedKeyName));
	if (SelectedCoverEntryPoint == nullptr) return EBTNodeResult::Succeeded;
	
	//notify cover that enter to it
	AGenericCharacter* GenericCharacter = Cast<AGenericCharacter>(Controller->GetPawn());
	
	if (!IsValid(GenericCharacter)) return EBTNodeResult::Failed;
	
	if (ocupied)
	{
		GenericCharacter->NotifyBeginTravellingToCover(SelectedCoverEntryPoint->GetOwningSpline(), SelectedCoverEntryPoint->GetSection());
	}
	else //notify cover that exit it
	{
		GenericCharacter->NotifyExitCover();
		Blackboard->SetValueAsBool(TEXT("IsCovered"),false);
	}
	
	return EBTNodeResult::Succeeded;
}
