// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LadyUmbrella/UI/Menu.h"
#include "SaveMenu.generated.h"

class UAnimatedButtonSaveMenu;
/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API USaveMenu : public UMenu
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UAnimatedButtonSaveMenu* AnimatedButtonSaveMenuSlot1;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UAnimatedButtonSaveMenu* AnimatedButtonSaveMenuSlot2;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UAnimatedButtonSaveMenu* AnimatedButtonSaveMenuSlot3;

	bool bIsSaveOrLoad;
	

public:
	UFUNCTION()

	void OnSaveButtonClicked(FString& SaveSlotNamePressed);
	
	virtual void NativeConstruct() override;
	
	
};
