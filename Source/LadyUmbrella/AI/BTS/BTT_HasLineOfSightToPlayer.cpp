// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_HasLineOfSightToPlayer.h"
#include <LadyUmbrella/AI/Controllers/AIControllerMafia.h>
#include <LadyUmbrella/Characters/Enemies/EnemyCharacter.h>
#include <Kismet/GameplayStatics.h>
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"


bool UBTT_HasLineOfSightToPlayer::CheckIfOutOfCover(APlayerCharacter* Player)
{
	return (Player->IsMovementControlledByCover() && Player->GetCoverMovementComponent()->GetInsideCoverState() ==
		EInsideCoverState::Aiming) || !(Player->IsMovementControlledByCover()) || !bNeedPlayerCoverState;
}

EBTNodeResult::Type UBTT_HasLineOfSightToPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AEnemyCharacter* Character = Cast<AEnemyCharacter>(OwnerComp.GetAIOwner()->GetPawn());

	if (!IsValid(Character))
	{
		return EBTNodeResult::Failed;
	}

	FVector StartEnemy;
	FRotator PlayerRotation;
	FVector PlayerEye;
	
	Character->GetController()->GetPlayerViewPoint(StartEnemy, PlayerRotation);
	
	StartEnemy.Z += StartTraceVerticalOffset;
	
	FHitResult OutHit;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Character);

	TArray<AActor*> AttachedActors;
	Character->GetAttachedActors(AttachedActors);

	for (AActor* Child : AttachedActors)
	{
		ActorsToIgnore.Add(Child);
	}
	
	APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (!IsValid(Player))
	{
		return EBTNodeResult::Failed;
	}
	
	if (CheckIfOutOfCover(Player))
	{
		Player->GetActorEyesViewPoint(PlayerEye, PlayerRotation);

		UKismetSystemLibrary::LineTraceSingle(GetWorld(), StartEnemy, PlayerEye, ETraceTypeQuery::TraceTypeQuery2, false, ActorsToIgnore, EDrawDebugTrace::None, OutHit, true, FColor::Emerald);
	
		if (OutHit.bBlockingHit)
		{
			if (OutHit.GetActor() == Player)
			{
				return EBTNodeResult::Succeeded;
			}
		}
	}
	else
	{
		FVector PlayerCoveredLocation = Player->GetActorLocation() - Player->GetActorUpVector() * 20;
		UKismetSystemLibrary::LineTraceSingle(GetWorld(), StartEnemy, PlayerCoveredLocation, ETraceTypeQuery::TraceTypeQuery2, false, ActorsToIgnore, EDrawDebugTrace::None, OutHit, true, FColor::Blue);
	
		if (OutHit.bBlockingHit)
		{
			if (OutHit.GetActor() == Player)
			{
				return EBTNodeResult::Succeeded;
			}
		}
	}
	
	return EBTNodeResult::Failed;
}
