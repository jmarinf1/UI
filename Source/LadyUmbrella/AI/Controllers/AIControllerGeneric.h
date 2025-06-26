// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIControllerGeneric.generated.h"

class AArenaManager;
class AAISpawnPoint;
/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API AAIControllerGeneric : public AAIController
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, Category = "AI", meta = (AllowPrivateAcees = "true"))
	UBehaviorTree* behaviourTree;

	UPROPERTY()
	AArenaManager* CurrentArenaManager;
	
	UPROPERTY()
	APawn* OwnerPawn;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
	bool bShowDebugIcons;

	virtual void OnPossess(APawn* InPawn) override;

	//begin play -> set zone in bb
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere,Category="IA")
	int CurrentZone;

public:
	AAIControllerGeneric(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual void InitializeController(AArenaManager* ArenaManager, const AAISpawnPoint& SpawnPoint);

	virtual void DetectPlayer(AActor* PlayerCharacter);

	virtual void SetZone(int Newzone);

	void StopBehaviourTree();

	virtual AArenaManager* GetArena() const;

	UFUNCTION()
	bool GetShowDebugIcon();

	UFUNCTION()
	int GetZone() const { return CurrentZone; };
};
