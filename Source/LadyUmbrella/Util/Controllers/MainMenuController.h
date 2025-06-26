// 
// MainMenuController.h
// 
// Controller for the main menu. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainMenuController.generated.h"

class UMainMenu;
/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API AMainMenuController : public APlayerController
{
	GENERATED_BODY()

	UPROPERTY()
	UMainMenu* MainMenuInstance;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UMainMenu> MainMenuWidget;

	virtual void BeginPlay() override;
};
