#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LaserElement.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;

UCLASS()
class LADYUMBRELLA_API ALaserElement : public AActor
{
	GENERATED_BODY()

public:	
	ALaserElement();

protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser")
	UNiagaraSystem* LaserNiagaraSystem;

	UPROPERTY(VisibleAnywhere, Category = "Laser")
	UStaticMeshComponent* LaserBaseMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser Settings")
	FVector BoxLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser Settings")
	FVector BoxExtent;

	UPROPERTY(EditAnywhere, Category = "Laser Settings")
	float LaserRange;
private:
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* DetectionBox;

	enum class ELaserState
	{
		Deactivated,
		Activated
	};

	ELaserState CurrentState;

	FTimerHandle DeactivateTimerHandle;

	void ActivateLaser();
	void DeactivateLaser();
	void FireLaser() const;

	UFUNCTION()
	void OnPlayerDetected(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float LaserDamage;

	
};