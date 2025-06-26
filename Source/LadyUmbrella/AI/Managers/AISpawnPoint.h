// 
// AISpawnPoint.h
// 
// Actor that represents an SpawnPoint managed by ArenaManager
// Contains Enemy Arquetype and Spline Path Data
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AISpawnPoint.generated.h"

//fordward declarations
class ASplinePath;
class AEnemyCharacter;
class AArenaManager;
class UArrowComponent;
class AGenericWeapon;
class AReactiveElement;
class UBillboardComponent;

//targets
UENUM(BlueprintType)
enum class ETarget : uint8
{
	Enemy,
	Interactable
};

UCLASS()
class LADYUMBRELLA_API AAISpawnPoint : public AActor
{
	GENERATED_BODY()
private:
#pragma region COMPONENTS
	UPROPERTY(VisibleAnywhere, Category="EditorOnly")
	USceneComponent* Root;
	
	UPROPERTY(VisibleAnywhere, Category="EditorOnly")
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(VisibleAnywhere, Category="EditorOnly")
	UBillboardComponent* Billboard;

	// Billboard targets
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	UTexture2D* EnemyIcon;

	UPROPERTY(EditDefaultsOnly, Category = "Config")
	UTexture2D* InteractableIcon;
#if WITH_EDITORONLY_DATA

	UPROPERTY(VisibleAnywhere, Category="EditorOnly")
	UArrowComponent* Arrow;
#endif
#pragma endregion COMPONENTS
	
public:
#pragma region CONFIGURATION_DATA
	//Spawn Data
	UPROPERTY(EditAnywhere, Category="Config",meta = (BlueprintBaseOnly, DisplayPriority = 0))
	ETarget Target;
	
	UPROPERTY(EditInstanceOnly, Category="Config", meta = (BlueprintBaseOnly, DisplayPriority = 0,EditCondition = "Target == ETarget::Enemy",EditConditionHides))
	TSubclassOf<AEnemyCharacter> EnemyType;

	UPROPERTY(EditInstanceOnly, Category="Config", meta = (BlueprintBaseOnly, DisplayPriority = 0,EditCondition = "Target == ETarget::Interactable",EditConditionHides))
	TSubclassOf<AReactiveElement> ReactiveElementType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config", meta = (DisplayPriority = 0,AllowPrivateAccess = true,EditCondition = "Target == ETarget::Enemy",EditConditionHides))
	TSubclassOf<AGenericWeapon> WeaponBP;

	UPROPERTY(EditDefaultsOnly,Category="Config",meta=(DisplayPriority = 1,EditCondition = "Target == ETarget::Enemy",EditConditionHides))
	TSubclassOf<ASplinePath> SplineClass;

	UPROPERTY(EditInstanceOnly,Category="Config")
	bool bHasPath;
	
	UPROPERTY(VisibleAnywhere, Category="Config", meta=(DisplayPriority = 1,EditCondition = "Target == ETarget::Enemy",EditConditionHides))
	ASplinePath* Path;

	UPROPERTY(VisibleAnywhere,Category="Config")
	AArenaManager* OwningArenaManager;
	
	UPROPERTY(VisibleAnywhere,Category="Config",meta=(EditCondition = "Target == ETarget::Enemy",EditConditionHides))
	int NumZone = 0;

	UPROPERTY(EditInstanceOnly,Category="Config",meta=(EditCondition = "Target == ETarget::Enemy",EditConditionHides))
	int EnterArenaGroup;
	
	UPROPERTY(EditInstanceOnly,Category="Config",meta=(EditCondition = "Target == ETarget::Enemy",EditConditionHides))
	bool bIsReinforcement;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	int32 SpawnOrder;

	UPROPERTY(EditInstanceOnly,Category="Config")
	bool bIsActive = true;
#pragma endregion CONFIGURATION_DATA
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void OnConstruction(const FTransform& Transform) override;
#endif
	
public:
	// Sets default values for this actor's properties
	AAISpawnPoint();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
	void SetZone(int newzone);

	UFUNCTION()
	int GetZone() const;

	void SetArenaReference(AArenaManager* AttachedArena);
	
	UStaticMeshComponent* GetStaticMesh() const { return StaticMesh;} 

	AArenaManager* GetArenaManager() const { return OwningArenaManager;}
private:
	virtual void Destroyed() override;

	UFUNCTION()
	void RegisterInArenaManager();
};
