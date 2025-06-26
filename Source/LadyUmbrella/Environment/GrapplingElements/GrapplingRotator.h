
#pragma once

#include "CoreMinimal.h"
#include "GrapplingObject.h"
#include "GrapplingRotator.generated.h"

class AHookImpactPoint;
class UBoxComponent;

/**
 * @class AGrapplingRotator
 *
 * @brief A class that handles rotation logic for a grappling system.
 *
 * AGrapplingRotator is designed for use in systems where an object needs to
 * pivot or rotate based on grappling mechanics. It calculates and applies
 * rotation transformations, often used in physics-based grappling or movement
 * systems for interactive environments.
 *
 * This class facilitates smooth and controlled rotations, ensuring realistic
 * and responsive behavior during grappling sequences. The rotation logic
 * typically interacts with components such as skeletal meshes or physics bodies,
 * manipulating their rotation based on external inputs or forces.
 */
UCLASS()
class LADYUMBRELLA_API AGrapplingRotator : public AGrapplingObject
{
	GENERATED_BODY()
	
protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision Box", meta = (DisplayName = "Second Impact Collision Box", AllowPrivateAccess = "true"))
	UBoxComponent* SecondHitPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision Box", meta = (DisplayName = "Physical Collision Box", AllowPrivateAccess = "true"))
	UBoxComponent* PhysicalCollision; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation Curve", meta = (DisplayName = "Acceleration Curve", AllowPrivateAccess = "true"))
	UCurveFloat* RotationCurve;

	float RotationDuration;
	float RotationElapsed;

	FQuat InitialQuat;
	FQuat TargetQuat;
	
	UPROPERTY()
    FTimerHandle RotationTimer;
	
	UFUNCTION(CallInEditor, DisplayName = "Add Child Impact Point", Category = "Settings", meta = (AllowPrivateAccess = "true"))
	void AddNewImpactPoint();
	
	TSubclassOf<AHookImpactPoint> HookImpactPointSubClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rotating params", meta = (AllowPrivateAccess = "true"))
	TArray<AHookImpactPoint*> ImpactPoints;
	
public:

	AGrapplingRotator();
	
	virtual void Destroyed() override;

	UFUNCTION()
	void RotateActorDynamic();
	
	UFUNCTION()
	void RotateFromChildDynamic(const float DegreesOfRotationChild, const float SpeedOfRotationChild);

#pragma region HOOK INTERFACE FUNCTIONS
	virtual void ContactWithCable(FVector& Direction, UPrimitiveComponent* ComponentHit) override{};
	virtual GrabType GetGrabType() override;
#pragma endregion
	
};
