// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_IsElectricProjectileShooted.h"

#include "AIController.h"
#include "LadyUmbrella/Characters/Enemies/EnemyCharacter.h"
#include "LadyUmbrella/Util/Logger.h"
#include "LadyUmbrella/Weapons/GenericWeapon.h"


UBTT_IsElectricProjectileShooted::UBTT_IsElectricProjectileShooted()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UBTT_IsElectricProjectileShooted::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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
	
	AEnemyCharacter* Character = Cast<AEnemyCharacter>(Pawn);
	if (!IsValid(Character))
	{
		return EBTNodeResult::Failed;
	}
	
	Weapon = Character->GetEquippedWeapon();
	if (!IsValid(Weapon))
	{
		return EBTNodeResult::Failed;
	}
	
	return EBTNodeResult::InProgress;
}

void UBTT_IsElectricProjectileShooted::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	
	if (Weapon->IsElectricBulletShooted())
	{
		Weapon->SetElectricBulletShooted(false);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

