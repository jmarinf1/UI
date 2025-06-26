// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_DotProductEnemyPlayer.generated.h"

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UBTT_DotProductEnemyPlayer : public UBTTaskNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector PlayerKey;
	float const MinDotValue = -0.5;
	float const MaxDotValue = 0.5;
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
