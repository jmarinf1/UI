// 
// Crosshair.h
// 
// Crosshair interface for the main player. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
//


#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Crosshair.generated.h"

class APlayerCharacter;
class AUmbrella;

/**
 * 
 */

UCLASS()
class LADYUMBRELLA_API UCrosshair : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	UFUNCTION(BlueprintCallable, Category = Visibility)
	void Show();
	
	UFUNCTION(BlueprintCallable, Category = Visibility)
	void Hide();

	void UpdateCrosshairState(const bool IsAiming, const bool IsShieldOpen, const bool IsOnEnemy);
	
	//Need to add functionality to these functions to C++ not blueprint
	UFUNCTION(BlueprintImplementableEvent)
	void ChangeColorRed();
	
	UFUNCTION(BlueprintImplementableEvent)
	void ResetColor();
};
