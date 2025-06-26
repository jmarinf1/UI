// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericPickable.h"
#include "LadyUmbrella/Items/InteractuableObjects/GenericInteractable.h"
#include "UpgradePieces.generated.h"

class UBoxComponent;

UCLASS()
class LADYUMBRELLA_API AUpgradePieces : public AGenericPickable
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Graphics", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMeshComponent;
	
public: 
	AUpgradePieces();

private:
	
	virtual int32 Interacting() override;
	virtual int32 Interacting(APlayerCharacter* Interactor) override;
	
};
