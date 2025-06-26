// 
// AnimatedButton.h
// 
// Animated Button for UI. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LadyUmbrella/UI/Basics/State/UIState.h"
#include "AnimatedButton.generated.h"

class UButton;
class UOverlay;
class UTextBlock;

UCLASS()
class LADYUMBRELLA_API UAnimatedButton : public UUserWidget
{
	GENERATED_BODY()
protected:
	// Bindings
	UPROPERTY(BlueprintReadWrite, Category = "Button", meta = (BindWidget, AllowPrivateAccess = "true"))
	UButton* Button;

	UPROPERTY(BlueprintReadWrite, Category = "Overlay", meta = (BindWidget, AllowPrivateAccess = "true"))
	UOverlay* TextOverlay;
	
	UPROPERTY(BlueprintReadWrite, Category = "Text", meta = (BindWidget, AllowPrivateAccess = "true"))
	UTextBlock* ButtonName;
	//
	// UMG BP Variables
	//Button
	UPROPERTY(EditAnywhere, Category="Button Color On Idle", meta = (AllowPrivateAccess = true))
	FLinearColor ButtonColorOnIdle;

	UPROPERTY(EditAnywhere, Category="Button Color On Idle", meta = (AllowPrivateAccess = true))
	FLinearColor ButtonColorOnPressed;
	
	UPROPERTY(EditAnywhere, Category="Button Hovered Scale", meta = (AllowPrivateAccess = true))
	float ButtonHoveredX;

	UPROPERTY(EditAnywhere, Category="Button Hovered Scale", meta = (AllowPrivateAccess = true))
	float ButtonHoveredY;

	UPROPERTY(EditAnywhere, Category="Button Hovered Shear", meta = (AllowPrivateAccess = true))
	float ButtonShearX;

	UPROPERTY(EditAnywhere, Category="Button Hovered Shear", meta = (AllowPrivateAccess = true))
	float ButtonShearY;
	//
	//Text
	UPROPERTY(EditAnywhere, Category="Button Text", meta = (AllowPrivateAccess = true))
	FString Text;

	UPROPERTY(EditAnywhere, Category="On Pressed Text Color", meta = (AllowPrivateAccess = true))
	FSlateColor TextColorOnPressed;
	
	UPROPERTY(EditAnywhere, Category="Hovered Text Translation", meta = (AllowPrivateAccess = true))
	float TextHoveredX;

	UPROPERTY(EditAnywhere, Category="Hovered Text Translation", meta = (AllowPrivateAccess = true))
	float TextHoveredY;

	UPROPERTY(EditAnywhere, Category="On Pressed Text Translation", meta = (AllowPrivateAccess = true))
	float TextOnPressedX;

	UPROPERTY(EditAnywhere, Category="On Pressed Text Translation", meta = (AllowPrivateAccess = true))
	float TextOnPressedY;
	//
	//
	// Default Variables
	FVector2D IdleButtonScale;
	FVector2D IdleTextTranslation;
	FSlateColor TextColor;
	EUIState State;
	FLinearColor IdleButtonColor;
	FLinearColor HoveredButtonColor;
	FVector2D IdleButtonShear;
	//
	
public:

	UFUNCTION()
	void Idle();
	
	UFUNCTION()
	void Hovered();

	UFUNCTION()
	void OnPressed();

	UButton* GetButton() const;

	void SetButtonText(const FText& NewText) const;

protected:

	virtual void NativeConstruct() override;
	void UpdateState();
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	virtual void NativeOnFocusLost(const FFocusEvent& InFocusEvent) override;
	void UpdateButtonColor(FLinearColor NewColor) const;
	
};
