//  
// EncounterMenu.cpp
// 
// Implementation of the GenericProgressBar class.
// 
// Copyright Zulo Interactive. All Rights Reserved.
//


#include "EncounterMenu.h"

#include "EngineUtils.h"
#include "LadyUmbrella/UI/Basics/AnimatedButtonEncounters.h"
#include "Components/ScrollBox.h"
#include "Kismet/GameplayStatics.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Environment/CheckPointTrigger.h"
#include "LadyUmbrella/Util/Logger.h"
#include "LadyUmbrella/Util/SaveSystem/LU_GameInstance.h"

class UUpgradeSlot;

void UEncounterMenu::NativeConstruct()
{
	Super::NativeConstruct();
	
	LoadCheckpointsFromDataTable();
	
	if (IsValid(AnimatedButtonSubclass))
	{
		for (auto It = Encounters.begin(); It != Encounters.end(); ++It)
		{
			UAnimatedButtonEncounters* NewButton = CreateWidget<UAnimatedButtonEncounters>(GetWorld(), AnimatedButtonSubclass);
			if (IsValid(NewButton))
			{
				UScrollBox* CurrentChapterScrollBox = nullptr;
				// Figure out which scrollbox we have to add the button to
				int32 ChapterId = It.Value().LevelId;
				switch (It.Value().LevelId)	
				{
					case 1:
						CurrentChapterScrollBox = EncounterScrollBox_Chapter1;
						break;
					case 2:
						CurrentChapterScrollBox = EncounterScrollBox_Chapter2;
					break;
					case 3:
						CurrentChapterScrollBox = EncounterScrollBox_Chapter3;
						break;
					case 4:
						CurrentChapterScrollBox = EncounterScrollBox_Chapter4;
						break;
					case 5:
						CurrentChapterScrollBox = EncounterScrollBox_Chapter5;
						break;
					case 6:
						CurrentChapterScrollBox = EncounterScrollBox_Chapter6;
						break;
					
						default:
							break;
				}
				
				if (!IsValid(CurrentChapterScrollBox))
				{
					return;	
				}
				
				CurrentChapterScrollBox->AddChild(NewButton);
				CurrentChapterScrollBox->SetVisibility(ESlateVisibility::Visible);
				NewButton->SetButtonText(FText::FromName(It.Value().EncounterName));
				NewButton->SetId(It.Value().UniqueId);
				
				NewButton->OnButtonClicked.AddDynamic(this, &UEncounterMenu::ProcessClickOfButtons);
				// Slots es el array que contiene a los children
			}
		}
	}
}

void UEncounterMenu::LoadCheckpointsFromDataTable()
{
	if (IsValid(DataTablePtr))
	{
		if (IsValid(DataTableRowHandler.DataTable))
		{
			TArray<FName> Names = DataTableRowHandler.DataTable->GetRowNames();
			
			if (Names.Num() < 0)
			{
				return;
			}
						
			for (int32 i = 0; i < Names.Num(); ++i)
			{
				if (Names[i].IsValid())
				{
					const FName Name = Names[i];
					const FCheckpointDataDriven* Row = DataTableRowHandler.DataTable->FindRow<FCheckpointDataDriven>(Name, "");
					// Not using IsValid bc it doesn't work with FStructs -.-
					if (Row)
					{
						Encounters.Add
						(
							i ,
				{
							Row->ButtonId,
					 Row->Level,
							Row->LevelName,
					Row->Identifier,
							Row->EncounterName,
							0
				          }
						);
					}
				}
			}
		}
	}
}

void UEncounterMenu::ProcessClickOfButtons(int32 Id)
{
	// Pasos:
	/*
	 * Ya se ha clicado el botón, por lo que sabemos cuál es
	 * Se accede al mapa con el id, comprobamos que los contenidos piola.
	 * Accedemos al String del nombre del mapa y lo cargamos.
	 * Una vez esté cargado el nivel, buscamos entre los checkpoints y accedemos al que tenga el id del mapa.
	 * Gestionar las clases de los checkpoints si es necesario
	 */

	if (!Encounters.Find(Id))
	{
		return;
	}
	const FName WorldChosenName = Encounters.Find(Id)->LevelName;
	
	// aqui estaría bien meter una pantalla de carga o algo similar para disimular la carga de elementos
	
	FLatentActionInfo action;
	action.Linkage = 1;

	// llevar cuiddado con esto, por ahora funciona pero hay que ver si funciona para todos...
	//UGameplayStatics::OpenLevel(GetWorld(), WorldChosenName);
	UGameplayStatics::LoadStreamLevel(GetWorld(), WorldChosenName, true, false, action);
	TArray<ACheckPointTrigger*> CheckPointTriggers;
	GetAllEncountersCheckpoints(GetWorld(), ACheckPointTrigger::StaticClass() ,CheckPointTriggers);
	
	if (CheckPointTriggers.Num() > 0)
	{
		for (int32 i = 0; i < CheckPointTriggers.Num(); ++i)
		{
			ACheckPointTrigger* CheckpointSearched = CheckPointTriggers[i];
			
			if (IsValid(CheckpointSearched) && CheckpointSearched->GetEncounterName().IsEqual( Encounters.Find(Id)->EncounterName) )
			{
				FLogger::DebugLog("Checkpoint found");
				
				ULU_GameInstance* GameInstance = Cast<ULU_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
				if (IsValid(GameInstance))
				{
					GameInstance->SetLastCheckpointHit(CheckpointSearched);
				}
				
				APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
				if (IsValid(PlayerCharacter))
				{
					FTransform ArrowTransform = CheckpointSearched->GetArrowTransform();
					FLogger::DebugLog("%i", ArrowTransform.GetLocation().X);
					FLogger::DebugLog("%i", ArrowTransform.GetLocation().Y);
					FLogger::DebugLog("%i", ArrowTransform.GetLocation().Z);

					PlayerCharacter->SetActorLocation(CheckpointSearched->GetArrowTransform().GetLocation());
				}
			}
		}
	}
}

void UEncounterMenu::GetAllEncountersCheckpoints(const UWorld* WorldContext, const TSubclassOf<AActor>& ActorClass, TArray<ACheckPointTrigger*>& OutActors)
{
	OutActors.Reset();

	if (ActorClass)
	{
		if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull))
		{
			for (TActorIterator<AActor> It(World, ActorClass); It; ++It)
			{
				ACheckPointTrigger* Actor = Cast<ACheckPointTrigger>(*It);
				if (IsValid(Actor) && Actor->GetIsEncounter())
				{
					OutActors.Add(Actor);
				}
			}
		}
	}
}