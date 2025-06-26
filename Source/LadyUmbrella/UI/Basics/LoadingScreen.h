// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NumericProgressBar.h"
#include "LoadingScreen.generated.h"

class UImage;
class UTextBlock;

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API ULoadingScreen : public UNumericProgressBar
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Image", meta = (BindWidget, AllowPrivateAccess = "true"))
	UImage* LoadingImage;

	UPROPERTY(BlueprintReadWrite, Category = "Percentage", meta = (BindWidget, AllowPrivateAccess = "true"))
	UTextBlock* TextPercentage;

	public:
	
	virtual void UpdateProgressBar(const float Value) override;

	UTextBlock* GetPercentageText() const;
	void SetPercentageText(UTextBlock* NewText);
};
