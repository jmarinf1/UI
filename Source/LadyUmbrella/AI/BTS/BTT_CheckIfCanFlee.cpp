// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_CheckIfCanFlee.h"

#include "LadyUmbrella/AI/IAConstants.h"
#include "LadyUmbrella/AI/Controllers/AIControllerMafia.h"
#include "LadyUmbrella/Util/Logger.h"
#include "LadyUmbrella/AI/Managers/ArenaManager.h"

UBTT_CheckIfCanFlee::UBTT_CheckIfCanFlee()
{
	// if (bShootUntilRunOutAllBullets)
	// {
	bNotifyTick = true;
	bCreateNodeInstance = true;
	//}
}

EBTNodeResult::Type UBTT_CheckIfCanFlee::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIControllerMafia* Controller = Cast<AAIControllerMafia>(OwnerComp.GetAIOwner());
	if (!IsValid(Controller))
	{
		return EBTNodeResult::Failed;
	}
	//FLogger::DebugLog("Check if Can flee");
	int CurrentZone = Controller->GetZone();
	if (Controller->GetArena()->GetFleeQueue()->Contains(CurrentZone))
	{
		// if (Controller->GetArena()->GetEnemiesInZone(CurrentZone) <= 1)
		// {
		// 	FLogger::DebugLog("ESTOY SOLO EN MI ZONA: %d", CurrentZone);
		// 	return EBTNodeResult::Failed;
		// }
		
		const int32 IDEnemy = Controller->GetArena()->GetFleeQueue()->Find(CurrentZone)->EnemiesIDs[0];
		int EnemiesInZone = Controller->GetArena()->GetEnemiesInZone(CurrentZone);
		if (EnemiesInZone <= IAConstants::MIN_NUMBER_OF_ENEMIES_IN_ZONE)
		{
			return EBTNodeResult::Failed;
		}
		if (IDEnemy == Controller->GetPawn()->GetUniqueID())
		{
			Controller->GetArena()->ReduceEnemyInZone(Controller->GetZone());
			return EBTNodeResult::Succeeded;
		}
	}
	return EBTNodeResult::Failed;
}
