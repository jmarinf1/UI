
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LadyUmbrella/UI/Menu.h"
#include "LadyUmbrella/UI/Menus/WorkbenchElements/UpgradeSlot.h"
#include "LadyUmbrella/Weapons/Umbrella/UpgradeComponent/Upgrade.h"
#include "LadyUmbrella/Weapons/Umbrella/UpgradeComponent/UpgradeType.h"
#include "WorkBenchMenu.generated.h"



class UTextBlock;
class UUpgradeComponent;
class UVerticalBox;

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UWorkBenchMenu : public UMenu
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UTextBlock* AmountOfPlayerPieces;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UVerticalBox* VB_Upgrades;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UVerticalBox* VB_Upgrades_Shield;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UVerticalBox* VB_Upgrades_Shotgun;

#pragma region BUTTONS
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UButton* ExitButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UButton* Button_Left;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UButton* Button_Right;
#pragma endregion BUTTONS
	/*
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UVerticalBox* VB_DescriptionRegion;
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TArray<UUpgradeSlot*> UpgradeSlotArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUpgradeSlot> UpgradeSlotClass;

	int32 PiecesOfThePlayer;

	// Unused for now
	UPROPERTY()
	UUpgradeComponent* UmbrellaUpgradeComponent;

	UFUNCTION()
	void CloseMenu();

	UFUNCTION()
	void OnClickButton();

	// IMPORTANT: If we have to add more upgrades, this value will MOST LIKELY change
	// This is used to avoid having 3 maps. This way we know how many upgrades are
	// assigned to the Shotgun and assigned to the Shield functionality
	const uint8 Upgrades_Separator = 4;
	
public:
	
	void SetPiecesElement(const int32 NewPieces) const;

	/*
	 * Retrieves the Umbrella Upgrade Component and creates UUpgradeSlot widgets with their info
	 */
	void ReadUmbrellaUpgrades(const TMap<EUpgradeType, FUpgrade>& Upgrades);
	//void ReadUmbrellaUpgrades(const UUpgradeComponent& CurrentUpgrades);

	void OnUpgradePressed(const int32 UpgradePressed, const int32 CurrentLevel);

	void OpenMenu() const;

	FORCEINLINE void SetPiecesOfThePlayer(const int32 NewPieces) { PiecesOfThePlayer = NewPieces; };
	FORCEINLINE int32 GetPiecesOfThePlayer () const { return PiecesOfThePlayer; };


protected:
	virtual void NativeConstruct() override;
};
