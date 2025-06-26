// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../Menu.h"
#include "ChapterSelectorMenu.generated.h"

class UEncounterMenu;
class UButton;
/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UChapterSelectorMenu : public UMenu
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UAnimatedButton* WGMenuButtonFirstLevel;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UAnimatedButton* WGMenuButtonSecondLevel;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UAnimatedButton* WGMenuButtonThirdLevel;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UAnimatedButton* WGMenuButtonFourthLevel;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UAnimatedButton* WGMenuButtonFifthLevel;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UAnimatedButton* WGMenuButtonSixthLevel;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UEncounterMenu* WBEncounterMenu;

	UFUNCTION()
	void OnHoverButtonFirstLevel();
	UFUNCTION()
	void OnHoverButtonSecondLevel();
	UFUNCTION()
	void OnHoverButtonThirdLevel();
	UFUNCTION()
	void OnHoverButtonFourthLevel();
	UFUNCTION()
	void OnHoverButtonFifthLevel();
	UFUNCTION()
	void OnHoverButtonSixthLevel();
	
	const FName FirstLevelName	= "L_Streets_Persistent";
	const FName SecondLevelName = "L_Persistent_Museum";
	const FName ThirdLevelName	= "L_Persistent_RoofTops";
	const FName FourthLevelName = "L_ConstructionRooftops_PersistentLevel";
	const FName FifthLevelName	= "L_Building_PersistentLevel";
	const FName SixthLevelName	= "L_Sniper_Persistent";
	
public:
	virtual void NativeConstruct() override;
	UFUNCTION()
	void OpenLevel1();
	UFUNCTION()
	void OpenLevel2();
	UFUNCTION()
	void OpenLevel3();
	UFUNCTION()
	void OpenLevel4();
	UFUNCTION()
	void OpenLevel5();
	
};
