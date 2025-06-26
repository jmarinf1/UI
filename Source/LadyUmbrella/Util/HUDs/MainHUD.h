//  
// MainHUD.h
// 
// HUD for the main game. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LadyUmbrella/UI/Menus/CheatMenu.h"
#include "MainHUD.generated.h"

class APlayerCharacter;
class UPauseMenu;
class UPlayerHealth;
class UAmmoCounter;
class UCrosshair;
class USettingsMenu;
class UControlSettings;
class USubtitles;

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API AMainHUD : public AHUD
{
	GENERATED_BODY()

	bool bCheatMenuCanToggle;

public:

	UFUNCTION()
	void OnUmbrellaSpawned(APlayerCharacter* Character);
	
	UFUNCTION(BlueprintCallable)
	void TogglePauseMenu() const;
	void ToggleCheatMenu();
	void EnableToggleCheatMenu();

	UFUNCTION(BlueprintCallable)
	void ToggleSettingsMenu() const;
	
	UFUNCTION(BlueprintCallable)
	void ToggleControlSettingsMenu() const;
	
	UFUNCTION(BlueprintCallable)
	void TogglePlayerHUD() const;

	UFUNCTION(BlueprintCallable)
	void UnTogglePlayerHUD() const;

	void ClearWidgets();
	void AddSubtitle(const FString& CharacterName, const FString& Subtitle) const;

	UPauseMenu* GetPauseMenuInstance() const;
	TSubclassOf<UPauseMenu> GetPauseMenuWidget() const;
	UPlayerHealth* GetPlayerHealthInstance() const;
	TSubclassOf<UPlayerHealth> GetPlayerHealthWidget() const;
	UAmmoCounter* GetAmmoCounterInstance() const;
	TSubclassOf<UAmmoCounter> GetAmmoCounterWidget() const;
	UCrosshair* GetCrosshairInstance() const;
	TSubclassOf<UCrosshair> GetCrosshairWidget() const;
	UCheatMenu* GetCheatMenuInstance() const;
	TSubclassOf<UCheatMenu> GetCheatMenuWidget() const;
	USettingsMenu* GetSettingsMenuInstance() const;
	TSubclassOf<USettingsMenu> GetSettingsMenuWidget() const;
	UControlSettings* GetControlSettingsInstance() const;
	TSubclassOf<UControlSettings> GetControlSettingsWidget() const;
	USubtitles* GetSubtitlesInstance() const;
	TSubclassOf<USubtitles> GetSubtitlesWidget() const;
	
	void SetPauseMenuInstance(const TSubclassOf<UPauseMenu>& NewPauseMenuWidget);
	void SetCheatMenuInstance(const TSubclassOf<UCheatMenu>& NewCheatMenuWidget);
	void SetPlayerHealthInstance(const TSubclassOf<UPlayerHealth>& NewPlayerHealthWidget);
	void SetAmmoCounterInstance(const TSubclassOf<UAmmoCounter>& NewAmmoCounterWidget);
	void SetCrosshairInstance(const TSubclassOf<UCrosshair>& NewCrosshairWidget);
	void SetSettingsMenuInstance(const TSubclassOf<USettingsMenu>& NewSettingsMenuWidget);
	void SetControlSettingsInstance(const TSubclassOf<UControlSettings>& NewControlSettingsWidget);
	void SetSubtitleInstance(const TSubclassOf<USubtitles>& NewSubtitleWidget);

protected:

	virtual void BeginPlay() override;

private:
	
	UPROPERTY()
	UPauseMenu* PauseMenuInstance;

	UPROPERTY()
	UPlayerHealth* PlayerHealthInstance;

	UPROPERTY()
	UAmmoCounter* AmmoCounterInstance;

	UPROPERTY()
	UCrosshair* CrosshairInstance;

	UPROPERTY()
	UCheatMenu* CheatMenuInstance;

	UPROPERTY()
	USettingsMenu* SettingsMenuInstance;

	UPROPERTY()
	UControlSettings* ControlSettingsMenuInstance;

	UPROPERTY()
	USubtitles* SubtitlesInstance;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Menus", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UPauseMenu> PauseMenuWidget;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Menus", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UCheatMenu> CheatMenuWidget;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Menus", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<USettingsMenu> SettingsMenuWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SubMenus", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UControlSettings> ControlSettingsMenuWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HUD", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UPlayerHealth> PlayerHealthWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HUD", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UAmmoCounter> AmmoCounterWidget;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HUD", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UCrosshair> CrosshairWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HUD", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<USubtitles> SubtitlesWidget;
};
