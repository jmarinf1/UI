// Fill out your copyright notice in the Description page of Project Settings.


#include "CheckPointTrigger.h"

#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Util/Logger.h"
#include "LadyUmbrella/Util/DataDrivenStructs/FCheckpointDataDriven.h"
#include "LadyUmbrella/Util/SaveSystem/LU_GameInstance.h"

ACheckPointTrigger::ACheckPointTrigger()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CollisionBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Box Component"));
	SetRootComponent(CollisionBoxComponent);
	CollisionBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ACheckPointTrigger::OnBeginOverlap);
	bIsActive = true;

	PlayerCheckpointArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Player Checkpoint Transform"));
	PlayerCheckpointArrow->SetupAttachment(RootComponent);

	ConstructorHelpers::FObjectFinder<UDataTable> DataTableAsset(TEXT(
		"/Script/Engine.DataTable'/Game/DataStructures/DataTables/DT_EncounterCheckpoints.DT_EncounterCheckpoints'"));
	if (DataTableAsset.Succeeded())
	{
		DataTablePtr = DataTableAsset.Object;
	}
}

void ACheckPointTrigger::BeginPlay()
{
	Super::BeginPlay();
}

void ACheckPointTrigger::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                        const FHitResult& SweepResult)
{
		if (CheckHitPlayer(OtherActor))
		{
			if (ULU_GameInstance* GameInstance = Cast<ULU_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
			{
				GameInstance->SetLastCheckpointHit(this);
				GameInstance->SaveCheckPoint();

				bIsActive = false;
			}
		}
}

bool ACheckPointTrigger::GetTableIsValid() const
{
	return 	IsValid(DataTablePtr) && IsValid(DataTableRowHandler.DataTable);
}

#if WITH_EDITOR

void ACheckPointTrigger::Destroyed()
{
	Super::Destroyed();
	UnRegisterCheckpointAsEncounter();
}

void ACheckPointTrigger::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	const FName LevelName = *UGameplayStatics::GetCurrentLevelName(GetWorld());

	static const TMap<FName, int32> LevelIdMap =
	{
		{"L_Streets_Persistent", 1},
		{"L_Persistent_Museum", 2},
		{"L_Persistent_RoofTops", 3},
		{"L_ConstructionRooftops_PersistentLevel", 4},
		{"L_Building_Persistent", 5},
		{"L_Sniper_Persistent", 6}
	};

	if (const int32* FoundId = LevelIdMap.Find(LevelName))
	{
		LevelId = *FoundId;
	}
	else
	{
		LevelId = 0;
	}
	UniqueCheckpointId = GetNextAvailableCheckpointId();
}

int32 ACheckPointTrigger::GetNextIDForLevel(const FString& CurrentLevelName) const
{
	if (!DataTablePtr)
	{
		return 1;
	}

	int32 MaxID = 0;
	TArray<FCheckpointDataDriven*> AllRows;
	DataTablePtr->GetAllRows<FCheckpointDataDriven>("", AllRows);

	for (const FCheckpointDataDriven* Row : AllRows)
	{
		if (Row && Row->LevelName == CurrentLevelName)
		{
			MaxID = FMath::Max(MaxID, Row->ButtonId);
		}
	}

	return MaxID + 1;
}

int32 ACheckPointTrigger::GetNextAvailableCheckpointId() const
{
	if (!GetTableIsValid())
	{
		return -1;
	}

	TSet<int32> UsedIds;
	TArray<FCheckpointDataDriven*> AllRows;
	DataTablePtr->GetAllRows<FCheckpointDataDriven>("", AllRows);

	for (const FCheckpointDataDriven* Row : AllRows)
	{
		if (Row)
		{
			UsedIds.Add(Row->Identifier);
		}
	}

	int32 NewId = 0;
	while (UsedIds.Contains(NewId))
	{
		++NewId;
	}

	return NewId;
}

void ACheckPointTrigger::RegisterCheckpointAsEncounter()
{
	if (GetTableIsValid())
	{
		if (EncounterName.IsNone() || EncounterName.IsEqual("") || EncounterName.IsEqual("None"))
		{
			FLogger::DebugLog("Adding a checkpoint with invalid name");
			FLogger::DebugLog("Name shouldn't be None or EmptyName");
			return;
		}

		TArray<FName> Names = DataTableRowHandler.DataTable->GetRowNames();
		if (Names.Num() < 0)
		{
			return;
		}

		bool bNewRowExists = false;
		for (int32 i = 0; i < Names.Num(); ++i)
		{
			if (Names[i].IsValid())
			{
				const FName Name = Names[i];
				const FCheckpointDataDriven* Row = DataTableRowHandler.DataTable->FindRow<FCheckpointDataDriven>(Name, "");
				// Not using IsValid bc it doesn't work with FStructs -.-
				if (Row && Row->EncounterName.IsEqual(EncounterName))
				{
					FLogger::WarningLog("Trying to add checkpoint already in the table");
					bNewRowExists = true;
				}
			}
		}

		if (!bNewRowExists)
		{
			const int32 Id = GetNextIDForLevel(*UGameplayStatics::GetCurrentLevelName(GetWorld()));

			FCheckpointDataDriven NewRowInfo;
			// I'm trusting the designers
			NewRowInfo.LevelName = *UGameplayStatics::GetCurrentLevelName(GetWorld());
			NewRowInfo.Level = LevelId;
			NewRowInfo.ButtonId = Id;
			NewRowInfo.EncounterName = EncounterName;
			// careful touching this as it modifies the row name
			UniqueCheckpointId = GetNextAvailableCheckpointId();
			const FString NewRowName = " CheckpointEncounter" + FString::FromInt(UniqueCheckpointId);
			NewRowInfo.Identifier = UniqueCheckpointId;

			DataTablePtr->AddRow(FName(NewRowName), NewRowInfo);

			SetIsEncounter(true);
		}
	
	}
}

void ACheckPointTrigger::UnRegisterCheckpointAsEncounter() const
{
	if (IsValid(DataTablePtr))
	{
		if (IsValid(DataTableRowHandler.DataTable))
		{
			auto Names = DataTableRowHandler.DataTable->GetRowNames();
			FName RowName = FName(GetActorNameOrLabel());
			auto Name = DataTablePtr->FindRow<FCheckpointDataDriven>(RowName, "");

			if (!Name)
			{
				FLogger::WarningLog("This checkpoint is not registered");
			}
			else
			{
				DataTablePtr->RemoveRow(RowName);
			}
		}
	}
}



#endif

const FName& ACheckPointTrigger::GetEncounterName() const noexcept
{
	return EncounterName;
}

FVector ACheckPointTrigger::GetArrowForwardVector() const noexcept
{
	return PlayerCheckpointArrow->GetForwardVector();
}

FVector ACheckPointTrigger::GetArrowPosition() const noexcept
{
	return PlayerCheckpointArrow->GetComponentLocation();
}

bool ACheckPointTrigger::CheckHitPlayer(AActor* OtherActor) const
{
	return OtherActor->IsA(APlayerCharacter::StaticClass()) && bIsActive;
}

const FTransform& ACheckPointTrigger::GetArrowTransform() const noexcept
{
	return PlayerCheckpointArrow->GetComponentTransform();
}

void ACheckPointTrigger::SetIsActive(const bool isActive)
{
	bIsActive = isActive;
}
