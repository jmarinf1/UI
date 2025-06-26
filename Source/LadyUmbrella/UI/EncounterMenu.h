//  
// EncounterMenu.h
// 
// Implementation of the GenericProgressBar class.
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "Menu.h"
#include "LadyUmbrella/Util/DataDrivenStructs/FCheckpointDataDriven.h"
#include "LadyUmbrella/UI/Menus/ChapterSelector/FEnemiesEncounter.h"
#include "EncounterMenu.generated.h"

class ACheckPointTrigger;
struct FGameplayTag;
struct FCheckpointDataDriven;

class UAnimatedButtonEncounters;
class UScrollBox;
/**
 * UEncounterMenu is responsible for managing the interactions and UI components
 * in the Encounter menu of the game. It builds a dynamic UI based on available
 * encounters, loads data from a data table, and handles button interactions for
 * navigating between encounters.
 */
UCLASS()
class LADYUMBRELLA_API UEncounterMenu : public UMenu
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

	UFUNCTION()
	void ProcessClickOfButtons(int32 Id);
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UAnimatedButtonEncounters> AnimatedButtonSubclass;

	/**
	 * This map contains the IDs for choosing which encounter we load
	 * @param Key: Represents the position in the Map of the Encounter. Will be indicated by the button in the menu.
	 * @param FEnemiesEncounter: Struct that holds the name of the map the encounter is in and an ID inside that map, marking which encounter will the player be placed on
	 */
	UPROPERTY(VisibleAnywhere)
	TMap<int32, FEnemiesEncounter> Encounters;

public:
	
	/**
	 * UI Pointers to the ScrollBoxes
	 */
#pragma region SCROLLBOXPTRS
	// Hacer todos uno en lugar de que sean varios 
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UScrollBox* EncounterScrollBox_Chapter1;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UScrollBox* EncounterScrollBox_Chapter2;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UScrollBox* EncounterScrollBox_Chapter3;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UScrollBox* EncounterScrollBox_Chapter4;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UScrollBox* EncounterScrollBox_Chapter5;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UScrollBox* EncounterScrollBox_Chapter6;
	
#pragma endregion
	UPROPERTY()
	FString CurrentLevel;

#pragma region DATADRIVEN
	/**
	 * Pointer to the DataTable that has all Data
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data Driven", DisplayName = "Player Table Info",meta = (AllowPrivateAccess = "true"))
	UDataTable* DataTablePtr;

	/**
	 * Element to control Rows from DataTablePtr Type
	 */
	UPROPERTY(EditAnywhere, Category = "Data Driven", DisplayName = "Row Type", meta = (AllowPrivateAccess = "true"))
	FDataTableRowHandle DataTableRowHandler;

	UPROPERTY(EditAnywhere, Blueprintable, Category = "Data Driven", DisplayName = "Name of the Row", meta = (AllowPrivateAccess = "true"))
	FName RowName;
	/**
	* Struct that represents the data that there is inside the DataTablePtr. Not needed for now, but will come in handy
	*/
	UPROPERTY(VisibleAnywhere, Category = "Data Driven", DisplayName = "Checkpoint Struct")
	FCheckpointDataDriven FCheckpointDataDrivenStruct;

	/**
	 * Called when wanted to set Player Attributes from DataTablePtr
	 */
	UFUNCTION()
	void LoadCheckpointsFromDataTable();

#pragma endregion
	
	void GetAllEncountersCheckpoints(const UWorld* WorldContext,
		const TSubclassOf<AActor>& ActorClass, TArray<ACheckPointTrigger*>& OutActors);
	
};
