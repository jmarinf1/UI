// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_CanAdvanveNextZone.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "LadyUmbrella/Characters/Enemies/Agency/AgencyCharacter.h"
#include "LadyUmbrella/Util/Logger.h"

EBTNodeResult::Type UBTT_CanAdvanveNextZone::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = Cast<AAIController>(OwnerComp.GetAIOwner());
	if (!IsValid(Controller))
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* Blackboard = Controller->GetBlackboardComponent();
	if (!Blackboard)
	{
		return EBTNodeResult::Failed;
	}

	AAgencyCharacter* AgencyCharacter = Cast<AAgencyCharacter>(Controller->GetPawn());
	int CurrentRemainingRagagues = AgencyCharacter->GetRemainingRafagues();
	
	AActor* PlayerCharacter = Cast<AActor>(Blackboard->GetValueAsObject(PlayerKey.SelectedKeyName));
	if (!IsValid(PlayerCharacter))
	{
		return EBTNodeResult::Failed;
	}
	
	FVector PlayerLocation = PlayerCharacter->GetActorLocation();
	FVector EnemyLocation = Controller->GetPawn()->GetActorLocation();
	
	FVector EnemytoPlayer = PlayerLocation - EnemyLocation;
	FVector PlayerFordward = PlayerCharacter->GetActorForwardVector();

	float DotValue = FVector::DotProduct(EnemytoPlayer.GetSafeNormal(), PlayerFordward.GetSafeNormal());
	
	float Distance = FVector::Distance(PlayerLocation, EnemyLocation);

	//esto dejarlos en if con return fail
	//y comprobar que no resta la zona cuando no debe
	if (Distance > Blackboard->GetValueAsFloat(AdvanceRadiusKey.SelectedKeyName))
	{
		if (CurrentRemainingRagagues == 0)
		{
			int CurrentZone = Blackboard->GetValueAsInt(CurrentZoneKey.SelectedKeyName);
			int NextZone = CurrentZone - 1;
			
			if (DotValue < 0 && NextZone >= 0)
			{
				AgencyCharacter->SetRandomNumRafagues();
				//go to advance
				return EBTNodeResult::Failed;
			}	
		}
	}

	return EBTNodeResult::Succeeded;
}
