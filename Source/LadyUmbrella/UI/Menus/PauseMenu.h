//  
// PauseMenu.h
// 
// Pause menu interface for the main player. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "LadyUmbrella/UI/Menu.h"
#include "PauseMenu.generated.h"

class USettingsMenu;
class UVerticalBox;

UCLASS()
class LADYUMBRELLA_API UPauseMenu : public UMenu
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Visibility")
	void Show();

	UFUNCTION(BlueprintCallable, Category = "Functionality")
	void Resume();

	UFUNCTION(BlueprintCallable, Category = "Functionality")
	void Save();
	
	UFUNCTION(BlueprintCallable, Category = "Functionality")
	void Load();

	UFUNCTION(BlueprintCallable, Category = "Functionality")
	void Settings();
	
	UFUNCTION(BlueprintCallable, Category = "Functionality")
	void ExitMainMenu();

	UFUNCTION(BlueprintCallable, Category = "Functionality")
	void ExitDesktop();
	
	FORCEINLINE UAnimatedButton* GetResumeButton() const { return ResumeButton; }
	FORCEINLINE UAnimatedButton* GetSaveButton() const { return SaveButton; }
	FORCEINLINE UAnimatedButton* GetLoadButton() const { return LoadButton; }
	FORCEINLINE UAnimatedButton* GetSettingsButton() const { return SettingsButton; }
	FORCEINLINE UAnimatedButton* GetExitMainMenuButton() const { return ExitMainMenuButton; }
	FORCEINLINE UAnimatedButton* GetExitDesktopButton() const { return ExitDesktopButton; }

	FORCEINLINE void SetResumeButton(UAnimatedButton* NewResumeButton) { ResumeButton = NewResumeButton; }
	FORCEINLINE void SetSaveButton(UAnimatedButton* NewSaveButton) { SaveButton = NewSaveButton; }
	FORCEINLINE void SetLoadButton(UAnimatedButton* NewLoadButton) { LoadButton = NewLoadButton; }
	FORCEINLINE void SetSettingsButton(UAnimatedButton* NewSettingsButton) { LoadButton = NewSettingsButton; }
	FORCEINLINE void SetExitMainMenuButton(UAnimatedButton* NewExitMainMenuButton) { ExitMainMenuButton = NewExitMainMenuButton; }
	FORCEINLINE void SetExitDesktopButton(UAnimatedButton* NewExitDesktopButton) { ExitDesktopButton = NewExitDesktopButton; }
	
private:

	UPROPERTY(BlueprintReadWrite, Category = "Buttons", meta = (BindWidget, AllowPrivateAccess = "true"))
	UAnimatedButton* ResumeButton;

	UPROPERTY(BlueprintReadWrite, Category = "Buttons", meta = (BindWidget, AllowPrivateAccess = "true"))
	UAnimatedButton* SaveButton;

	UPROPERTY(BlueprintReadWrite, Category = "Buttons", meta = (BindWidget, AllowPrivateAccess = "true"))
	UAnimatedButton* LoadButton;

	UPROPERTY(BlueprintReadWrite, Category = "Buttons", meta = (BindWidget, AllowPrivateAccess = "true"))
	UAnimatedButton* SettingsButton;

	UPROPERTY(BlueprintReadWrite, Category = "Buttons", meta = (BindWidget, AllowPrivateAccess = "true"))
	UAnimatedButton* ExitMainMenuButton;

	UPROPERTY(BlueprintReadWrite, Category = "Buttons", meta = (BindWidget, AllowPrivateAccess = "true"))
	UAnimatedButton* ExitDesktopButton;
	
	UPROPERTY(BlueprintReadWrite, Category = "Vertical Box", meta = (BindWidget, AllowPrivateAccess = "true"))
	UVerticalBox* MenuButtonsContainer;
	
protected:
	
	//Gamepad testing
	virtual void NativeConstruct() override;
	virtual FReply NativeOnAnalogValueChanged(const FGeometry& InGeometry, const FAnalogInputEvent& InAnalogEvent) override;
	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	//
};