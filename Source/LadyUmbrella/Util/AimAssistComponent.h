#pragma once 
 
#include "CoreMinimal.h" 
#include "Components/ActorComponent.h" 
#include "AimAssistComponent.generated.h" 
 
class AGenericCharacter; 
 
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent)) 
class LADYUMBRELLA_API UAimAssistComponent : public UActorComponent 
{ 
    GENERATED_BODY() 
 
public:  
    UAimAssistComponent(); 
 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Assist|Snapping") 
    float CapsuleRadius; 
 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Assist|Snapping") 
    float CapsuleLength; 
     
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Assist|Snapping") 
    float AimAssistSpeed; 
 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Assist|Snapping") 
    float AimAssistSpeedMultiplier; 
     
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Assist|Snapping") 
    float AimAngleThreshold; 
 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Assist|Snapping") 
    FVector TargetOffset; 
 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Assist|Soft Lock") 
    float SoftLockAngle; 
 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Assist|Soft Lock") 
    float SoftLockSlowMultiplier; 
 
    UPROPERTY(BlueprintReadOnly, Category = "Aim Assist|Soft Lock") 
    bool bShouldSlowAiming; 
 
    UPROPERTY(BlueprintReadOnly, Category = "Aim Assist|Soft Lock") 
    bool bIsAiming; 
 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Assist|Soft Lock") 
    float SoftLockRadius; 
 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Assist|Soft Lock") 
    float SoftLockLength; 
     
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Assist|Debug") 
    bool bShowDebugSnap;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Assist|Debug") 
    bool bShowDebugSoftLock;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Assist|Debug") 
    bool bShowDebugVisibility;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Assist|Debug") 
    bool bDebugMessages;
protected: 
    virtual void BeginPlay() override; 
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override; 
 
public:  
    UFUNCTION(BlueprintCallable) 
    void OnStartAiming(); 
    UFUNCTION(BlueprintCallable) 
    void StopRotation(); 
    UFUNCTION(BlueprintCallable) 
    void CheckForSoftLock(); 
    void SetCameraMultiplier(APlayerController* PC, float NewMult); 
 
private: 
    TArray<FHitResult> PerformSphereTrace(float TraceRadius, float TraceLength, bool ShowDebug); 
    TArray<AGenericCharacter*> FilterUniqueEnemies(const TArray<FHitResult>& HitResults); 
    AGenericCharacter* FindBestTarget(const TArray<AGenericCharacter*>& Enemies, const FVector& ViewLocation, const FVector& ViewDirection); 
    void RotateViewToTarget(APlayerController* PC, const FVector& TargetLocation, float DeltaTime); 
     
private: 
    UPROPERTY() 
    AGenericCharacter* CurrentTarget = nullptr;
    UPROPERTY()
    float CameraSnapStartOffset = 350.0f;
}; 