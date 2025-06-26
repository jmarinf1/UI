// 
// MainController.h
// 
// MainController for the main game. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainController.generated.h"

enum class ELanguageCode : uint8;
class APlayerCharacter;
class UInputMappingContext;
class AMainHUD;
class UInputAction;

UCLASS()
class LADYUMBRELLA_API AMainController : public APlayerController
{
	GENERATED_BODY()

	AMainController();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Development Cheats", meta = (AllowPrivateAccess = "true"))
	float NoclipSpeedMultiplier;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Language", meta = (AllowPrivateAccess = "true"))
	ELanguageCode LanguageCode;

#pragma region INPUT MAPPING CONTEXTS
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Mapping Contexts", meta = (AllowPrivateAccess = "true"))
	TArray<UInputMappingContext*> ControllerInputMappingContexts;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Mapping Contexts", meta = (AllowPrivateAccess = "true"))
	TArray<UInputMappingContext*> KeyboardInputMappingContexts;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input Mapping Contexts", meta = (AllowPrivateAccess = "true"))
	int32 ActiveControllerInputMappingContextIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input Mapping Contexts", meta = (AllowPrivateAccess = "true"))
	int32 ActiveKeyboardInputMappingContextIndex;

#pragma endregion
	
#pragma region INPUT ACTIONS
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input Actions", meta = (AllowPrivateAccess = "true"))
	UInputAction* ActivateNextControllerMappingContextAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input Actions", meta = (AllowPrivateAccess = "true"))
	UInputAction* ActivatePreviousControllerMappingContextAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input Actions", meta = (AllowPrivateAccess = "true"))
	UInputAction* ActivateNextKeyboardMappingContextAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input Actions", meta = (AllowPrivateAccess = "true"))
	UInputAction* ActivatePreviousKeyboardMappingContextAction;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input Actions", meta = (AllowPrivateAccess = "true"))
	UInputAction* AimAction;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input Actions", meta = (AllowPrivateAccess = "true"))
	UInputAction* CoverAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input Actions", meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveBetweenCoversAction;;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input Actions", meta = (AllowPrivateAccess = "true"))
	UInputAction* CycleUmbrellaBulletAction;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input Actions", meta = (AllowPrivateAccess = "true"))
	UInputAction* GrappleAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input Actions", meta = (AllowPrivateAccess = "true"))
	UInputAction* InteractingAction;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input Actions", meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input Actions", meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input Actions", meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input Actions", meta = (AllowPrivateAccess = "true"))
	UInputAction* ReloadAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input Actions", meta = (AllowPrivateAccess = "true"))
	UInputAction* ShootUmbrellaAction;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input Actions", meta = (AllowPrivateAccess = "true"))
	UInputAction* ToggleShieldAction;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input Actions", meta = (AllowPrivateAccess = "true"))
	UInputAction* HoldShieldAction;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input Actions", meta = (AllowPrivateAccess = "true"))
	UInputAction* PauseInputAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Actions", meta = (AllowPrivateAccess = "true"))
	UInputAction* CheatInputAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Actions", meta = (AllowPrivateAccess = "true"))
	UInputAction* SwapElectricAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Actions", meta = (AllowPrivateAccess = "true"))
	UInputAction* SwapHookAction;

#pragma endregion

#pragma region VARIABLES
private:
	UPROPERTY()
	AMainHUD* MainHUD;

#pragma endregion 

public:

	void HandleTogglePauseMenu();
	void HandleToggleCheatMenu();
	void EnableToggleCheatMenu();
	void HandleToggleSettingsMenu() const;
	void HandleToggleControlSettingsMenu() const;
	void HandleTogglePlayerHUD() const;
	void HandleUnTogglePlayerHUD() const;
	void HandleUmbrellaSpawned() const;
	void HandleClearWidgets() const;
	
	AMainHUD* GetMainHUD() const;
	UInputAction* GetPauseInputAction() const;
	UInputAction* GetCheatInputAction() const;
	UInputAction* GetAimInputAction() const { return AimAction; }
	UInputAction* GetHoldShieldInputAction() const { return HoldShieldAction; }
	
	void SetMainHUD(AMainHUD* NewMainHUD);

	/****************************/
	/*          CHEATS          */
	/****************************/
	UFUNCTION(Exec) void Godmode();
	UFUNCTION(Exec) void NoClip();
	UFUNCTION(Exec) void InfiniteAmmo();
	UFUNCTION(Exec) void ReloadLevel();
	UFUNCTION(Exec) void SetAmmoCurrent(const int Amount);
	UFUNCTION(Exec) void SetAmmoReserve(const int Amount);
	UFUNCTION(Exec) void SetComponents(const int Amount);

protected:

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:

#pragma region INPUT FUNCTIONS

public:
	void SetupPlayerInput(APlayerCharacter* _PlayerCharacter);
	
	FORCEINLINE int32 GetControllerInputMappingContextIndex() const { return ActiveControllerInputMappingContextIndex; };
	FORCEINLINE int32 GetKeyboardInputMappingContextIndex() const { return ActiveKeyboardInputMappingContextIndex; };
	FORCEINLINE ELanguageCode GetLanguageCode() const { return LanguageCode; }
	
	UFUNCTION()
	UInputMappingContext* GetControllerMappingContextAtIndex(int _InputMappingContextIndex) const;
	UFUNCTION()
	UInputMappingContext* GetKeyboardMappingContextAtIndex(int _InputMappingContextIndex) const;

	void SetActiveControllerInputMappingContext(int _NewInputMappingContextIndex);
	void SetActiveKeyboardInputMappingContext(int _NewInputMappingContextIndex);

public:
	void ActivateNextAvailibleControllerInputMappingContext();
	void ActivatePreviousAvailibleControllerInputMappingContext();

	void ActivateNextAvailibleKeyboardInputMappingContext();
	void ActivatePreviousAvailibleKeyboardInputMappingContext();

#pragma endregion 
	
};
