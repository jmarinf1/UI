// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LadyUmbrella/UI/Menu.h"
#include "ControlSettings.generated.h"

class UAnimatedButton;
class UTextBlock;

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UControlSettings : public UMenu
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Animated Button", meta = (BindWidget, AllowPrivateAccess = "true"))
	UAnimatedButton* ControlType;

	UPROPERTY(BlueprintReadWrite, Category = "Animated Button", meta = (BindWidget, AllowPrivateAccess = "true"))
	UTextBlock* PlatformName;

	void UpdateSettingsOptions(const bool bMoveForward, const bool bMoveBackward) const;
	void ExitSettingsMenu();

public:

	void Show();
	
	UFUNCTION()
	void UpdatePlatformName();

protected:
	
	virtual void NativeConstruct() override;
	virtual FReply NativeOnAnalogValueChanged(const FGeometry& InGeometry, const FAnalogInputEvent& InAnalogEvent) override;
	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	
};