// 
// AIControllerAgency.h
// 
// Specific AIController for Agency Agents
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 


#pragma once

#include "CoreMinimal.h"
#include "AIControllerGeneric.h"
#include "AIControllerFlankAgency.generated.h"


/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API AAIControllerFlankAgency : public AAIControllerGeneric
{
	GENERATED_BODY()
	
public:
	virtual void OnPossess(APawn* InPawn) override;
};
