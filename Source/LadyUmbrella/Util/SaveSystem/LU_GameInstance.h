// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FSavePuzzle.h"
#include "Engine/GameInstance.h"
#include "LadyUmbrella/Util/SaveSystem/FSaveStructPlayer.h"
#include "LU_GameInstance.generated.h"

class ISaveInterface;
class ULU_SaveGame;
class ACheckPointTrigger;
/**
 * ULU_GameInstance
 * Represents the game instance class that manages game-wide systems, states, and persistent data.
 * Typically used for orchestrating game-specific logic that persists beyond individual levels or sessions.
 */
UCLASS()
class LADYUMBRELLA_API ULU_GameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	ULU_GameInstance();

	virtual void Init() override;

	FString GetPlatformName() const;
	void SetPlatformName(FString NewPlatformName);

	int GetControlTypeCounter() const;
	void SetControlTypeCounter(const int NewControlTypeCounter);
	
	/**
	* Create Game Save Object
	* Creates a Save Game Object in case it doesn't exist
	*/
	UFUNCTION(BlueprintCallable)
	void CreateGameSaveObject(const FString& SaveSlot);
	/**
	* Save the game data
	* Save the Game Data into the Save Game Object
	*/
	UFUNCTION(BlueprintCallable)
	void SaveGame(const FString& SaveSlot);
	/**
	* Load Game Data
	* Load the data saved from the Save Game Object into the game
	*/
	UFUNCTION(BlueprintCallable)
	void LoadGame(const FString& SaveSlot);
	/**
	 * Get all data from the actors and set it into the GameInstance
	 */
	UFUNCTION(BlueprintCallable)
	void SaveDataToGameInstance();

	UFUNCTION(BlueprintCallable)
	void SaveCheckPoint();

	UFUNCTION(BlueprintCallable)
	void LoadCheckpoint();

	[[nodiscard]] FORCEINLINE ACheckPointTrigger* GetLastCheckpointHit() const { return LastCheckpointHit; }
	FORCEINLINE void SetLastCheckpointHit(ACheckPointTrigger* NewLastCheckpoint) { LastCheckpointHit = NewLastCheckpoint; }

	[[nodiscard]] FORCEINLINE ULU_SaveGame* GetSaveGameFile() const { return SaveFile; }
	/**
	 * Setter should not be created as we manage its creation from GameInstance, but just in case
	 */
	FORCEINLINE void SetSaveFile(ULU_SaveGame* NewSaveFile) { SaveFile = NewSaveFile; }

	[[nodiscard]] FORCEINLINE FString GetCurrentSaveSlot() const { return CurrentSaveSlot; }
	FORCEINLINE void SetCurrentSaveSlot(const FString& NewCurrentSaveSlot) { CurrentSaveSlot = NewCurrentSaveSlot; }

	UFUNCTION()
	void OpenLevelFromLoadGame();

	UFUNCTION()
	void OpenLevelFromLoadGameDelegate(UWorld* World);

	ISaveInterface* ValidateSaveInterface(AActor* ActorToValidate);
	
	/**
	 *	Struct that contains the Player Data Info in the Game Instance. There should only be one
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	FSaveStructPlayer GI_SaveStructPlayer;
	/**
	 * Array of pointers of all the Actors that can save data. Initialized in the constructor.
	 * Marked as deprecated. Will be deleted.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	TArray<AActor*> ActorsWithSaveData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	FSavePuzzle GI_SavePuzzle;

private:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	ACheckPointTrigger* LastCheckpointHit = nullptr;
	
	/**
	 * SaveFile
	 * It seems like save game objects generate different slots if you change the name of it
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	ULU_SaveGame* SaveFile;

	UPROPERTY()
	FString CurrentSaveSlot;

	FString PlatformName;
	int ControlTypeCounter;
};