// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LadyUmbrella/Weapons/Umbrella/UpgradeComponent/UpgradeComponent.h"
#include "UpgradeSlot.generated.h"

DECLARE_DELEGATE_TwoParams(ProgressBarEndedDelegate, int32, int32);

class UHorizontalBox;
class UTextBlock;
class UProgressBar;
class UButton;

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UUpgradeSlot : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
public:
	/**
	 * Prepares the widget to show the info from the Upgrade given in the TPair
	 * @param UpgradeToShow Contained in the UpgradeComponent->Upgrades. Gives the info from an upgrade
	 */
	void SetupUpgradeSlots(const TPair<EUpgradeType, FUpgrade>& UpgradeToShow);

	FORCEINLINE void SetUpgradeId(const int UpgradeIdentifier);
	
	ProgressBarEndedDelegate ProgressEndedDelegate;
	
	UFUNCTION()
	void SetPercentToBar(const int32 UpgradeIdentifier);

	FORCEINLINE int32 GetCurrentLevel() const{ return CurrentLevel; };	

private:

#pragma region UI COMPONENTS POINTERS
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UHorizontalBox* HB_Content;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UTextBlock* TextUpgradeName;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UButton* UpgradeButton;

	// Progress bar for visual feedback
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UProgressBar* UpgradeInProcessProgressBar;

	// Array with the Progress Bars that represent the level of the Upgrade	
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TArray<UProgressBar*> UpgradeLevels; 
	
#pragma endregion UI COMPONENTS POINTERS
	
#pragma region ANIMATION VARIABLES
	
	bool  bIsHolding;
	float HoldTime;
	float MaxHoldTime = 1.f;
	
#pragma endregion ANIMATION VARIABLES

	int32 UpgradeId;
	int32 CurrentLevel;
	float AmountToAddToUpgradeInProcessProgressBar;
	
	UFUNCTION()
	void ProgressBarForward();
	UFUNCTION()
	void ResetProgressBar();
	
};
