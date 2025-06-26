// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_CheckIfCloseCombat.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Util/Logger.h"

UBTService_CheckIfCloseCombat::UBTService_CheckIfCloseCombat()
{
	bNotifyBecomeRelevant = true;
}

void UBTService_CheckIfCloseCombat::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	if (IsValid(Player))
	{
		if (FVector::Distance(Pawn->GetActorLocation(), Player->GetActorLocation()) < Blackboard->GetValueAsFloat("CloseRadius"))
		{
			Blackboard->SetValueAsBool("IsCloseCombat", true);
		}
	}
}

void UBTService_CheckIfCloseCombat::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	Blackboard = OwnerComp.GetBlackboardComponent();
	AAIController* Controller = OwnerComp.GetAIOwner();

	if (!IsValid(Blackboard) || !IsValid(Controller))
	{
		return;
	}

	Pawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!IsValid(Pawn))
	{
		return;
	}

	Player = Cast<APlayerCharacter>(Blackboard->GetValueAsObject("Player"));
	
}
