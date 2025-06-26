// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LadyUmbrella/Items/InteractuableObjects/GenericInteractable.h"
#include "GenericPickable.generated.h"

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API AGenericPickable : public AGenericInteractable
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	int AmountToPickup;
	
public:
	virtual int32 Interacting() override;
	virtual int32 Interacting(APlayerCharacter* Interactor) override;
	virtual TYPE_OF_INTERACTION GetTypeOfInteraction() const override { return NO_PARAMS; };

	FORCEINLINE int GetAmountToPickup() const { return AmountToPickup; }
	FORCEINLINE void SetAmountToPickup(const int Value) { AmountToPickup = Value; }
};
