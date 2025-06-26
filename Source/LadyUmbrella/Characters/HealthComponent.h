// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthAmmountChanged, int32, NewHealth, bool, bLostHealth);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LADYUMBRELLA_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

#pragma region PARAMETERS
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	int32 MaxHealth;
	
	/**
 	* Whether this Component can regenerate Health
 	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	bool bCanRegenerate;

	/**
 	* Interval between calls to the RegenerateHealth Function
 	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float RegenerationSpeed;
	
	/**
	* Time for the first call to RegenerateHealth function
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float RegenerationCooldown;
	
	/**
	* Value to add each tick to Health. Left to 1 for visual cool effect
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	int RegenerationValue; 

#pragma endregion

#pragma region VARIABLES
protected:
	UPROPERTY(VisibleInstanceOnly)
	int32 CurrentHealth;

	UPROPERTY()
	FTimerHandle RegenTimerHandle;

	UPROPERTY()
	FTimerHandle RegenDelayHandle;

#pragma endregion 

#pragma region DELEGATES
public:
	/**
	 * Adds a delegate for On Death events
	*/
	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnDeathDelegate OnDeathDelegate;

	UPROPERTY(BlueprintAssignable, Category= "Health")
	FOnHealthAmmountChanged OnHealthAmmountChanged;

#pragma endregion 
	
#pragma region FUNCTIONS
	
#pragma region BASIC FUNCTIONS
public:
	UHealthComponent();
protected:
	virtual void BeginPlay() override;

#pragma endregion 
	
#pragma region PRIVATE FUNCTIONS
private:
	/**
	 * Function called to regenerate health depending on time
	 */
	UFUNCTION()
	void RegenerateHealth();

	UFUNCTION()
	void StartRegenHealth();

	UFUNCTION()
	void StopRegenHealth();

	UFUNCTION()
	void OnDeath();

#pragma endregion

#pragma region PUBLIC FUNCTIONS
public:
	/**
	 * Sets the health to the param
	 * @param NewHealth The new Health Value
	*/
	UFUNCTION(BlueprintCallable)
	void SetHealth(int NewHealth);

	/**
	* Subtracts the health to CurrentHealth
	* @param HealthToSubtract The ammount of health to be subtracted
	* @return Returns if the player still alive or not	 
	*/
	UFUNCTION(BlueprintCallable)
	void SubtractHealth(int HealthToSubtract);
	
	/**
	* Adds health to the current health
	* @param HealthToAdd The ammount of health to be Added
	*/
	UFUNCTION(BlueprintCallable)
	void AddHealth(int HealthToAdd);

#pragma endregion

#pragma region GETTERS AND SETTERS
public:
	UFUNCTION()
	FORCEINLINE int32 GetHealth() const { return CurrentHealth; };
	UFUNCTION()
	FORCEINLINE int32 GetMaxHealth() const { return MaxHealth; };
	UFUNCTION()
	FORCEINLINE bool CanRegenerateHealth() const { return bCanRegenerate; };
	UFUNCTION()
	FORCEINLINE bool IsDead() const { return CurrentHealth <= 0;};
	
	UFUNCTION()
	void SetMaxHealth(const int NewMaxHealth);

	UFUNCTION()
	FORCEINLINE void SetCanRegenerate(const bool NewRegenerate) { bCanRegenerate = NewRegenerate; };

#pragma endregion 

#pragma endregion 
	
};
