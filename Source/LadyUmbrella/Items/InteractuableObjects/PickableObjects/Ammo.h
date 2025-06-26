// 
// Ammo.h
// 
// The munition used for the umbrella, being able to pick it up on the ground.
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "GenericPickable.h"
#include "LadyUmbrella/Interfaces/InteractInterface.h"
#include "Ammo.generated.h"

class AActor;
class UBoxComponent;
class UMaterialBillboardComponent;
class UPrimitiveComponent;
class USphereComponent;
class UStaticMeshComponent;
class UWidgetComponent;
class UInteractiveIcon;

UCLASS()
class LADYUMBRELLA_API
AAmmo : public AGenericPickable
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Graphics", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMeshComponent;

public:
	AAmmo();
	
private:	
	virtual int32 Interacting() override;
	virtual int32 Interacting(APlayerCharacter* Interactor) override;
	virtual TYPE_OF_INTERACTION GetTypeOfInteraction() const override { return NO_PARAMS; };

	virtual void BeginPlay() override;
	virtual void Tick(const float DeltaTime) override;
};
