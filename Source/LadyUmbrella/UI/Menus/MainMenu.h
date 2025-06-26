//  
// MainMenu.h
// 
// Main Menu interface for the game. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "LadyUmbrella/UI/Menu.h"
#include "MainMenu.generated.h"

class UVerticalBox;

UCLASS()
class LADYUMBRELLA_API UMainMenu : public UMenu
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, Category = "Visibility")
	void Show();
	
	UFUNCTION(BlueprintCallable, Category = "Functionality")
	void NewGame();
	
	UFUNCTION(BlueprintCallable, Category = "Functionality")
	void ExitDesktop();

	FORCEINLINE UAnimatedButton* GetNewGameButton() const { return NewGameButton; }
	FORCEINLINE UAnimatedButton* GetContinueButton() const { return ContinueButton; }
	FORCEINLINE UAnimatedButton* GetSettingsButton() const { return SettingsButton; }
	FORCEINLINE UAnimatedButton* GetExitGameButton() const { return ExitGameButton; }

	FORCEINLINE void SetNewGameButton(UAnimatedButton* Value) { NewGameButton = Value; }
	FORCEINLINE void SetContinueButton(UAnimatedButton* Value) { NewGameButton = Value; }
	FORCEINLINE void SetSettingsButton(UAnimatedButton* Value) { NewGameButton = Value; }
	FORCEINLINE void SetExitGameButton(UAnimatedButton* Value) { ExitGameButton = Value; }

private:

	UPROPERTY(BlueprintReadWrite, Category = "Buttons", meta = (BindWidget, AllowPrivateAccess = "true"))
	UAnimatedButton* NewGameButton;
	
	UPROPERTY(BlueprintReadWrite, Category = "Buttons", meta = (BindWidget, AllowPrivateAccess = "true"))
	UAnimatedButton* ContinueButton;

	UPROPERTY(BlueprintReadWrite, Category = "Buttons", meta = (BindWidget, AllowPrivateAccess = "true"))
	UAnimatedButton* SettingsButton;
	
	UPROPERTY(BlueprintReadWrite, Category = "Buttons", meta = (BindWidget, AllowPrivateAccess = "true"))
	UAnimatedButton* ExitGameButton;

	UPROPERTY(BlueprintReadWrite, Category = "Vertical Box", meta = (BindWidget, AllowPrivateAccess = "true"))
	UVerticalBox* MenuButtonsContainer;
	
protected:
	UFUNCTION()
	void ContinueGame();
	//Gamepad testing
	virtual void NativeConstruct() override;
	virtual FReply NativeOnAnalogValueChanged(const FGeometry& InGeometry, const FAnalogInputEvent& InAnalogEvent) override;
	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	//
};