// 
// AmmoCounter.h
// 
// Ammo counter interface for the main player. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AmmoCounter.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UAmmoCounter : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, Category = Visibility)
	void Show();
	
	UFUNCTION(BlueprintCallable, Category = Visibility)
	void Hide();
	
	void UpdateCurrentAmmo(int NewCurrentAmmo) const;
	void UpdateTotalAmmo(int NewTotalAmmo) const;

	UTextBlock* GetCurrentAmmo() const;
	UTextBlock* GetTotalAmmo() const;

	void SetCurrentAmmo(UTextBlock* NewCurrentAmmo);
	void SetTotalAmmo(UTextBlock* NewTotalAmmo);

private:

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UTextBlock* CurrentAmmo;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UTextBlock* TotalAmmo;
	
};
