// 
// TriggerBoxArena.h
// 
// TriggerBox that notify to the Arena linked if need to be Activated, StartCombat or SelectZones for Covers
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "TriggerBoxArena.generated.h"

//forward declarations
class AArenaManager;

UENUM()
enum class EPurpose : uint8
{
 StartCombatArena,
 ActivateArena,
 SelectZones,
 EnterArenaEvent
};
/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API ATriggerBoxArena : public ATriggerBox
{
 GENERATED_BODY()
#pragma region Configuration
	//purpose of current trigger arena
	UPROPERTY(EditInstanceOnly,Category= "Configuration")
	EPurpose Efunc;
	//arena reference
	UPROPERTY(VisibleAnywhere, Category= "Configuration", meta = (EditCondition = "Efunc == EPurpose::ActivateArena || Efunc == EPurpose::StartCombatArena || Efunc == EPurpose::SelectZones || Efunc == EPurpose::EnterArenaEvent", EditConditionHides))
	AArenaManager* ArenaManagerToActivate;
	//zone to set
	UPROPERTY(EditAnywhere,Category= "Configuration", meta = (EditCondition = "Efunc == EPurpose::SelectZones", EditConditionHides))
	uint8 NumZone;
	//cover class to find in beginplay
	UPROPERTY(EditAnywhere,Category="Configuration")
	UClass* CoverClass;
	//EnterGroup to active
	UPROPERTY(EditAnywhere,Category= "Configuration", meta = (EditCondition = "efunc == EPurpose::EnterArenaEvent", EditConditionHides))
	uint8 NumGroup;
#pragma endregion
#if WITH_EDITORONLY_DATA
	UPROPERTY()
	TObjectPtr<UTexture2D>  SpriteCombat;
	UPROPERTY()
	TObjectPtr<UTexture2D> SpriteActivate;
	UPROPERTY()
	TObjectPtr<UTexture2D> SpriteZones;
	UPROPERTY()
	TObjectPtr<UTexture2D> SpriteEnter;
#endif
public:
	ATriggerBoxArena();

	void SetArenaReference(AArenaManager* AttachedArena);
	//Active arena - enemies start patroling
	UFUNCTION()
	void ActiveArena(AActor* OverlappedActor, AActor* OtherActor);
	//Start combat - enemies start combat
	UFUNCTION()
	void StartCombatArena(AActor* OverlappedActor, AActor* OtherActor);

#if WITH_EDITORONLY_DATA
	void SetEFunc(const EPurpose& NewPurpose);
	void ResetBillboardSprite();
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
	//Start combat - enemies start combat
	UFUNCTION()
	void EnterArenaEvent(AActor* OverlappedActor, AActor* OtherActor);
private:
	
	virtual void BeginPlay() override;
	//Editor function to set zones to spawns and covers
	UFUNCTION(CallInEditor, Category="Configuration",meta = (EditCondition = "efunc == EPurpose::SelectZones",EditConditionHides))
	void SetZones() const;

	UFUNCTION(CallInEditor, Category="Configuration",meta = (EditCondition = "efunc == EPurpose::SelectZones",EditConditionHides))
	void ClearZones() const;

	
};