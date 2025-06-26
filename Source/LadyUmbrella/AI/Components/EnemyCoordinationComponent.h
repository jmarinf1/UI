// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyCoordinationComponent.generated.h"

class AArenaManager;
class AEnemyCharacter;
class APlayerCharacter;
class AAgencyCharacter;

USTRUCT(BlueprintType)
struct FTokenScore
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TimeWithoutToken = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TokenScore = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Distance = 0;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LADYUMBRELLA_API UEnemyCoordinationComponent : public UActorComponent
{
	GENERATED_BODY()

	const float CALCULATE_TOKENS_NUMBER_OF_PARAMS = 3;


	UPROPERTY()
	AArenaManager* ArenaManager;

	UPROPERTY()
	APlayerCameraManager* CamManager;

	UPROPERTY(EditAnywhere, Category = "CoordinationManager")
	float TokenUpdateTime;

	UPROPERTY(EditAnywhere, Category = "CoordinationManager")
	int NumberOfTokens;

	UPROPERTY(EditAnywhere, Category = "CoordinationManager|Grenade")
	int NumberOfGrenadeTokens;
	
	UPROPERTY(VisibleAnywhere, Category = "CoordinationManager|Grenade")
	float GrenadeTokenPool;
	
	UPROPERTY(EditAnywhere, Category = "CoordinationManager|Grenade")
	float TimeInCoverUntilThrowGrenade;
	
	UPROPERTY(EditAnywhere, Category = "CoordinationManager|Grenade")
	float ThrowGrenadeRate;

	UPROPERTY(EditAnywhere, Category = "CoordinationManager|Grenade")
	float MinThrowGrenadeDistance;

	UPROPERTY(EditAnywhere, Category = "CoordinationManager|Grenade")
	float MaxThrowGrenadeDistance;
	
	UPROPERTY(EditAnywhere, Category = "CoordinationManager|Grenade")
	float PlayerPositionUpdateTime;
	
	UPROPERTY(EditAnywhere, Category = "CoordinationManager|Grenade")
	float MinDistanceToAvoidGrenade;
	
	UPROPERTY(VisibleAnywhere, Category = "CoordinationManager|Grenade")
	FVector LastPlayerPosition;

	UPROPERTY(EditAnywhere, Category = "CoordinationManager|Agency")
	int NumberOfElectricBulletTokens;
	
	UPROPERTY(VisibleAnywhere, Category = "CoordinationManager|Agency")
	float ElectricBulletTokenPool;

	UPROPERTY(VisibleAnywhere, Category = "CoordinationManager")
	float MaxTimeWithoutToken;

	UPROPERTY(EditAnywhere, Category = "CoordinationManager", meta = (ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax = "1"))
	float WeightedDistance;

	UPROPERTY(EditAnywhere, Category = "CoordinationManager", meta = (ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax = "1"))
	float WeightedAngle;

	UPROPERTY(EditAnywhere, Category = "CoordinationManager", meta = (ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax = "1"))
	float WeightedTimeWithoutToken;
	
	UPROPERTY(VisibleAnywhere, Category = "CoordinationManager")
	class UBillboardComponent* BillboardComponent;

	UPROPERTY()
	APlayerCharacter* PlayerRef;

	UPROPERTY()
	FTimerHandle CalculateTokensTimerHandler;
	
	UPROPERTY()
	FTimerHandle CheckPlayerPositionTimerHandler;

	UPROPERTY()
	FTimerHandle CalculateGrenadeTokensTimerHandler;

	UPROPERTY(VisibleAnywhere)
	TMap<AEnemyCharacter*, FTokenScore> TokenScoreMap;
	
public:	
	// Sets default values for this component's properties
	UEnemyCoordinationComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:

	UFUNCTION()
	void Initialize();

	UFUNCTION()
	void StopAndCleanup();

	UFUNCTION()
	void SetPoolReference(const TArray<FEnemyPoolEntry>& EnemyPool);

	UFUNCTION()
	void RemoveEnemy(AGenericCharacter* DeadCharacter);

	UFUNCTION()
	void AddEnemy(FEnemyPoolEntry NewCharacter);
private:
	
	UFUNCTION()
	void CalculateTokenUsers();

	UFUNCTION()
	void CalculateGrenadeTokenUsers();

	UFUNCTION()
	void CheckPlayerPosition();

	UFUNCTION()
	void CheckThrowGrenade(bool bNewState);

	UFUNCTION()
	void AddGrenadeTokenToPool();

	UFUNCTION()
	void ChangeElectricBulletTokenToPool(bool bIsRequesting, AAgencyCharacter* AgencyCharacter);
		
};
