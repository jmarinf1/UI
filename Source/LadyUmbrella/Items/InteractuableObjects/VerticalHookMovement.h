// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericInteractable.h"
#include "VerticalHookMovement.generated.h"

class UCharacterMovementComponent;
class UCameraComponent;
class UBoxComponent;
class UStaticMeshComponent;
class UArrowComponent;
class USplineComponent;
class APlayerCharacter;

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API AVerticalHookMovement : public AGenericInteractable
{
	GENERATED_BODY()

	AVerticalHookMovement();

	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
	void StartSplineMovement(APlayerCharacter* Interactor);
	UFUNCTION()
	void MovePlayerToStartOfSpline(APlayerCharacter* Interactor);
	
	float SplineProgress;

	FTimerHandle SplineTimer;

	FTimerHandle MovementToSplineTimer;
	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* CameraMovingComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UArrowComponent* ArrowComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	USplineComponent* PlayerMovementSplineComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float SplineMoveSpeed;

	bool CheckPlayerComponentsAndVisibility(APlayerCharacter*& PlayerInteractor,
		UCharacterMovementComponent*& CharMovComp, APlayerController*& PlayerController);
	
	void ClearTimer();
	
	FVector CameraOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float BlendTime; 

public:
	
	virtual void BeginPlay() override;

	virtual int32 Interacting(APlayerCharacter* CharMovComp) override;

	FORCEINLINE virtual TYPE_OF_INTERACTION GetTypeOfInteraction() const override { return PLAYER_CHARACTER; };
	
};
