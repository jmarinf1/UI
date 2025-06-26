// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "LadyUmbrella/Interfaces/HookInterface.h"
#include "LadyUmbrella/Util/SaveSystem/SaveInterface.h"
#include "GrapplingObject.generated.h"

class UMovementComponent;
class UStaticMeshComponent;
class UBoxComponent;
class UArrowComponent;
class UTimelineComponent;
class UPrimitiveComponent;

UCLASS()
class LADYUMBRELLA_API AGrapplingObject : public AActor, public IHookInterface, public ISaveInterface
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hook SceneRoot", meta = (DisplayName = "Scene Root", AllowPrivateAccess = "true"))
	USceneComponent* SceneRootComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hook Skeletal Mesh",  meta = (DisplayName = "Skeletal Main mesh", AllowPrivateAccess = "true"))
	USkeletalMeshComponent* SkeletalMeshComponent;	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hook Skeletal Mesh",  meta = (DisplayName = "Static Main mesh", AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMeshComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision Box", meta = (DisplayName = "Impact Collision Box", AllowPrivateAccess = "true"))
	UBoxComponent* BoxComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = ( AllowPrivateAccess = "true"))
	UArrowComponent* FallingDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics Settings", meta = (DisplayName = "Is Puzzle Solved", AllowPrivateAccess = "true"))
	bool bIsSolved;

	virtual void BeginPlay() override;

public:	
	AGrapplingObject();
	
	virtual void OnConstruction(const FTransform& Transform) override;
	
	// Hook interface inherited functions
	virtual void ContactWithCable(FVector& Direction) override;
	virtual void ContactWithCable(FVector& Direction, UPrimitiveComponent* ComponentHit) override{};
	virtual GrabType GetGrabType() override;

	UPROPERTY()
	FTransform SolvedTransform;

	// Comented just in case we need it later
	//UFUNCTION()
	//void BeginOverlap( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	
	// Save

	virtual void SaveGame() override;
	virtual void LoadGame() override;
	virtual void SaveDataCheckPoint() override;
	virtual void LoadDataCheckPoint() override;
	
};
