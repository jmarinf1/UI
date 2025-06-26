// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LadyUmbrella/Interfaces/HitReactiveInterface.h"
#include "ReactiveElement.generated.h"

class USceneComponent;
class UBoxComponent;
class UStaticMeshComponent;
class USkeletalMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReacted);
UCLASS()
class LADYUMBRELLA_API AReactiveElement : public AActor, public IHitReactiveInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AReactiveElement();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void Reaction(HitSource Source) override;
	virtual bool ShouldReact(HitSource Source);
	
	UPROPERTY(BlueprintAssignable)
	FOnReacted OnReactedDelegate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Scene Root Object", meta = (AllowPrivateAccess = "true"))
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision Box Object", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* BoxRoot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Static Mesh Object", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ObjectMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skeletal Mesh Object", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* SkeletalMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reactive Behaviour", meta = (AllowPrivateAccess = "true"))
	TArray<HitSource> HittableBy;


protected:
	bool Reacted = false;

};
