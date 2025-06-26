// Fill out your copyright notice in the Description page of Project Settings.


#include "EQT_FilterCoversInFlankPath.h"
#include "EQT_FilterCoversAlreadySelected.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_ActorBase.h"
#include "LadyUmbrella/AI/Controllers/AIControllerRegularAgency.h"
#include "LadyUmbrella/Environment/CoverSystem/AICoverEntryPoint.h"
#include "LadyUmbrella/Environment/CoverSystem/CoverSpline.h"

UEQT_FilterCoversInFlankPath::UEQT_FilterCoversInFlankPath()
{
	TestComment = "Filter Covers already selected in pathfinding to BestFlackCover";
	//constant score and filtertype set as match
	SetWorkOnFloatValues(false);
	//test purpose
	TestPurpose = EEnvTestPurpose::FilterAndScore;
	//item type
	ValidItemType = UEnvQueryItemType_ActorBase::StaticClass();
}

void UEQT_FilterCoversInFlankPath::RunTest(FEnvQueryInstance& QueryInstance) const
{
	//Super::RunTest(QueryInstance);
	UObject* Owner = QueryInstance.Owner.Get();

	if (!Owner)
	{
		return;
	}
	// get pawn
	APawn* PawnOwner = Cast<APawn>(Owner);
	if (!PawnOwner)
	{
		return;
	}

	// get aicontroller
	AAIControllerRegularAgency* Controller = Cast<AAIControllerRegularAgency>(PawnOwner->GetController());
	if (!Controller)
	{
		return;
	}
	
	UBlackboardComponent* Blacboard = Controller->GetBlackboardComponent();
	if (!Blacboard)
	{
		return;
	}

	BoolValue.BindData(QueryInstance.Owner.Get(), QueryInstance.QueryID);
	const bool bwantinside = BoolValue.GetValue();

	AAICoverEntryPoint* CurrentGoal = Cast<AAICoverEntryPoint>(Blacboard->GetValueAsObject(("GoalCover")));
	if (!CurrentGoal) return;

	//AActor* CurrentGoalParent = CurrentGoal->GetParentActor();
	AActor* GoalCoverSpline = CurrentGoal->GetOwningSpline()->GetOwner();
	
	TArray<AAICoverEntryPoint*> CoversInPath = Controller->GetCoversSelected();
	
	for (FEnvQueryInstance::ItemIterator It(this, QueryInstance); It; ++It)
	{
		if (AActor* actor = GetItemActor(QueryInstance, It.GetIndex()))
		{
			bool binside = false;
			bool bCoverRegistered = false;
			
			AAICoverEntryPoint* CurrentCover = Cast<AAICoverEntryPoint>(actor);
			AActor* CurrentCoverOwner = CurrentCover->GetOwningSpline()->GetOwner();

			
			for (AAICoverEntryPoint* Cover : CoversInPath)
			{
				if (Cover->GetOwningSpline()->GetOwner() == CurrentCoverOwner)
				{
					bCoverRegistered = true;
					break;
				}
			}
			
			if (!bCoverRegistered)
			{
				//only covers not contained in path to flank cover are filtered
				//if (!Controller->getCoversSelected().Contains(CurrentCover))
				//{ 
					binside = true;
				//}
			}
			

			//Set score
			It.SetScore(TestPurpose, FilterType, binside, bwantinside);
		}
	}
}
