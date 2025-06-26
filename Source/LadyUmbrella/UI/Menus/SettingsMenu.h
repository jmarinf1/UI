// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LadyUmbrella/UI/Menu.h"
#include "SettingsMenu.generated.h"

class UAnimatedButton;
class UHorizontalBox;

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API USettingsMenu : public UMenu
{
	GENERATED_BODY()

	// Bindings
	UPROPERTY(BlueprintReadWrite, Category = "Animated Button", meta = (BindWidget, AllowPrivateAccess = "true"))
	UAnimatedButton* ControlsButton;

	UPROPERTY(BlueprintReadWrite, Category = "Animated Button", meta = (BindWidget, AllowPrivateAccess = "true"))
	UAnimatedButton* GeneralButton;
	
	UPROPERTY(BlueprintReadWrite, Category = "Animated Button", meta = (BindWidget, AllowPrivateAccess = "true"))
	UAnimatedButton* GraphicsButton;

	UPROPERTY(BlueprintReadWrite, Category = "Animated Button", meta = (BindWidget, AllowPrivateAccess = "true"))
	UAnimatedButton* AudioButton;

	UPROPERTY(BlueprintReadWrite, Category = "Animated Button", meta = (BindWidget, AllowPrivateAccess = "true"))
	UAnimatedButton* CameraButton;

	UPROPERTY(BlueprintReadWrite, Category = "Animated Button", meta = (BindWidget, AllowPrivateAccess = "true"))
	UAnimatedButton* AccessibilityButton;

	UPROPERTY(BlueprintReadWrite, Category = "Horizontal Box", meta = (BindWidget, AllowPrivateAccess = "true"))
	UHorizontalBox* MenuButtonsContainer;
	
	//
public:

	UFUNCTION(BlueprintCallable, Category = "Visibility")
	void Show();

	void ShowSubOptions(const int Option) const;
	
protected:
	
	virtual void NativeConstruct() override;
	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	
};
