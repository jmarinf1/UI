#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractiveMovementComponent.generated.h"

class UCapsuleComponent;
class USkeletalMeshComponent;
class UCharacterMovementComponent;
class UAnimMontage;
class UMotionWarpingComponent;
class UKickInterface;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LADYUMBRELLA_API UInteractiveMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInteractiveMovementComponent();

protected:
	virtual void BeginPlay() override;
	
private:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", ToolTip = "Owner actor of this component."))
	AActor* Owner;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", ToolTip = "Capsule component of the character."))
	UCapsuleComponent* Capsule;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", ToolTip = "Skeletal mesh of the character."))
	USkeletalMeshComponent* SkeletalMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", ToolTip = "Character movement component."))
	UCharacterMovementComponent* CharacterMovement;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", ToolTip = "Motion warping component used for custom animation root motion."))
	UMotionWarpingComponent* MotionWarping;

	// ANIMATION
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive Movement", meta = (AllowPrivateAccess = "true", ToolTip = "End position of the mantle movement."))
	UAnimInstance* AnimInstance;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Mantle", meta = (AllowPrivateAccess = "true", ToolTip = "Animation montage used for mantling."))
	UAnimMontage* MantleMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Vault", meta = (AllowPrivateAccess = "true", ToolTip = "Animation montage used for long vaulting."))
	UAnimMontage* VaultMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Vault", meta = (AllowPrivateAccess = "true", ToolTip = "Animation montage used for short vaulting."))
	UAnimMontage* ShortVaultMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Step", meta = (AllowPrivateAccess = "true", ToolTip = "Animation montage used for stepping up."))
	UAnimMontage* StepMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement", meta = (AllowPrivateAccess = "true", ToolTip = "Minimum surface normal Z value to allow interaction. (1 = Horizontal; 0 = Vertical)"))
	float SurfaceNormalZValue;

	// MANTLE
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Mantle", meta = (AllowPrivateAccess = "true", ToolTip = "Vertical offset for the mantle sphere trace launched from the player."))
	float MantleSphereTraceOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Mantle", meta = (AllowPrivateAccess = "true", ToolTip = "Maximum distance to stick to wall when mantling."))
	float StickToWallDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Mantle", meta = (AllowPrivateAccess = "true", ToolTip = "Base forward distance to start detecting a potential climbable surface."))
	float MantleDistance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive Movement|Mantle", meta = (AllowPrivateAccess = "true", ToolTip = "End position of the mantle movement."))
	FVector MantleEndPos;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Vault", meta = (AllowPrivateAccess = "true"))
	float MantleAnimPlayRate;

	// VAULT
	// CONSTANTES O CONFIG VARIABLES
	UPROPERTY()
	float SphereTraceRadius;
	
	UPROPERTY()
	FVector SphereTraceVerticalOffset;
	
	UPROPERTY()
	float SphereTraceLength;
	
	UPROPERTY()
	int SphereTraceStep;
	
	UPROPERTY()
	int MinimumLandSpace;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Vault", meta = (AllowPrivateAccess = "true", ToolTip = "Maximum distance to check if vault is possible (Length of the trace launched from the player )."))
	float CheckVaultDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Vault", meta = (AllowPrivateAccess = "true", ToolTip = "Minimum length of vaultable surface."))
	float MinVaultableLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Vault", meta = (AllowPrivateAccess = "true", ToolTip = "Minimum length of vaultable surface."))
	float VaultOverHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Vault", meta = (AllowPrivateAccess = "true", ToolTip = "Playback rate for the vault animation."))
	float VaultAnimPlayRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Vault", meta = (AllowPrivateAccess = "true", ToolTip = "Playback rate for the short vault animation."))
	float ShortVaultAnimPlayRate;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Vault", meta = (AllowPrivateAccess = "true", ToolTip = "Playback rate for the short vault animation."))
	float LongVaultAnimPlayRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Vault", meta = (AllowPrivateAccess = "true", ToolTip = "Maximum distance to be able to vault."))
	float MaxVaultDistance;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Vault", meta = (AllowPrivateAccess = "true", ToolTip = "Maximum distance to be able to vault."))
	float LongVaultDistance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive Movement|Vault", meta = (AllowPrivateAccess = "true", ToolTip = "True if current vault is a short vault."))
	bool bShortVault;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive Movement|Vault", meta = (AllowPrivateAccess = "true", ToolTip = "True if current vault is a short vault."))
	bool bLongVault;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interactive Movement|Vault", meta = (AllowPrivateAccess = "true", ToolTip = "Maximum distance to be considered a short vault."))
	float ShortVaultDistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interactive Movement|Vault", meta = (AllowPrivateAccess = "true", ToolTip = "Maximum distance to be considered a short vault."))
	float VaultLandTraceLenght;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive Movement|Vault", meta = (AllowPrivateAccess = "true", ToolTip = "Start position of the vault movement."))
	FVector VaultStartPos;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive Movement|Vault", meta = (AllowPrivateAccess = "true", ToolTip = "Middle position of the vault movement."))
	FVector VaultMiddlePos;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive Movement|Vault", meta = (AllowPrivateAccess = "true", ToolTip = "Landing position after vaulting."))
	FVector VaultLandPos;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive Movement|Vault", meta = (AllowPrivateAccess = "true", ToolTip = "Landing position after vaulting."))
	bool bIsVoidVault;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive Movement|Vault", meta = (AllowPrivateAccess = "true", ToolTip = "Whether motion warping can be applied to vault."))
	bool bCanWarp = false;


	// Kick
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive Movement|Vault|Kick", meta = (AllowPrivateAccess = "true", ToolTip = "Result of the hit when kicking during vault."))
	FHitResult KickHit;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive Movement|Vault|Kick", meta = (AllowPrivateAccess = "true", ToolTip = "Actor hit and kicked during vault."))
	AActor* KickedActor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interactive Movement|Vault|Kick", meta = (AllowPrivateAccess = "true", ToolTip = "Half-size of the box trace used to detect kickable enemies during vaulting."))
	FVector KickedBoxTraceExtent;
	

	// STEP
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive Movement|Step", meta = (AllowPrivateAccess = "true", ToolTip = "Landing position of the step."))
	FVector StepLandPos;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive Movement|Step", meta = (AllowPrivateAccess = "true", ToolTip = "Starting position of the step."))
	FVector StepStartPos;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Step", meta = (AllowPrivateAccess = "true", ToolTip = "Playback rate of the step animation."))
	float StepAnimPlayRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Step", meta = (AllowPrivateAccess = "true", ToolTip = "Maximum distance to check for stepping possibility."))
	int StepCheckDistance;

	// SWING
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Swing", meta = (AllowPrivateAccess = "true", ToolTip = "Maximum distance to check for swing possibility."))
	float CheckSwingDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Swing", meta = (AllowPrivateAccess = "true", ToolTip = "Vertical offset applied to swing trace origin."))
	float SwingTraceVerticalOffset;

public:
	UFUNCTION(BlueprintCallable, Category = "Interactive Movement")
	void CheckMantle(TArray<AActor*> IgnoredActors);

	UFUNCTION(BlueprintCallable, Category = "Interactive Movement")
	void Mantle();

	UFUNCTION()
	bool CheckSwing(TArray<AActor*> IgnoredActors) const;

	UFUNCTION()
	bool CheckVault(TArray<AActor*> IgnoredActors);

	UFUNCTION()
	bool SetUpVault(const FVector& StartVaultLocation, TArray<AActor*> IgnoredActors);
	
	UFUNCTION()
	bool VaultMotionWarp();

	UFUNCTION()
	bool StepMotionWarp();

	UFUNCTION()
	void OnMotionWarpStarted(FName NotifyName);

	UFUNCTION()
	void OnMotionWarpFinished(FName NotifyName);

private:
	
	UFUNCTION()
	void PrepareIgnoredActors(TArray<AActor*>& IgnoredActors) const;
	
	UFUNCTION()
	bool HandleVaultTrace(const FVector& StartVaultLocation, const TArray<AActor*>& IgnoredActors, int iIndex, int& LastIndex, bool& bCanLand);
	
	UFUNCTION()
	bool ProcessSphereTraceHit(const FVector& StartVaultLocation, const FHitResult& OutHit, int iIndex, int& LastIndex, bool& bCanLand);
	
	UFUNCTION()
	bool ProcessVaultLanding(const FVector& SphereTraceStart, const FHitResult& OutHit, const TArray<AActor*>& IgnoredActors, bool& bCanLand);
	
	UFUNCTION()
	void DetectKickableActor(const FHitResult& OutHit, const FVector& SphereTraceStart, const TArray<AActor*>& IgnoredActors);
	
	UFUNCTION()
	FVector GetLineTraceStart() const;
	
	UFUNCTION()
	bool CheckLandingLocation(const FVector& SphereTraceStart, const TArray<AActor*>& IgnoredActors, bool& bCanLand);

	
	UFUNCTION()
	bool FinalizeVault(int LastIndex, const FVector& StartVaultLocation, bool bCanLand);
	
	UFUNCTION()
	void OnMantleCompleted(UAnimMontage* Montage, bool bInterrupted);
	
	UFUNCTION()
	void OnVaultCompleted(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnShortVaultCompleted(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnStepCompleted(UAnimMontage* Montage, bool bInterrupted);
};
