// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericInteractable.h"
#include "Kismet/GameplayStatics.h"
#include "Workbench.generated.h"

class UWorkBenchMenu;
class USphereComponent;
class UCameraComponent;
class UStaticMeshComponent;
class USceneComponent;
class APlayerController;

UCLASS()
class LADYUMBRELLA_API AWorkbench : public AGenericInteractable
{
	GENERATED_BODY()

#pragma region COMPONENTS
	
	UPROPERTY(EditAnywhere)
	UCameraComponent* CameraComponent;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* StaticMeshComponent;
	
#pragma endregion

#pragma region WIDGETS

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UWorkBenchMenu> WorkbenchWidgetReference;

	UPROPERTY()
	UWorkBenchMenu* WorkbenchCreatedWidget;
	

#pragma endregion

	AWorkbench();

	virtual void BeginPlay() override;

	// virtual void OnConstruction(const FTransform& Transform) override;
	
	void DisableInputCustom();
	
	UFUNCTION()
	void SetUpWidget();
	
#pragma region INTERACT_FUNCTIONS
	virtual int32 Interacting() override;
	virtual int32 Interacting(APlayerCharacter* PlayerCharacternInteractor) override;
	// TODO: change this to PLAYER_CHARACTER and have that ptr do all
	virtual TYPE_OF_INTERACTION GetTypeOfInteraction() const override { return NO_PARAMS; };
#pragma endregion

	UPROPERTY()
	APlayerController* PlayerController;

	UPROPERTY()
	uint32 PiecesForUpgrade;

};
