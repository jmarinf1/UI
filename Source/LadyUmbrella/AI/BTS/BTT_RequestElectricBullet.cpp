// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_RequestElectricBullet.h"

#include "AIController.h"
#include "LadyUmbrella/Characters/Enemies/Agency/AgencyCharacter.h"
#include "LadyUmbrella/Util/Logger.h"


EBTNodeResult::Type UBTT_RequestElectricBullet::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (!IsValid(Controller))
	{
		return EBTNodeResult::Failed;
	}

	APawn* Pawn = Controller->GetPawn();
	if (!IsValid(Pawn))
	{
		return EBTNodeResult::Failed;
	}

	AAgencyCharacter* Character = Cast<AAgencyCharacter>(Pawn);
	if (!IsValid(Character))
	{
		return EBTNodeResult::Failed;
	}
	
	Character->OnElectricBulletTokenRequest.Broadcast(bIsRequesting, Character);

	if (Character->GetHasElectricBullet())
	{
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
