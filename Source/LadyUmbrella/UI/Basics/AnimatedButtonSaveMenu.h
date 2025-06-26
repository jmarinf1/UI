// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimatedButton.h"
#include "AnimatedButtonSaveMenu.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSaveButtonClicked, FString&, SaveSlotNamePressed);

/**
 * Animated button class specifically designed for the Save Menu.
 * Inherits from UAnimatedButton to provide additional functionality
 * tailored to interact with save slots.
 */
UCLASS()
class LADYUMBRELLA_API UAnimatedButtonSaveMenu : public UAnimatedButton
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta= ( AllowPrivateAccess=true ))
	FString SaveSlotName;
	
public:

	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnPressedButton();
	
	FOnSaveButtonClicked OnButtonClicked;
	/**
	 *	SetSaveSlotNameString: 
	 * @param SaveSlotNamePressed: Sets the String of the button that will be passed into the Game instance to know
	 * which SaveSlot has to be operated with
	 */
	UFUNCTION()
	FORCEINLINE void SetSaveSlotNameString(const FString& SaveSlotNamePressed){ SaveSlotName = SaveSlotNamePressed;};
		
};
