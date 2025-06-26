// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_NeedReload.h"
#include <LadyUmbrella/AI/Controllers/AIControllerMafia.h>
#include <LadyUmbrella/Characters/Enemies/EnemyCharacter.h>
#include <LadyUmbrella/Weapons/GenericWeapon.h>
#include <LadyUmbrella/Util/Logger.h>

EBTNodeResult::Type UBTT_NeedReload::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AEnemyCharacter* character = Cast<AEnemyCharacter>(OwnerComp.GetAIOwner()->GetPawn());

	if (!character)
	{
		return EBTNodeResult::Failed;
	}

	if (!character->GetEquippedWeapon())
	{
		return EBTNodeResult::Failed;
	}

	if (character->GetEquippedWeapon()->GetAmmoCurrent() <= 0 && !character->GetIsReloading())
	{
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}