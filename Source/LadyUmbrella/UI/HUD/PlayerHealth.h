// 
// PlayerHealth.h
// 
// Health interface for the main player. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "LadyUmbrella/UI/GenericProgressBar.h"
#include "PlayerHealth.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UPlayerHealth : public UGenericProgressBar
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = Visibility)
	void Show();

	UFUNCTION(BlueprintCallable, Category = Visibility)
	void Hide();

	UFUNCTION()
	UTextBlock* GetHealthText() const;

	UFUNCTION()
	void SetHealthText(UTextBlock* NewPlayerHealthText);

	UFUNCTION()
	void UpdatePlayerHealthWithDelegate(int32 NewHealth, bool bLostHealth);
	
private:

	UPROPERTY(BlueprintReadWrite, Category = "ProgressBar", meta = (BindWidget, AllowPrivateAccess = "true"))
	UTextBlock* HealthText;

protected:
	
	virtual void NativeOnInitialized() override;
	virtual void UpdateProgressBar(const float Value) override;
	
};
