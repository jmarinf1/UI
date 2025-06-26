// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_ThrowGrenade.h"
#include <LadyUmbrella/AI/Controllers/AIControllerMafia.h>
#include <LadyUmbrella/Characters/Enemies/Mafia/MafiaCharacter.h>

#include "LadyUmbrella/Util/Logger.h"

EBTNodeResult::Type UBTT_ThrowGrenade::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AMafiaCharacter* character = Cast<AMafiaCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	
	if (!character)
	{
		return EBTNodeResult::Failed;
	}
	
	if (character->GetHasGrenadeToken())
	{
		character->ThrowGrenade();

		character->SetHasGrenadeToken(false);

		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
