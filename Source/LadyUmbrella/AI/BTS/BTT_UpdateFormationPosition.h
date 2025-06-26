// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_UpdateFormationPosition.generated.h"

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UBTT_UpdateFormationPosition : public UBTTaskNode
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<USceneComponent> ComponentClass;
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
