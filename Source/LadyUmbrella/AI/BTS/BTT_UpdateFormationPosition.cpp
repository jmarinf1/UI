// Fill out your copyright notice in the Description page of Project Settings.

////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
/// DEPRECATED - DELETEARRRRR
////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
#include "BTT_UpdateFormationPosition.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "LadyUmbrella/Characters/Enemies/Agency/AgencyCharacter.h"

EBTNodeResult::Type UBTT_UpdateFormationPosition::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (Blackboard)
	{
		FName ComponentTag;
		Blackboard->GetValueAsBool(TEXT("IsLeader")) ? ComponentTag = "PosLineal1" : ComponentTag = "PosLineal2";
		
		if (AAgencyCharacter* ShieldAgent = Cast<AAgencyCharacter>(Blackboard->GetValueAsObject(TEXT("ShieldAgent"))))
		{
			TArray<USceneComponent*> Components;
			ShieldAgent->GetComponents<USceneComponent>(Components);

			for (USceneComponent* Comp : Components)
			{
				if (Comp->ComponentHasTag(ComponentTag))
				{
					// Aquí tienes el componente con ese tag
					USceneComponent* TargetComponent = Comp;
					FVector Location = TargetComponent->GetComponentTransform().GetLocation();
					Blackboard->SetValueAsVector(FName("LocationFormation"), Location);
					return EBTNodeResult::Succeeded;
				}
			}
		}
	}
	return EBTNodeResult::Failed;
}
