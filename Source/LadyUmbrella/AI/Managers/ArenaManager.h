// 
// ArenaManager.h
// 
// Manager that handles enemy spawning, victory condition when everyone is dead or reset when the player dies.
// 
// Copyright Zulo Interactive. All Rights Reserved.
//


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LadyUmbrella/AI/Components/EnemySpawnerComponent.h"
#include "ArenaManager.generated.h"
//fordward declaration
class UEnemyCoordinationComponent;
class UEnemySpawnerComponent;
class AEnemyCharacter;
class AAISpawnPoint;
class ACoordinationManager;
class APlayerCharacter;
class UEnemyZonesComponent;
class AAISpawnPoint;
class ATriggerBoxArena;

#pragma region structs
USTRUCT(BlueprintType)
struct FEnemyPoolEntry
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(VisibleAnywhere)
	AEnemyCharacter* Enemy = nullptr;

	UPROPERTY(VisibleAnywhere)
	int Group = 0;
	
	bool operator==(const FEnemyPoolEntry & other) const
	{
		return (other.Enemy == Enemy);
	}
};
USTRUCT()
struct FQueueStruct {
	GENERATED_BODY()

public:
	//Enemies Ids in zone
	UPROPERTY(VisibleInstanceOnly)
	TArray<int32> EnemiesIDs;

	void Clear()
	{
		EnemiesIDs.Empty();
	}
};

#pragma endregion
//Delegates
DECLARE_MULTICAST_DELEGATE(FRegisterSpawnsDelegate);

UCLASS()
class LADYUMBRELLA_API AArenaManager : public AActor
{
	GENERATED_BODY()
#pragma region configuration
	//int -> num zone
	//struct -> arrray de flee enemies ( ids) 
	UPROPERTY(VisibleInstanceOnly,Category="Configuration")
	TMap<int,FQueueStruct> FleeQueue;
	
	UPROPERTY(VisibleAnywhere,Category="Configuration")
	TArray<FEnemyPoolEntry> EnemiesPool;
	
	//coordination manager reference
	// UPROPERTY(EditAnywhere,Category="Configuration")
	// ACoordinationManager* CoordinationManager;
	//arena has spawned enemies or not
	UPROPERTY(VisibleAnywhere,Category="Configuration")
	bool bEnemiesSpawned;
	//combat started
	UPROPERTY(VisibleAnywhere,Category="Configuration")
	bool bCombatStarted;
	//arena is active or not
	UPROPERTY(EditAnywhere,Category="Configuration")
	bool bArenaActive;

	UPROPERTY(VisibleAnywhere,Category="Configuration")
	bool bEnemiesRegistered;
	
	UPROPERTY(EditAnywhere,Category="Configuration")
	float HearRadius;

	//if is false, arena is manageed as Mafia Arena
	UPROPERTY(EditAnywhere,Category="Configuration")
	bool bIsAgencyArena;

	//condition to know is reinforcement is needed
	UPROPERTY(EditInstanceOnly,Category="Configuration")
	bool bReinforcementsActivated;

	
	UPROPERTY(EditInstanceOnly,Category="Configuration",meta=(EditCondition = "bReinforcementsActivated == true",EditConditionHides))
	int32 NumEnemiesAliveToCallReinforcements;
	
	UPROPERTY(VisibleAnywhere,Category="Configuration")
	bool bReinforcementsHasBeenCalled;
	
	//Triggers
	UPROPERTY(EditDefaultsOnly,Category="Triggers")
	TSubclassOf<ATriggerBoxArena> TriggerClass;

	UPROPERTY(EditAnywhere, Category="Triggers")
	int32 NumTriggersToSpawn;

	UPROPERTY(VisibleAnywhere,Category="Triggers")
	TArray<ATriggerBoxArena*> AttachedTriggers;

	const int MinNumberOfAttachedTriggers = 2;

	//Spawn Points
	UPROPERTY(EditDefaultsOnly,Category="EnemySpawns")
	TSubclassOf<AAISpawnPoint> SpawnPointClass;

	UPROPERTY(EditAnywhere, Category="EnemySpawns")
	int32 NumEnemySpawnPoints;

	UPROPERTY(VisibleAnywhere,Category="EnemySpawns")
	TArray<AAISpawnPoint*> AttachedEnemySpawns;

	const int MinNumberOfAttachedEnemySpawns = 1;

#pragma endregion
#pragma region components
	UPROPERTY(VisibleAnywhere)
	class UBillboardComponent* Billboard;

	UPROPERTY()
	UStaticMesh* StaticMeshForSpawnPoint;

	//Zones Component - manage zones in covers and enemies
	UPROPERTY(EditInstanceOnly)
	UEnemyZonesComponent* EnemyZonesComponent;

	UPROPERTY(EditInstanceOnly)
	UEnemySpawnerComponent* EnemySpawnerComponent;

	UPROPERTY(EditInstanceOnly)
	UEnemyCoordinationComponent* EnemyCoordinationComponent;
	
	UPROPERTY()
	APlayerCharacter* PlayerCharacter; //player reference
#pragma endregion
	//To know what the first regular agency spawned
	UPROPERTY()
	bool bFirstRegularAssigned = false;
public:	
	//delegate to notifi spawn points to be registered
	FRegisterSpawnsDelegate OnRegisterSpawnsDelegate;
	
	void Activate();

	void StartCombat();

	void EnterToArena(uint8 NumGroup) const;
	
	FORCEINLINE bool IsActive() const { return bEnemiesSpawned; }

	FORCEINLINE bool IsCombatStarted() const { return bCombatStarted; }

	void UnlockBlockingElement();
#pragma region  ZonesComponentMethods
	void ClearZone(const int NumZone) const;
	void ReduceEnemyInZone(const int NumZone) const;
	void ReduceFreeCoverInZone(const int NumZone) const;
	void RemoveEnemyFromZone(AEnemyCharacter* Enemy) const;
	void AddCoverZone(const int NumZone) const;
	void AddEnemyZone(const int NumZone) const;
	bool IsZoneCreated(const int NumZone) const;
	int GetFreeCoversInZone(const int NumZone) const;
	int GetEnemiesInZone(const int NumZone) const;
#pragma endregion

	void AddEnemyToPool(FEnemyPoolEntry NewEnemy);

	void RegisterSpawn(AAISpawnPoint* SpawnToRegister) const;

	int GetHighestZone() const;

	FORCEINLINE bool GetIsAgencyArena() const {return bIsAgencyArena; };
	
	UFUNCTION()
	FORCEINLINE TArray<FEnemyPoolEntry>& GetEnemyPool() { return EnemiesPool;};
	
	TMap<int,FQueueStruct>* GetFleeQueue();
	// Sets default values for this actor's properties
	AArenaManager();

	bool FORCEINLINE IsFirstRegularAgencySpawned() const { return bFirstRegularAssigned;}
	void FORCEINLINE SetFirstRegularAgencySpawnedFlag() {  bFirstRegularAssigned = true;}
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	virtual void Destroyed() override;
	
private:
	//death delegates related functions
	UFUNCTION()
	void OnEnemyDeath(AGenericCharacter* DeadCharacter);
	void ClearEnemiesPool();

	UFUNCTION()
	void OnPlayerDeath(AGenericCharacter* DeadCharacter);
	//shoot delegates
	UFUNCTION()
	void OnPlayerShoot();

#if UE_EDITOR
	virtual void OnConstruction(const FTransform& Transform) override;
	void DeleteIncorrectAttachedActors();
	void CheckMinNumberofAttachedActors();
	void CreateNewTriggerBoxInstances();
	void RemoveExcessTriggerBoxes();
	void CreateNewEnemySpawnPointsInstances();
	void RemoveExcessEnemySpawPoints();
#endif
};
