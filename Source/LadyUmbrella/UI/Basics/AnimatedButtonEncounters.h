// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimatedButton.h"
#include "AnimatedButtonEncounters.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEncountersButtonClicked, int32, IdClicked);
/**
 * 
 */

UCLASS()
class LADYUMBRELLA_API UAnimatedButtonEncounters : public UAnimatedButton
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnPressedButton();

	int32 Id;
	
public:
	FORCEINLINE void SetId(const int32 InId) { Id = InId; }
	
	UPROPERTY()
	FOnEncountersButtonClicked OnButtonClicked;
};
