// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "FSaveStructPlayer.h"
#include "FChaptersProgress.h"
#include "LU_SaveGame.generated.h"

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API ULU_SaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	ULU_SaveGame();

	UPROPERTY(VisibleAnywhere)
	FString SaveSlotGame;
	
	UPROPERTY(VisibleAnywhere)
	FSaveStructPlayer PlayerData;
	
	/**
	 * TODO: This has to be changed, having the checkpoint as a String is dangerous because
	 * same names can be used for different actors in different names
	*/
	
	UPROPERTY(VisibleAnywhere)
	TMap<FString,bool> CheckPointsUnlocked;

	/**
	* ChaptersProgress: Contains the Chapters and the progress of them in game
	* @details FChaptersProgress: Is a struct that contains the state of the Chapter,
	* divided in bIsCompleted and bIsLastLevel
	* @details bIsCompleted: Marks a Chapter as completed
	* @details bIsLastLevel: Marks the Chapter as last reached for Loading Game purposes
	* @details FString has to be the names defined in
	*/
	UPROPERTY(VisibleAnywhere)
	TMap<FName, FChaptersProgress> ChaptersProgress;
	
};