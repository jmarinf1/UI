#pragma once

#include "CoreMinimal.h"
#include "ElectricElement.h"
#include "ElectricPuzzle.generated.h"

UCLASS()
class LADYUMBRELLA_API AElectricPuzzle : public AElectricElement
{
	GENERATED_BODY()

public:
	AElectricPuzzle();

protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetElectrified(bool bNewElectrified) override;
	virtual void ResetElectrifiedState() override;
	// Meshes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transform")
	FVector MeshPosition;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transform")
	FVector MeshRotation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transform")
	FVector MeshScale;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transform")
	FVector LightScale;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transform")
	FVector CollisionExtent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	UStaticMeshComponent* GeneratorMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	UStaticMeshComponent* LightMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Scene Root Object", meta = (AllowPrivateAccess = "true"))
	USceneComponent* RootComp;
private:
	UFUNCTION()
	void OnProjectileOverlap(UPrimitiveComponent* OverlappedComp,
							 AActor* OtherActor,
							 UPrimitiveComponent* OtherComp,
							 int32 OtherBodyIndex,
							 bool bFromSweep,
							 const FHitResult& SweepResult);


	// Dynamic Material for light
	UPROPERTY()
	UMaterialInstanceDynamic* GeneratorMaterialInstance;

};