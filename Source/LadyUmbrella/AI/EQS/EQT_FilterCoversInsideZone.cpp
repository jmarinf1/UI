// Fill out your copyright notice in the Description page of Project Settings.


#include "EQT_FilterCoversInsideZone.h"

#include "AIController.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_ActorBase.h"
#include "LadyUmbrella/Environment/CoverSystem/AICoverEntryPoint.h"
#include "LadyUmbrella/Environment/CoverSystem/CoverSpline.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "LadyUmbrella/Util/Logger.h"

UEQT_FilterCoversInsideZone::UEQT_FilterCoversInsideZone()
{
	TestComment = "if InsideZone = true -> test filter all covers outside this zone or if InsideZone = false -> test filter all covers inside this cover";
	//constant score and filtertype set as match
	SetWorkOnFloatValues(false);
	//test purpose
	TestPurpose = EEnvTestPurpose::FilterAndScore;
	//item type
	ValidItemType = UEnvQueryItemType_ActorBase::StaticClass();
}

void UEQT_FilterCoversInsideZone::RunTest(FEnvQueryInstance& QueryInstance) const
{
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
	//set bool test
	BoolValue.BindData(QueryInstance.Owner.Get(), QueryInstance.QueryID);
	const bool bWantinside = BoolValue.GetValue();
	//get ai controller
	AAIController* Controller = Cast<AAIController>(PawnOwner->GetController());
	if (!Controller)
	{
		return;
	}
	//get blackboard
	UBlackboardComponent* BlackBoard = Controller->GetBlackboardComponent();
	if (!IsValid(BlackBoard))
	{
		return;
	}
	//get current zone of enemy is located
	int Zone;
	if (BlackBoard->GetValueAsBool("FleeingNextZone"))
	{
		Zone = BlackBoard->GetValueAsInt(TEXT("NextZone"));
	}
	else
	{
		Zone = BlackBoard->GetValueAsInt(TEXT("CurrentZone"));
	}
	BlackBoard->SetValueAsBool("FleeingNextZone",false);	
	for (FEnvQueryInstance::ItemIterator It(this, QueryInstance); It; ++It)
	{
		if (AActor* actor = GetItemActor(QueryInstance, It.GetIndex()))
		{
			bool binside = false;

			AAICoverEntryPoint* CurrentCover = Cast<AAICoverEntryPoint>(actor);
			int CurrentCoverZone = CurrentCover->GetOwningSpline()->GetZone();
			if (insideZone)
			{ //include covers inside zone
				if (CurrentCoverZone == Zone)
				{
					binside = true;
				}
			}
			else
			{ //include covers outside zone
				if (CurrentCoverZone != Zone)
				{
					if (!ZonesSmallerthanCurrentZone)
					{
						binside = true;
					}
					else if (CurrentCoverZone < Zone)
					{
						binside = true;
					}
				}
			}
			//Set score
			It.SetScore(TestPurpose, FilterType, binside, bWantinside);
		}
	}
}
