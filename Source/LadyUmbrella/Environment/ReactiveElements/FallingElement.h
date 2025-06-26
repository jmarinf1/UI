// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ReactiveElement.h"
#include "FallingElement.generated.h"


class USkeletalMeshComponent;

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API AFallingElement : public AReactiveElement
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reactive Behaviour", meta = (AllowPrivateAccess = "true"))
	bool bHasToBeDestroyed;
	
public:
	AFallingElement();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* SkeletalMeshComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reactive Behaviour", meta = (AllowPrivateAccess = "true", ClampMin = 0.1f, ClampMax = 50.f, UIMin = 0.1f, UIMax = 50.0f))
	float SimulationSeconds = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reactive Behaviour", meta = (AllowPrivateAccess = "true", ClampMin = 0.1f, ClampMax = 50.f, UIMin = 0.1f, UIMax = 50.0f))
	float LingeringSeconds = 1.0f;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void Reaction(HitSource Source) override;

private:
	float FallingDistance = 0.5f;
	float GravityForce = 0.f;
	float CurrentSpeed = 0.f;
};
