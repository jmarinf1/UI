// 
// ArenaManager.cpp
// 
// Manager can be activated(Spawn enemies and start patroling), startcombat(enemies start searching covers and attacking)
// 
// Copyright Zulo Interactive. All Rights Reserved.
//


#include "ArenaManager.h"

#include "TriggerBoxArena.h"
#include "Components/BillboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Containers/Map.h"
#include "LadyUmbrella/AI/Components/EnemyCoordinationComponent.h"
#include "LadyUmbrella/AI/Components/EnemySpawnerComponent.h"
#include "LadyUmbrella/AI/Controllers/AIControllerGeneric.h"
#include "LadyUmbrella/AI/Controllers/AIControllerMafia.h"
#include "LadyUmbrella/Characters/Enemies/EnemyCharacter.h"
#include "LadyUmbrella/Weapons/GenericWeapon.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/AI/Components/EnemyZonesComponent.h"
#include "LadyUmbrella/AI/Managers/AISpawnPoint.h"

// Sets default values
AArenaManager::AArenaManager()
{
	Billboard = CreateDefaultSubobject<UBillboardComponent>("Billboard");
	static ConstructorHelpers::FObjectFinder<UTexture2D> TextureArena(TEXT("/Script/Engine.Texture2D'/Game/Textures/Sprites/Icons/ArenaManager/T_ArenaIcon.T_ArenaIcon'"));
	if (TextureArena.Object != nullptr)
	{
		Billboard->SetSprite(TextureArena.Object);
	}
	Billboard->SetupAttachment(RootComponent);

	EnemyZonesComponent = CreateDefaultSubobject<UEnemyZonesComponent>(TEXT("EnemyZonesComponent"));
	EnemySpawnerComponent = CreateDefaultSubobject<UEnemySpawnerComponent>(TEXT("EnemySpawnerComponent"));
	EnemyCoordinationComponent = CreateDefaultSubobject<UEnemyCoordinationComponent>(TEXT("EnemyCoordinationComponent"));  
	
#if WITH_EDITORONLY_DATA
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ArrowStaticMeshRef (TEXT("/Game/Meshes/StaticMeshes/EditorOnly/ArenaManager/SM_SpawnPoint.SM_SpawnPoint"));
	if (ArrowStaticMeshRef.Object != nullptr)
	{
		StaticMeshForSpawnPoint = ArrowStaticMeshRef.Object;
	}
#endif

	//Initialize variables
	HearRadius = 3000.f;
	bEnemiesSpawned = false;
	bCombatStarted = false;
	bArenaActive = true;
	bEnemiesRegistered = false;
	NumTriggersToSpawn = 2;
	NumEnemySpawnPoints = 1;
	bIsAgencyArena = false;
}

void AArenaManager::Activate()
{
	if (bEnemiesSpawned || !bArenaActive || !IsValid(EnemySpawnerComponent)) return;

	bEnemiesSpawned = true;

	//suscribe delegate dead and shoot
	if (PlayerCharacter)
	{
		PlayerCharacter->OnDeathChanged.AddDynamic(this, &AArenaManager::OnPlayerDeath);
		PlayerCharacter->OnPlayerShot.BindUObject(this, &AArenaManager::OnPlayerShoot);
	}

	//spawns will be registered
	if (!bEnemiesRegistered)
	{
		OnRegisterSpawnsDelegate.Broadcast();
		bEnemiesRegistered = true;
	}

	
	EnemySpawnerComponent->SpawnEnemies();
}

void AArenaManager::StartCombat()
{
	if (bCombatStarted || !bArenaActive) return;
	
	bCombatStarted = true;
	
	EnemyCoordinationComponent->Initialize();
	
	//enemies begin detect player
	for (FEnemyPoolEntry EnemyEntry : EnemiesPool)
	{
		if (AAIControllerGeneric* Controller = Cast<AAIControllerGeneric>(EnemyEntry.Enemy->GetController()))
		{
			Controller->DetectPlayer(PlayerCharacter);
		}
	}
}

void AArenaManager::EnterToArena(uint8 NumGroup) const
{
	if (!bArenaActive || !IsValid(EnemySpawnerComponent)) return;
	
	EnemySpawnerComponent->SpawnEnterArenaGroup(NumGroup);
}

// Called when the game starts or when spawned
void AArenaManager::BeginPlay()
{
	Super::BeginPlay();

	if (!bArenaActive) return; //check if arena is active
	
	//get player reference
	PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (IsValid(PlayerCharacter))
	{
		PlayerCharacter->OnPlayerShot.BindUObject(this, &AArenaManager::OnPlayerShoot);
	}
	
	EnemySpawnerComponent->SetReferences(PlayerCharacter,this);
	//EnemySpawnerComponent->SaveSpawnPoints();
	
	//save initial zones Pool
	EnemyZonesComponent->Initialize();
}
#if WITH_EDITOR
void AArenaManager::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	CheckMinNumberofAttachedActors();

	DeleteIncorrectAttachedActors();

	// If we need to create new Trigger Boxes to reach our number.
	if (AttachedTriggers.Num() < NumTriggersToSpawn)
	{
		CreateNewTriggerBoxInstances();
	}
	// If we need to delete Trigger Boxes because we have too many.
	else if (AttachedTriggers.Num() > NumTriggersToSpawn)
	{
		RemoveExcessTriggerBoxes();
	}
	
	// If we need to create new Spawn to reach our number.
	if (AttachedEnemySpawns.Num() < NumEnemySpawnPoints)
	{
		CreateNewEnemySpawnPointsInstances();
	}
	// If we need to delete Spawn Boxes because we have too many.
	else if (AttachedEnemySpawns.Num() > NumEnemySpawnPoints)
	{
		RemoveExcessEnemySpawPoints();
	}
}
#endif
void AArenaManager::Destroyed()
{
	Super::Destroyed();

#if UE_EDITOR // This code is only meant to execute in the editor, cause the Arena Manager should not be destroyed otherwise.
	
	// Deleting all Triggers attached to this Manager.
	for (int i = AttachedTriggers.Num() - 1; i >= 0; --i)
	{
		if (IsValid(AttachedTriggers[i]))
		{
			AttachedTriggers[i]->Destroy();
		}
		
		AttachedTriggers.RemoveAt(i);
	}

	// Deleting all EnemySpawnPoints attached to this Manager.
	for (int i = AttachedEnemySpawns.Num() - 1; i >= 0; --i)
	{
		if (IsValid(AttachedEnemySpawns[i]))
		{
			AttachedEnemySpawns[i]->Destroy();
		}
		
		AttachedEnemySpawns.RemoveAt(i);
	}

#endif
}

void AArenaManager::UnlockBlockingElement()
{
	//unlock blocking element
}

void AArenaManager::ClearZone(const int NumZone) const
{
	EnemyZonesComponent->ClearEnemiesInZone(NumZone);
	EnemyZonesComponent->ClearFreeCoversInZone(NumZone);
}

void AArenaManager::ReduceEnemyInZone(const int NumZone) const
{
	EnemyZonesComponent->RemoveEnemyFromZone(NumZone);
}

void AArenaManager::ReduceFreeCoverInZone(const int NumZone) const
{
	EnemyZonesComponent->RemoveFreeCoverFromZone(NumZone);
}

void AArenaManager::AddCoverZone(const int NumZone) const
{
	EnemyZonesComponent->AddFreeCoverToZone(NumZone);
}
void AArenaManager::AddEnemyZone(const int NumZone) const
{
	EnemyZonesComponent->AddEnemyToZone(NumZone);
}

bool AArenaManager::IsZoneCreated(const int NumZone) const
{
	return EnemyZonesComponent->IsZoneCreated(NumZone);
}

int AArenaManager::GetFreeCoversInZone(const int NumZone) const
{
	return EnemyZonesComponent->GetFreeCoversInZone(NumZone);
}

int AArenaManager::GetEnemiesInZone(const int NumZone) const
{
	return EnemyZonesComponent->GetEnemiesInZone(NumZone);
}

void AArenaManager::AddEnemyToPool(FEnemyPoolEntry NewEnemy)
{
	NewEnemy.Enemy->OnDeathChanged.AddDynamic(this, &AArenaManager::OnEnemyDeath);
	EnemiesPool.Emplace(NewEnemy);
	EnemyCoordinationComponent->AddEnemy(NewEnemy);
}


void AArenaManager::RemoveEnemyFromZone(AEnemyCharacter* Enemy) const
{
	if (IsValid(Enemy) && IsValid(Enemy->GetController()))
	{
		if (Enemy->GetController()->IsA(AAIControllerMafia::StaticClass()))
		{
			//Remove from zone pool
			AAIControllerMafia* Controller = Cast<AAIControllerMafia>(Enemy->GetController());
			ReduceEnemyInZone(Controller->GetZone());
		}	
	}
}

void AArenaManager::RegisterSpawn(AAISpawnPoint* SpawnToRegister) const
{
	EnemySpawnerComponent->AddSpawnPoint(SpawnToRegister);
}

int AArenaManager::GetHighestZone() const
{
	return EnemyZonesComponent->GetHiguestZone();
}

TMap<int,FQueueStruct>* AArenaManager::GetFleeQueue()
{
	return &FleeQueue;
}

///////////////////////
//Dead Events
void AArenaManager::OnEnemyDeath(AGenericCharacter* DeadCharacter)
{
	if (!IsValid(EnemyCoordinationComponent)) return;
	
	if (AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(DeadCharacter))
	{
		//reduce 1 enemy in zone
		RemoveEnemyFromZone(Enemy);
		//remove enemy in coordination
		EnemyCoordinationComponent->RemoveEnemy(Enemy);
		//remove enemy from pool and his components
		for (FEnemyPoolEntry EnemyEntry : EnemiesPool)
		{
			if (IsValid(EnemyEntry.Enemy) && EnemyEntry.Enemy == Enemy)
			{
				EnemiesPool.Remove(EnemyEntry);
				break;
			}
		}
	}

	int RemainingEnemies = EnemiesPool.Num();
	bool bArenaEventsEnd = EnemySpawnerComponent->GetRemainingEnterArenaEvents() == 0;
	//all events called, reinforcements called and evemts called
	if (RemainingEnemies == 0 && bArenaEventsEnd)
	{
		if (bReinforcementsActivated && !bReinforcementsHasBeenCalled)
		{
			return;
		}
		
		EnemyCoordinationComponent->StopAndCleanup();
		//unlock blocking element
		UnlockBlockingElement();
		//unbind delegates
		PlayerCharacter->OnDeathChanged.RemoveDynamic(this, &AArenaManager::OnEnemyDeath);
		PlayerCharacter->OnPlayerShot.Unbind();
	}

	//Call reinforcements
	if (bReinforcementsActivated && !bReinforcementsHasBeenCalled && RemainingEnemies <= NumEnemiesAliveToCallReinforcements && bArenaEventsEnd)
	{
		bReinforcementsHasBeenCalled = true;
		EnemySpawnerComponent->SpawnReinforcements();
	}
}

void AArenaManager::ClearEnemiesPool()
{
	//Cambiar: Que pases a un estado de Patrulla, Idle o random movement
	//De momento - destroy
	for (FEnemyPoolEntry EnemyEntry : EnemiesPool)
	{
		 if (IsValid(EnemyEntry.Enemy))
		 {
		 	EnemyEntry.Enemy->GetEquippedWeapon()->Destroy();
		 	if (EnemyEntry.Enemy->GetShield())
		 	{
		 		EnemyEntry.Enemy->GetShield()->Destroy();
		 	}
		 	EnemyEntry.Enemy->Destroy();
		 }
	}
	EnemiesPool.Empty();
}

void AArenaManager::OnPlayerDeath(AGenericCharacter* DeadCharacter)
{
	//Stop Coordination Manager
	EnemyCoordinationComponent->StopAndCleanup();
	
	//Clear Enemies Pool
	ClearEnemiesPool();

	// Reset Events
	EnemySpawnerComponent->SetCurrentEventEnterArena(0);
	
	//restore initial zones Pool
	EnemyZonesComponent->Reset();
	//restore flee queue
	for (TPair<int,FQueueStruct>& Elem : FleeQueue)
	{
		Elem.Value.Clear();
	}
	
	//Reset triggers conditions
	bEnemiesSpawned = false;
	bCombatStarted = false;
	bReinforcementsHasBeenCalled = false;

	
	PlayerCharacter->OnDeathChanged.RemoveDynamic(this, &AArenaManager::OnPlayerDeath);
	PlayerCharacter->OnPlayerShot.Unbind();
	
	Activate();
}

void AArenaManager::OnPlayerShoot()
{
	//distance arena instance to player
	float DistanceArenaPlayer = (PlayerCharacter->GetActorLocation() - GetActorLocation()).Size();
	if (DistanceArenaPlayer < HearRadius)
	{
		StartCombat();
	}
}

//Editor functions
#pragma region Editor:functions

#if UE_EDITOR
void AArenaManager::DeleteIncorrectAttachedActors()
{
	// Delete (from our array and the world) those AttachedTriggers that are no longer correctly setup.
	for (int i = AttachedTriggers.Num() - 1; i >= 0; --i)
	{
		if (!IsValid(AttachedTriggers[i]))
		{
			AttachedTriggers.RemoveAt(i);
			continue;
		}
		
		if (!AttachedTriggers[i]->IsAttachedTo(this))
		{
			AttachedTriggers[i]->Destroy();
			AttachedTriggers.RemoveAt(i);
		}
	}

	// Delete (from our array and the world) those AttachedEnemySpawns that are no longer correctly setup.
	for (int i = AttachedEnemySpawns.Num() - 1; i >= 0; --i)
	{
		if (!IsValid(AttachedEnemySpawns[i]))
		{
			AttachedEnemySpawns.RemoveAt(i);
			continue;
		}
		
		if (!AttachedEnemySpawns[i]->IsAttachedTo(this))
		{
			AttachedEnemySpawns[i]->Destroy();
			AttachedEnemySpawns.RemoveAt(i);
		}
	}
}

void AArenaManager::CheckMinNumberofAttachedActors()
{
	// Forcing a minimum positive number of TriggersToSpawn
	if (NumTriggersToSpawn < MinNumberOfAttachedTriggers)
	{
		NumTriggersToSpawn = MinNumberOfAttachedTriggers;
	}
	
	// Forcing a minimum positive number of EnemySpawns
	if (NumEnemySpawnPoints < MinNumberOfAttachedEnemySpawns)
	{
		NumEnemySpawnPoints = MinNumberOfAttachedEnemySpawns;
	}
}

void AArenaManager::CreateNewTriggerBoxInstances()
{
	for (int i = AttachedTriggers.Num(); i < NumTriggersToSpawn; i++)
	{
		FVector Offset(0, 200 * i, 0);
		FVector Location = GetActorLocation() + Offset;
		FActorSpawnParameters Params;
		Params.Owner = this;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		ATriggerBoxArena* Trigger = GetWorld()->SpawnActor<ATriggerBoxArena>(TriggerClass, Location, FRotator::ZeroRotator, Params);
		if (IsValid(Trigger))
		{
			Trigger->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
			Trigger->SetArenaReference(this);
			AttachedTriggers.Add(Trigger);

			if (i == 0)
			{
				Trigger->SetEFunc(EPurpose::ActivateArena);
			}
			if (i == 1)
			{
				Trigger->SetEFunc(EPurpose::StartCombatArena);
			}
		}
	}
}

void AArenaManager::RemoveExcessTriggerBoxes()
{
	for (int i = AttachedTriggers.Num() - 1; i >= NumTriggersToSpawn; i--)
	{
		if (IsValid(AttachedTriggers[i]))
		{
			AttachedTriggers[i]->Destroy();
		}
		AttachedTriggers.RemoveAt(i);
	}
}

void AArenaManager::CreateNewEnemySpawnPointsInstances()
{
	for (int i = AttachedEnemySpawns.Num(); i < NumEnemySpawnPoints; i++)
	{
		FVector Offset(200, 200 * i, 0);
		FVector Location = GetActorLocation() + Offset;
		FActorSpawnParameters Params;
		Params.Owner = this;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AAISpawnPoint* NewSpawnPoint = GetWorld()->SpawnActor<AAISpawnPoint>(SpawnPointClass, Location, FRotator::ZeroRotator, Params);
		if (IsValid(NewSpawnPoint))
		{
			NewSpawnPoint->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
			NewSpawnPoint->SetArenaReference(this);
			AttachedEnemySpawns.Add(NewSpawnPoint);
			// if (!bAttached)
			// {
			// 	UE_LOG(LogTemp, Error, TEXT("Failed to attach SpawnPoint %s to ArenaManager!"), *NewSpawnPoint->GetName());
			// }
		}
	}
}

void AArenaManager::RemoveExcessEnemySpawPoints()
{
	for (int i = AttachedEnemySpawns.Num() - 1; i >= NumEnemySpawnPoints; i--)
	{
		if (IsValid(AttachedEnemySpawns[i]))
		{
			AttachedEnemySpawns[i]->Destroy();
		}
		AttachedEnemySpawns.RemoveAt(i);
	}
}
#endif
#pragma endregion  Editor:functions