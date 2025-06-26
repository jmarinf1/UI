// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemySpawnerComponent.generated.h"

//forward declarations
class APlayerCharacter;
class AArenaManager;
class ACoordinationManager;
class AAISpawnPoint;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LADYUMBRELLA_API UEnemySpawnerComponent : public UActorComponent
{
	GENERATED_BODY()
	
	//Spawn points references
	UPROPERTY(EditInstanceOnly,Category="Configuration")
	TArray<AAISpawnPoint*> SpawnPointsOnActivate;

	UPROPERTY(EditInstanceOnly,Category="Configuration")
	TArray<AAISpawnPoint*> SpawnPointsReinforcements;
	
	//interactable pool
	UPROPERTY(VisibleAnywhere,Category="Configuration")
	TArray<AActor*> InteractablesPool;
	
	//spawns trigger in Events
	TMap<int32, TArray<AAISpawnPoint*>> SpawnPointsOnEnterEvent;

	UPROPERTY(EditAnywhere,Category="Configuration")
	TSubclassOf<AAISpawnPoint> SpawnPointClass;

	UPROPERTY(EditAnywhere,Category="Configuration")
	bool bSpawnFollowsAnOrder = false;

	//Counters
	int CurrentSpawnPointIndex; //index of SpawnPointsOnActivate
	int CurrentEventEnterArena; //key of SpawnPointsOnEnterEvent
	int32 SpawnIndexInGroup; //index of array inside SpawnPointsOnEnterEvent map
	int8_t RemainingEnterArenaEvent;
	int8_t TotalEnterArenaEvent;
	
	//Timers
	UPROPERTY()
	FTimerHandle TimerHandle_SpawnDelay; //Spawn delay
	UPROPERTY()
	FTimerHandle TimerHandle_SpawnDelayEvent; //Spawn delay

	UPROPERTY()
	FTimerHandle TimerHandle_SpawnDelayReinforcements; 

	UPROPERTY(EditAnywhere,Category="Configuration")
	float TimeBetweenSpawnsOnActivate;

	UPROPERTY(EditAnywhere,Category="Configuration")
	float TimeBetweenSpawnsOnEvent;

	UPROPERTY(EditAnywhere,Category="Configuration")
	float TimeBetweenSpawnsOnReinforcement;

	UPROPERTY(EditAnywhere,Category="Configuration")
	float SpawnZOffset;
	//References
	UPROPERTY()
	APlayerCharacter* PlayerCharacter;

	UPROPERTY()
	AArenaManager* ArenaManager;
public:	
	// Sets default values for this component's properties
	UEnemySpawnerComponent();

	void SpawnEnemies();

	void SetReferences(APlayerCharacter* NewPlayer, AArenaManager* NewArena);
	
	void AddSpawnPoint(AAISpawnPoint* NewSpawnPoint);
	
	void SpawnEnterArenaGroup(uint8 NumGroup);

	void SetCurrentEventEnterArena(int32 NewEventEnterArena);

	void SpawnReinforcements();

	FORCEINLINE int GetRemainingEnterArenaEvents() const { return RemainingEnterArenaEvent;}
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
private:
	UFUNCTION()
	void SpawnHandlerOnActive();

	void SpawnEnemy(const AAISpawnPoint& SpawnPoint) const;

	void SpawnInteractable(const AAISpawnPoint& SpawnPoint);

	void SortSpawns(TArray<AAISpawnPoint*>& SpawnGroup) const;
};



