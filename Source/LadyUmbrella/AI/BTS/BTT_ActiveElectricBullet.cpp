// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_ActiveElectricBullet.h"

#include "AIController.h"
#include "LadyUmbrella/Characters/Enemies/EnemyCharacter.h"
#include "LadyUmbrella/Weapons/GenericWeapon.h"

EBTNodeResult::Type UBTT_ActiveElectricBullet::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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
	
	AGenericWeapon* Weapon = Character->GetEquippedWeapon();
	if (!IsValid(Weapon))
	{
		return EBTNodeResult::Failed;
	}

	if (!Weapon->IsElectricBulletActivated())
	{
		Weapon->SetWeaponBulletType(EBulletType::Electric);
		Weapon->SetElectricBulletActivated(true);
		Character->Fire(); // Esto llamará OnElectricShoot() con el timer
	}

	return EBTNodeResult::Succeeded;
}

