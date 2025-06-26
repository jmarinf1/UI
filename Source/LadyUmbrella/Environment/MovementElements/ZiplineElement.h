#pragma once

#include "CoreMinimal.h"
#include "LadyUmbrella/Items/InteractuableObjects/GenericInteractable.h"
#include "ZiplineElement.generated.h"

class UInputAction;
class USplineMeshComponent;
class UStaticMeshComponent;
class USceneComponent;
class USplineComponent;
class UBoxComponent;
class APlayerCharacter;
class UWidgetComponent;
class UInteractiveIcon;
class UParticleSystemComponent;



UCLASS()
class LADYUMBRELLA_API AZiplineElement : public AGenericInteractable
{
	GENERATED_BODY()

	const float SplineMaxTime = 1.0f; 
public:
	AZiplineElement();

protected:
	virtual void BeginPlay() override;
	
public:
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;
	
	// Components
    
    /** Anchor point representing the zipline's start. Automatically updated based on Cube1. */
    UPROPERTY(VisibleAnywhere)
    USceneComponent* StartPoint;
    
    /** Anchor point representing the zipline's end. Automatically updated based on Cube2. */
    UPROPERTY(VisibleAnywhere)
    USceneComponent* EndPoint;
    
    /** Offset from Cube1 to determine the actual start of the zipline. */
    UPROPERTY(EditAnywhere, Category = "ZipLine Settings")
    FVector StartOffset;
    
    /** Offset from Cube2 to determine the actual end of the zipline. */
    UPROPERTY(EditAnywhere, Category = "ZipLine Settings")
    FVector EndOffset;
    
    /** Root component used for transformation and rotation of the entire zipline actor. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USceneComponent* DefaultRoot;
    
    /** Mesh component for the starting cube (Cube1). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* Cube1;
    
    /** Mesh component for the ending cube (Cube2). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* Cube2;
    
    /** Spline component used to define the zipline path. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USplineComponent* ZipLineSpline;
    
    /** Spline mesh visualizing the cable between Cube1 and Cube2. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USplineMeshComponent* ZiplineMesh;
    
    /** Collision box used to detect overlap near Cube1 for interaction. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UBoxComponent* Cube1CollisionBox;
    
    /** Particle effect that plays while the player rides the zipline. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UParticleSystemComponent* ZiplineEffect;
    
    // Settings
    
    /** Position of the start cube (Cube1) relative to the actor. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ZipLine Settings", meta = (ToolTip = "Relative position of Cube1 (zipline start). Move to set the beginning of the zipline."))
    FVector Cube1Position;
    
    /** Position of the end cube (Cube2) relative to the actor. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ZipLine Settings", meta = (MakeEditWidget = true, ToolTip = "Relative position of Cube2 (zipline end). Move to set the end point."))
    FVector Cube2Position;
    
    /** Scale of the Cube1 mesh. Controls visual size of the start anchor. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ZipLine Settings", meta = (ToolTip = "Scale of the start cube (Cube1). Adjust for visual size."))
    FVector Cube1Scale;
    
    /** Offset applied to the Cube1 collision box. Used to tweak overlap area. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ZipLine Settings", meta = (MakeEditWidget = true, ToolTip = "Offset for the collision box on Cube1. Use to adjust interaction area."))
    FVector CollisionOffset;
    
    /** Scale of the Cube2 mesh. Controls visual size of the end anchor. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ZipLine Settings", meta = (ToolTip = "Scale of the end cube (Cube2). Adjust for visual size."))
    FVector Cube2Scale;
    
    /** Local rotation of Cube1 (start anchor). Adjust orientation of the mesh. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ZipLine Settings", meta = (ToolTip = "Local rotation of Cube1 (start). For fine-tuning orientation."))
    FRotator Cube1Rotation;
    
    /** Local rotation of Cube2 (end anchor). Adjust orientation of the mesh. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ZipLine Settings", meta = (ToolTip = "Local rotation of Cube2 (end). For fine-tuning orientation."))
    FRotator Cube2Rotation;
    
    /** Speed at which the player travels along the zipline. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ZipLine Settings", meta = (ToolTip = "Movement speed of the player while on the zipline."))
    float ZipLineSpeed;
    
    /** Vertical offset applied to the cable mesh. Purely visual. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ZipLine Settings", meta = (ToolTip = "Height offset applied to the cable mesh for visual adjustment."))
    float CableHeightOffset;
    
    /** Size of the interaction box near Cube1. Determines interaction range. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ZipLine Settings", meta = (ToolTip = "Extent of the overlap box for Cube1. Controls how close the player must be to interact."))
    FVector Box1Extent;
    
    /** Controls the visual thickness (X & Y) of the zipline cable mesh. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ZipLine Settings", meta = (ToolTip = "XY scale of the cable mesh. Changes visual thickness of the zipline."))
    FVector2D V_MeshScale;
    
    /** Vertical offset for the interaction widget above the cable. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ZipLine Settings", meta = (ToolTip = "How high the interaction widget floats above the cable."))
    float UIHeightOffset;
    
    /** Offset applied to spline time during snapping, based on camera direction. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ZipLine Settings", meta = (ClampMin = "0.0", ClampMax = "0.1", ToolTip = "Offset for snapping along the spline, influenced by camera direction."))
    float SplineTimeOffset;
    
    /** Speed at which the player interpolates (snaps) toward the cable before mounting. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ZipLine Settings", meta = (ClampMin = "2.0", ClampMax = "10.0", ToolTip = "Snapping interpolation speed when player approaches the cable."))
    float SnappingInterpSpeed;
    
    /** Minimum distance before the snap-to-cable movement completes. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ZipLine Settings", meta = (ClampMin = "0.1", ClampMax = "1.0", ToolTip = "Distance threshold at which snapping finishes and the player attaches to the zipline."))
    float SnapMinDistanceThreshold;
    
    /** Lower clamp for dot product comparison used during interaction camera checks. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ZipLine Settings", meta = (ToolTip = "Minimum camera dot product value for allowing interaction. Affects angle tolerance."))
    float CameraLookClampMin;
    
    /** Upper clamp for dot product comparison used during interaction camera checks. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ZipLine Settings", meta = (ToolTip = "Maximum camera dot product value for allowing interaction. Affects angle tolerance."))
    float CameraLookClampMax;
    
    /** Maximum spline time (0.0 - 1.0) allowed when starting zipline from mid-spline overlap. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ZipLine Settings", meta = (ToolTip = "Maximum spline time value to allow starting zipline ride from a mid-point overlap. Usually 0.9–1.0."))
    float MaxStartTime;
    
    // Input
    
    /** Input action that initiates the zipline ride (usually Jump or Interact). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Actions", meta = (ToolTip = "Input action bound to starting the zipline (e.g., Jump or Interact)."))
    UInputAction* JumpAction;
    
    // Runtime State
    
    /** Current time (0.0–1.0) along the spline used for zipline movement. */
    UPROPERTY()
    float CurrentSplineTime;
    
    /** True if the player is in interaction range of the zipline. */
    UPROPERTY()
    bool IsInRange;
    
    /** True if the player is currently snapping to the zipline before riding. */
    UPROPERTY()
    bool IsSnapping;
    
    /** World location the player is snapping to before starting the ride. */
    UPROPERTY()
    FVector SnapLocation;
    
    /** Timer used for moving the character along the spline while riding. */
    UPROPERTY()
    FTimerHandle MovementTimerHandle;

	// Overlap handlers
	UFUNCTION()
	void OnCube1Overlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                    UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep,
	                    const FHitResult& SweepResult);

	UFUNCTION()
	void OffCube1Overlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnSplineOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                          UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex,
	                          bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void ProcessOverlap(AActor* OtherActor);

	void Jump();
	void MoveCharacterAlongSpline();
	
	void AdjustSnappingPoint();
};
