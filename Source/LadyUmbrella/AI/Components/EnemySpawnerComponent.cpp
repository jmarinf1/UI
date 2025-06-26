// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawnerComponent.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LadyUmbrella/AI/Managers/AISpawnPoint.h"
#include "LadyUmbrella/Characters/Enemies/EnemyCharacter.h"
#include "LadyUmbrella/AI/Controllers/AIControllerGeneric.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Environment/ReactiveElements/ReactiveElement.h"
#include "LadyUmbrella/Weapons/GenericWeapon.h"
#include "LadyUmbrella/AI/IAConstants.h"
#include "LadyUmbrella/AI/Managers/ArenaManager.h"

// Sets default values for this component's properties
UEnemySpawnerComponent::UEnemySpawnerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	CurrentSpawnPointIndex = 0;
	CurrentEventEnterArena = 0;
	SpawnIndexInGroup = 0;
	RemainingEnterArenaEvent = 0;
	TotalEnterArenaEvent = 0;
	
	TimeBetweenSpawnsOnActivate = 1.f;
	TimeBetweenSpawnsOnEvent = 0.3f;
	TimeBetweenSpawnsOnReinforcement = 0.3f;
	SpawnZOffset = 50.f;
}

void UEnemySpawnerComponent::SpawnEnemies()
{
	if (SpawnPointsOnActivate.Num() != 0)
	{
		//Sort by spawn order
		SortSpawns(SpawnPointsOnActivate);
		//Timer Delegate
		FTimerDelegate Delegate;
		Delegate.BindUObject(this, &UEnemySpawnerComponent::SpawnHandlerOnActive);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_SpawnDelay, Delegate, TimeBetweenSpawnsOnActivate, true);
	}
}

void UEnemySpawnerComponent::SetReferences(APlayerCharacter* NewPlayer,AArenaManager* NewArena)
{
	PlayerCharacter = NewPlayer;
	ArenaManager = NewArena;
}

void UEnemySpawnerComponent::AddSpawnPoint(AAISpawnPoint* NewSpawnPoint)
{
	//check if is reinforcement
	if (NewSpawnPoint->bIsReinforcement)
	{
		SpawnPointsReinforcements.Add(NewSpawnPoint);
		return;
	}
	//check if is part of an event
	if (NewSpawnPoint->EnterArenaGroup != -1)
	{
		// assign group
		if (!SpawnPointsOnEnterEvent.Contains(NewSpawnPoint->EnterArenaGroup))
		{
			TotalEnterArenaEvent++;
			RemainingEnterArenaEvent++;
		}
		TArray<AAISpawnPoint*>& GroupArray = SpawnPointsOnEnterEvent.FindOrAdd(NewSpawnPoint->EnterArenaGroup);
		GroupArray.Add(NewSpawnPoint);
		return;
	}

	//Enemy is spawned when arena is activated
	SpawnPointsOnActivate.Add(NewSpawnPoint);
}

void UEnemySpawnerComponent::SortSpawns(TArray<AAISpawnPoint*>& SpawnGroup) const
{
	if (bSpawnFollowsAnOrder)
	{
		SpawnGroup.Sort([](const AAISpawnPoint& A, const AAISpawnPoint& B)
		{
			return A.SpawnOrder < B.SpawnOrder;
		});
	}
}

void UEnemySpawnerComponent::SpawnEnterArenaGroup(uint8 NumGroup)
{
	if (NumGroup != CurrentEventEnterArena)	return;

	RemainingEnterArenaEvent--;
	
	if (SpawnPointsOnEnterEvent.Find(CurrentEventEnterArena))
	{
		TArray<AAISpawnPoint*>& SpawnGroup = SpawnPointsOnEnterEvent[CurrentEventEnterArena];
		//Sort by spawn order
		SortSpawns(SpawnGroup);
		
		//Spawn Enemies
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_SpawnDelayEvent, [this,SpawnGroup]()
		{
			// Verify if current event group already exist
			if (!SpawnPointsOnEnterEvent.Contains(CurrentEventEnterArena))
			{
				//go next
				++CurrentEventEnterArena;
				SpawnIndexInGroup = 0;

				//no events available
				if (!SpawnPointsOnEnterEvent.Contains(CurrentEventEnterArena))
				{
					GetWorld()->GetTimerManager().ClearTimer(TimerHandle_SpawnDelayEvent);
					return;
				}
			}

			// Verify if there are spawns to be triggered
			if (SpawnIndexInGroup < SpawnGroup.Num())
			{
				AAISpawnPoint* SpawnPoint = SpawnGroup[SpawnIndexInGroup];
				
				if (SpawnPoint && SpawnPoint->bIsActive)
				{
					SpawnEnemy(*SpawnPoint);
				}
				++SpawnIndexInGroup;
			}
			else
			{
				//go next
				++CurrentEventEnterArena;
				SpawnIndexInGroup = 0;
				//clear timer
				GetWorld()->GetTimerManager().ClearTimer(TimerHandle_SpawnDelayEvent);
			}
		}, 0.3f, true);
	}
	else
	{
		//FLogger::DebugLog("No se ha encontrado el grupo asociado al evento");
	}
}

void UEnemySpawnerComponent::SetCurrentEventEnterArena(int32 NewEventEnterArena)
{
	CurrentEventEnterArena = NewEventEnterArena;
	RemainingEnterArenaEvent = TotalEnterArenaEvent;
}

void UEnemySpawnerComponent::SpawnReinforcements()
{
	SortSpawns(SpawnPointsReinforcements);
	
	int NumEnemiesToBeSpawned = SpawnPointsReinforcements.Num();
	int NumEnemiesSpawned = 0;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_SpawnDelayReinforcements, [this,NumEnemiesToBeSpawned,NumEnemiesSpawned]() mutable
	{
		
		if (NumEnemiesSpawned < NumEnemiesToBeSpawned)
		{
			SpawnEnemy(*SpawnPointsReinforcements[NumEnemiesSpawned]);
			NumEnemiesSpawned++;
		}
		else
		{
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle_SpawnDelayReinforcements);
		}
	},TimeBetweenSpawnsOnReinforcement,true);
}

// Called when the game starts
void UEnemySpawnerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UEnemySpawnerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UEnemySpawnerComponent::SpawnHandlerOnActive()
{
	if (const AAISpawnPoint* CurrentSpawnPoint = SpawnPointsOnActivate[CurrentSpawnPointIndex])
	{
		switch (CurrentSpawnPoint->Target)
		{
		case ETarget::Enemy:
			{
				SpawnEnemy(*CurrentSpawnPoint); //funcion spawn enemie generica
			}
			break;
		case ETarget::Interactable:
			{	//pasar a funcion generica spawn interactable
				SpawnInteractable(*CurrentSpawnPoint);
			}
			break;
		default:
			break;
		}
	}
	
	if (CurrentSpawnPointIndex == SpawnPointsOnActivate.Num() - 1)
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_SpawnDelay);
		CurrentSpawnPointIndex = 0;
		return;
	}
	
	CurrentSpawnPointIndex++;
}

void UEnemySpawnerComponent::SpawnEnemy(const AAISpawnPoint& SpawnPoint) const
{
	//get transform
	FVector Location = SpawnPoint.GetStaticMesh()->GetComponentLocation();
	Location.Z += SpawnZOffset;
	FRotator Rotation = SpawnPoint.GetStaticMesh()->GetComponentRotation();
	//get Enemy Class
	UClass* EnemyClass = SpawnPoint.EnemyType;
	if (IsValid(EnemyClass))
	{
		//Spawn
		AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(GetWorld()->SpawnActor(
			EnemyClass,
			&Location,
			&Rotation
		));
		if (IsValid(Enemy))
		{
			//setup AIController
			Enemy->SpawnDefaultController();
			if (AAIControllerGeneric* Controller = Cast<AAIControllerGeneric>(Enemy->GetController()))
			{
				if(SpawnPoint.ActorHasTag("Flanker"))
				{	//esto meterlo en funcion en el controller
					Controller->GetBlackboardComponent()->SetValueAsBool("IsFlanker", true);
				}
				if (SpawnPoint.EnterArenaGroup != IAConstants::INVALID_NUMBER_DF_ENEMIES || SpawnPoint.bIsReinforcement)
				{
					Controller->DetectPlayer(PlayerCharacter);
				}
				Controller->InitializeController(ArenaManager, SpawnPoint);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Controller is not of expected base type"));
			}
			//setup EnemyCharacter
			Enemy->SetSplinePath(SpawnPoint.Path);
			Enemy->SetWeaponBP(SpawnPoint.WeaponBP);
			Enemy->Initialize();
			//save in pool
			ArenaManager->AddEnemyToPool(FEnemyPoolEntry(Enemy,SpawnPoint.EnterArenaGroup));
		}	
	}
}

void UEnemySpawnerComponent::SpawnInteractable(const AAISpawnPoint& SpawnPoint)
{
	FVector Location = SpawnPoint.GetStaticMesh()->GetComponentLocation();
	FRotator Rotation = SpawnPoint.GetStaticMesh()->GetComponentRotation();
	
	UClass* InteractableClass = SpawnPoint.ReactiveElementType;
	AReactiveElement* Interactable = Cast<AReactiveElement>(GetWorld()->SpawnActor(
		InteractableClass,
		&Location,
		&Rotation
		));
	InteractablesPool.Emplace(Interactable);
}