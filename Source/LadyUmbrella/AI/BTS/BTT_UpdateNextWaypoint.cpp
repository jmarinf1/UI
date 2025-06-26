// 
// BTT_UpdateNextWaypoint.cpp
// 
// Implementation of Task UpdateNextWaypoint: Action executed by a BehaviurTree.
// 
// Copyright Zulo Interactive. All Rights Reserved.
//



#include "BTT_UpdateNextWaypoint.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SplineComponent.h"
#include "LadyUmbrella/Characters/Enemies/EnemyCharacter.h"
#include "LadyUmbrella/AI/Controllers/AIControllerMafia.h"
#include "LadyUmbrella/AI/SplinePath.h"

EBTNodeResult::Type UBTT_UpdateNextWaypoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* bb = OwnerComp.GetBlackboardComponent();
	AEnemyCharacter* character = Cast<AEnemyCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	ASplinePath* spline = character->GetSplinePath();
	
	if (bb && character && spline)
	{
		//Get current index
		int32 currentIndex = bb->GetValueAsInt(TEXT("IndexNextWaypoint"));

		//get spline point
		FVector location = spline->Spline->GetLocationAtSplinePoint(currentIndex,ESplineCoordinateSpace::World);
		
		//Next waypoint
		currentIndex++;
		//Check if next waypoint is out of max length
		//if (currentIndex >= character->GetPathLength())
		if (currentIndex >= spline->Spline->GetNumberOfSplinePoints())
		{
			currentIndex = 0;
		}
		//Set index
		bb->SetValueAsInt(TEXT("IndexNextWaypoint"), currentIndex);
		//Set Location
		bb->SetValueAsVector(TEXT("LocationNextWaypoint"), location/*character->GetLocationbyIndex(currentIndex)*/);
		//return success
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
