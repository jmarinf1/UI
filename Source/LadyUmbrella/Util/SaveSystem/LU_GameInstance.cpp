// Fill out your copyright notice in the Description page of Project Settings.

#include "LU_GameInstance.h"
#include "LU_SaveGame.h"
#include "SaveGameSlotNames.h"
#include "SaveInterface.h"
#include "Kismet/GameplayStatics.h"
#include "LadyUmbrella/Util/Logger.h"

class FAssetRegistryModule;

ULU_GameInstance::ULU_GameInstance()
{
    SaveFile = Cast<ULU_SaveGame>(UGameplayStatics::CreateSaveGameObject(ULU_SaveGame::StaticClass()));
    // We initalize the SaveGameSlot to "SaveSlot1"
    // This is not needed since MainMenu::NewGame button creates the SaveSlot1
    CurrentSaveSlot = SaveGameSlots::SaveSlot1;
    
}

void ULU_GameInstance::Init()
{
    Super::Init();

    ControlTypeCounter = -1;
}

void ULU_GameInstance::CreateGameSaveObject(const FString& SaveSlot)
{
    //SaveFile = Cast<ULU_SaveGame>(UGameplayStatics::CreateSaveGameObject((ULU_SaveGame::StaticClass())));
    if (IsValid(SaveFile))
    {
       UGameplayStatics::SaveGameToSlot(SaveFile, SaveSlot, 0);    
    }
}

void ULU_GameInstance::SaveGame(const FString& SaveSlot)
{
    //if (ULU_SaveGame* DataToSave = Cast<ULU_SaveGame>(UGameplayStatics::CreateSaveGameObject((ULU_SaveGame::StaticClass()))))
    // meter checkers del SaveSlot que ahora mismo puede entrar vacía a la función
    checkf(!SaveSlot.IsEmpty(), TEXT("SaveSlot (Save File Name) is empty when calling ULU_GameInstance::SaveGame"));
    
   if (IsValid(SaveFile))
   {
       SaveFile->PlayerData = GI_SaveStructPlayer;
       // do the same with the interfaces
       UGameplayStatics::SaveGameToSlot(SaveFile, SaveSlot, 0);
   }
   else if (!UGameplayStatics::DoesSaveGameExist(SaveSlot, 0))
   {
       CreateGameSaveObject(SaveSlot);
   } 
}

void ULU_GameInstance::LoadGame(const FString& SaveSlot = SaveGameSlots::SaveSlot1)
{    
    if (const ULU_SaveGame* DataToLoad = Cast<ULU_SaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlot, 0)))
    {
        // Encapsular en una funcion
        GI_SaveStructPlayer = DataToLoad->PlayerData;
        // Call a function to all components that load the data into them
        // check all actors that implement the interface of saving the game
        //CurrentSaveSlot = SaveSlot;
        UGameplayStatics::GetAllActorsWithInterface(GetWorld(), USaveInterface::StaticClass(), ActorsWithSaveData);

        if (ActorsWithSaveData.Num() > 0)
        {
            for (AActor* Actor : ActorsWithSaveData)
            {
                ISaveInterface* SaveInterface = ValidateSaveInterface(Actor);
                if (SaveInterface)
                {
                    SaveInterface->LoadGame();
                }
            }
        }
    }
    // this might get deleted, why would we want to create a saveslot if it doesn't exist? 
    else if (!UGameplayStatics::DoesSaveGameExist(SaveSlot, 0))
    {
        CreateGameSaveObject(SaveSlot);
        //CurrentSaveSlot = SaveSlot;
    }
    // This assign will depend on whether we create a saveslot that doesn't exist or not
    CurrentSaveSlot = SaveSlot;
}

void ULU_GameInstance::SaveDataToGameInstance()
{
    UGameplayStatics::GetAllActorsWithInterface(GWorld, USaveInterface::StaticClass(), ActorsWithSaveData);

    if (ActorsWithSaveData.Num() > 0)
    {
        for (AActor* Actor : ActorsWithSaveData)
        {
            ISaveInterface* SaveInterface = ValidateSaveInterface(Actor);
            if (SaveInterface)
            {
                SaveInterface->SaveGame();
            }
        }
    }
}

void ULU_GameInstance::SaveCheckPoint()
{
    UGameplayStatics::GetAllActorsWithInterface(GWorld, USaveInterface::StaticClass(), ActorsWithSaveData);

    if (ActorsWithSaveData.Num() > 0)
    {
        for (AActor* Actor : ActorsWithSaveData)
        {
            ISaveInterface* SaveInterface = ValidateSaveInterface(Actor);
            if (SaveInterface)
            {
                SaveInterface->SaveDataCheckPoint();
            }
        }
    }
}

void ULU_GameInstance::LoadCheckpoint()
{
    UGameplayStatics::GetAllActorsWithInterface(GWorld, USaveInterface::StaticClass(), ActorsWithSaveData);
    if (ActorsWithSaveData.Num() > 0)
    {
        for (AActor* Actor : ActorsWithSaveData)
        {
            ISaveInterface* SaveInterface = ValidateSaveInterface(Actor);
            if (SaveInterface)
            {
                SaveInterface->LoadDataCheckPoint();
            }
        }
    }
}

void ULU_GameInstance::OpenLevelFromLoadGame()
{
    FName LastLevelPlayed;
    for (auto It = SaveFile->ChaptersProgress.CreateIterator(); It; ++It)
    {
        if (It.Value().bLastChapterPlayed)
        {
            LastLevelPlayed = It.Key();
            break;
        }
    }
    // Bind to this class to know when a level it's played
    FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ULU_GameInstance::OpenLevelFromLoadGameDelegate);
    UGameplayStatics::OpenLevel(GetWorld(), LastLevelPlayed);
    
}

void ULU_GameInstance::OpenLevelFromLoadGameDelegate(UWorld* World)
{
    LoadGame();
}

ISaveInterface* ULU_GameInstance::ValidateSaveInterface(AActor* ActorToValidate)
{
    ISaveInterface* ReturnPtr = nullptr;
    if (ActorToValidate->GetClass()->ImplementsInterface(USaveInterface::StaticClass() ))
    {
        ReturnPtr = Cast<ISaveInterface>(ActorToValidate);
    }
    return ReturnPtr;
}


FString ULU_GameInstance::GetPlatformName() const
{
    return PlatformName;
}

void ULU_GameInstance::SetPlatformName(FString NewPlatformName)
{
    PlatformName = NewPlatformName;
}

int ULU_GameInstance::GetControlTypeCounter() const
{
    return ControlTypeCounter;
}

void ULU_GameInstance::SetControlTypeCounter(const int NewControlTypeCounter)
{
    ControlTypeCounter = NewControlTypeCounter;
}