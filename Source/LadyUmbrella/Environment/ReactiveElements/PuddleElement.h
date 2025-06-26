#pragma once

#include "CoreMinimal.h"
#include "ElectricItems/ElectricElement.h"
#include "PuddleElement.generated.h"

UCLASS()
class LADYUMBRELLA_API APuddleElement : public AElectricElement
{
	GENERATED_BODY()

public:
	APuddleElement();

protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetElectrified(bool bNewElectrified) override;
	virtual void ResetElectrifiedState() override;
	// Mesh position, rotation, and scale
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transform")
	FVector MeshPosition; 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transform")
	FVector MeshRotation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transform")
	FVector MeshScale;

private:
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComp,
							 AActor* OtherActor,
							 UPrimitiveComponent* OtherComp,
							 int32 OtherBodyIndex,
							 bool bFromSweep,
							 const FHitResult& SweepResult);

	
};
