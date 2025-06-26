//  
// Menu.h
// 
// Generic Menu with functionalities for all menus. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Menu.generated.h"

class UAnimatedButton;

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UMenu : public UUserWidget
{
	GENERATED_BODY()

	void JoyStickNavigation(const float AxisY, int32& NewIndex);
	void KeyNavigation(const bool bMoveForward, const bool bMoveBackward, int32& NewIndex);

public:
	
	//GamepadTesting
	void MenuNavigation(const bool bIsAnalog, const float AxisY, const bool bMoveForward, const bool bMoveBackward);

	FORCEINLINE TArray<UAnimatedButton*>& GetButtons() { return Buttons; }
	FORCEINLINE int32 GetNavigationIndex() const { return NavigationIndex; }
	FORCEINLINE float GetJoyStickDeadZone() const { return JoyStickDeadZone; }
	FORCEINLINE bool GetCanNavigate() const { return CanNavigate; }

	FORCEINLINE void SetButtons(const TArray<UAnimatedButton*>& NewButtons) { Buttons = NewButtons; }
	FORCEINLINE void SetNavigationIndex(const int32 NewIndex) { NavigationIndex = NewIndex; }
	FORCEINLINE void SetJoyStickDeadZone(const float NewThreshold) { JoyStickDeadZone = NewThreshold; }
	FORCEINLINE void SetCanNavigate(const bool NewCanNavigate) { CanNavigate = NewCanNavigate; }

private:
	
	TArray<UAnimatedButton*> Buttons;
	int32 NavigationIndex;
	float JoyStickDeadZone;
	bool CanNavigate;

protected:

	virtual void NativeConstruct() override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	//
};
