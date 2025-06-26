#pragma once

#include "CoreMinimal.h"
#include "LadyUmbrella/Environment/ReactiveElements/ReactiveElement.h"
#include "ElectricElement.generated.h"

UCLASS(Abstract)
class LADYUMBRELLA_API AElectricElement : public AReactiveElement
{
	GENERATED_BODY()

public:
	AElectricElement();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Electric State")
	bool bElectrified;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Electric State")
	float FElecTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Electric State")
	UParticleSystem* SparksEffect;

	UPROPERTY()
	UParticleSystemComponent* SparksComponent;

	UPROPERTY()
	FTimerHandle ElectrifiedTimerHandle;

	UFUNCTION()
	virtual void ResetElectrifiedState();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Electric State")
	TArray<AElectricElement*> ConnectedElectricElements;
public:
	UFUNCTION(BlueprintCallable, Category = "Electric State")
	virtual void SetElectrified(bool bNewElectrified);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Electric State")
    void OnElectrified();
};
