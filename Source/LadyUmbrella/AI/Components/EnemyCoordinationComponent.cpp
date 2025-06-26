// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCoordinationComponent.h"

#include "Kismet/GameplayStatics.h"
#include "LadyUmbrella/AI/Managers/ArenaManager.h"
#include "LadyUmbrella/Characters/Enemies/EnemyCharacter.h"
#include "LadyUmbrella/Characters/Enemies/Agency/AgencyCharacter.h"
#include "LadyUmbrella/Characters/Enemies/Mafia/MafiaCharacter.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Util/Logger.h"


// Sets default values for this component's properties
UEnemyCoordinationComponent::UEnemyCoordinationComponent()
	: TokenUpdateTime(2.f)
	, NumberOfTokens(2)
	, NumberOfGrenadeTokens(1)
	, TimeInCoverUntilThrowGrenade(2.f)
	, ThrowGrenadeRate(5.f)
	, MinThrowGrenadeDistance(1500)
	, MaxThrowGrenadeDistance(3000)
	, PlayerPositionUpdateTime(5.f)
	, MinDistanceToAvoidGrenade(200)
	, LastPlayerPosition(FVector::ZeroVector)
	, NumberOfElectricBulletTokens(1)
	, MaxTimeWithoutToken(0)
	, WeightedDistance(0.8f)
	, WeightedAngle(0.9f)
	, WeightedTimeWithoutToken(0.01)
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UEnemyCoordinationComponent::BeginPlay()
{
	Super::BeginPlay();
	
	CamManager = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
}

void UEnemyCoordinationComponent::Initialize()
{
	if (!GetOwner())
	{
		FLogger::ErrorLog("[UEnemyCoordinationComponent::Initialize]: OWNER NULL");
	}
	
	ArenaManager = Cast<AArenaManager>(GetOwner());
	if (!IsValid(ArenaManager))
	{
		FLogger::ErrorLog("[UEnemyCoordinationComponent::Initialize]: ARENA MANAGER NULL");
		return;
	}
	
	if (!GetWorld())
	{
		FLogger::ErrorLog("[UEnemyCoordinationComponent::Initialize]: WORLD NULL");
	}
	
	PlayerRef = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (!IsValid(PlayerRef))
	{
		FLogger::ErrorLog("[UEnemyCoordinationComponent::Initialize]: PlayerRef NULL");
		return;
	}
	
	if (IsValid(PlayerRef->GetCoverMovementComponent()) && !ArenaManager->GetIsAgencyArena())
	{
		PlayerRef->GetCoverMovementComponent()->OnIsCoveredChanged.AddDynamic(this, &UEnemyCoordinationComponent::CheckThrowGrenade);
	
		GrenadeTokenPool = NumberOfGrenadeTokens;

		AMafiaCharacter::OnGrenadeTokenReleased.AddDynamic(this, &UEnemyCoordinationComponent::AddGrenadeTokenToPool);
		GetWorld()->GetTimerManager().SetTimer(CheckPlayerPositionTimerHandler, this, &UEnemyCoordinationComponent::CheckPlayerPosition, PlayerPositionUpdateTime, true, 0);
	}

	if (ArenaManager->GetIsAgencyArena())
	{
		ElectricBulletTokenPool = NumberOfElectricBulletTokens;
		AAgencyCharacter::OnElectricBulletTokenRequest.AddDynamic(this, &UEnemyCoordinationComponent::ChangeElectricBulletTokenToPool);
	}

	GetWorld()->GetTimerManager().SetTimer(CalculateTokensTimerHandler, this, &UEnemyCoordinationComponent::CalculateTokenUsers, TokenUpdateTime, true, 0);
}

void UEnemyCoordinationComponent::StopAndCleanup()
{
	TokenScoreMap.Empty();
	//EnemyPool.Empty();
	MaxTimeWithoutToken = 0;
	LastPlayerPosition = FVector::ZeroVector;

	//Clear Timers
	if (CalculateTokensTimerHandler.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(CalculateTokensTimerHandler);
	}
	if (CheckPlayerPositionTimerHandler.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(CheckPlayerPositionTimerHandler);
	}
	
	if (CalculateGrenadeTokensTimerHandler.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(CalculateGrenadeTokensTimerHandler);
	}
	if (IsValid(PlayerRef))
	{
		if (IsValid(PlayerRef->GetCoverMovementComponent()))
		{
			PlayerRef->GetCoverMovementComponent()->OnIsCoveredChanged.RemoveDynamic(this, &UEnemyCoordinationComponent::CheckThrowGrenade);
		}
	}

	AMafiaCharacter::OnGrenadeTokenReleased.RemoveDynamic(this, &UEnemyCoordinationComponent::AddGrenadeTokenToPool);
	AAgencyCharacter::OnElectricBulletTokenRequest.RemoveDynamic(this, &UEnemyCoordinationComponent::ChangeElectricBulletTokenToPool);
	PlayerRef->GetCoverMovementComponent()->OnIsCoveredChanged.RemoveDynamic(this, &UEnemyCoordinationComponent::CheckThrowGrenade);
}

void UEnemyCoordinationComponent::SetPoolReference(const TArray<FEnemyPoolEntry>& EnemyPool)
{
	for (FEnemyPoolEntry Enemy : EnemyPool)
	{
		FTokenScore AuxScore;
		TokenScoreMap.Add(Enemy.Enemy, AuxScore);
	}
}

void UEnemyCoordinationComponent::RemoveEnemy(AGenericCharacter* DeadCharacter)
{
	if (AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(DeadCharacter))
	{
		TokenScoreMap.Remove(Enemy);
	}
}

void UEnemyCoordinationComponent::AddEnemy(FEnemyPoolEntry Enemy)
{
	FTokenScore AuxScore;
	TokenScoreMap.FindOrAdd(Enemy.Enemy, AuxScore);
}

void UEnemyCoordinationComponent::CalculateTokenUsers()
{
	if (!IsValid(PlayerRef))
	{
		FLogger::ErrorLog("[UEnemyCoordinationComponent::CalculateTokenUsers]: PlayerRef NULL");
		return;
	}
	float MaxDistance = -1000000000;
	TArray<float> DistanceArray;

	int i = 0;
	for (auto It = TokenScoreMap.CreateConstIterator(); It; ++It)
	{
		DistanceArray.Add(FVector::Dist(PlayerRef->GetActorLocation(),It.Key()->GetActorLocation()));
		if (MaxDistance < DistanceArray[i])
		{
			MaxDistance = DistanceArray[i];
		}
		i++;
	}

	i = 0;
	for (auto It = TokenScoreMap.CreateConstIterator(); It; ++It)
	{
		// Distance
		TokenScoreMap[It.Key()].TokenScore = (1 - (DistanceArray[i] / MaxDistance)) * WeightedDistance;
		TokenScoreMap[It.Key()].Distance = DistanceArray[i];
		
		// Angle From Player
		TokenScoreMap[It.Key()].TokenScore += (1 - (FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(CamManager->GetActorForwardVector().GetSafeNormal(), (It.Key()->GetActorLocation() - PlayerRef->GetActorLocation()).GetSafeNormal()))) / 180)) * WeightedAngle;
	
		// Time Without Token
		if (MaxTimeWithoutToken != 0)
		{
			TokenScoreMap[It.Key()].TokenScore += (TokenScoreMap[It.Key()].TimeWithoutToken / MaxTimeWithoutToken) * WeightedTimeWithoutToken;
		}

		TokenScoreMap[It.Key()].TokenScore /= CALCULATE_TOKENS_NUMBER_OF_PARAMS;
		i++;
	}

	TokenScoreMap.ValueStableSort([](FTokenScore A, FTokenScore B) {
		return A.TokenScore > B.TokenScore; // sort keys in reverse
		});


	i = 0;
	for (auto It = TokenScoreMap.CreateConstIterator(); It; ++It)
	{
		if (i < NumberOfTokens)
		{
			It.Key()->SetHasToken(true);
			TokenScoreMap[It.Key()].TimeWithoutToken = 0;
		}
		else
		{
			It.Key()->SetHasToken(false);
			TokenScoreMap[It.Key()].TimeWithoutToken += TokenUpdateTime;

			if (TokenScoreMap[It.Key()].TimeWithoutToken > MaxTimeWithoutToken)
			{
				MaxTimeWithoutToken = TokenScoreMap[It.Key()].TimeWithoutToken;
			}
		}
		i++;
	}
}

void UEnemyCoordinationComponent::CalculateGrenadeTokenUsers()
{
	int32 TokenScoreSize = TokenScoreMap.Num();
	if (!NumberOfGrenadeTokens ||  TokenScoreSize == 0)
	{
		return;
	}
	
	AMafiaCharacter* GrenadeMafiaCharacter;
	for (auto It = TokenScoreMap.CreateConstIterator(); It; ++It)
	{
		if (It.Value().Distance < MinThrowGrenadeDistance || It.Value().Distance > MaxThrowGrenadeDistance)
		{
			continue;
		}
		if (GrenadeTokenPool <= 0)
		{
			break;
		}
		
		GrenadeMafiaCharacter = Cast<AMafiaCharacter>(It.Key());

		if (IsValid(GrenadeMafiaCharacter))
		{
			GrenadeMafiaCharacter->SetHasGrenadeToken(true);
			GrenadeTokenPool--;
		}
	}
}

void UEnemyCoordinationComponent::CheckPlayerPosition()
{
	if (!IsValid(PlayerRef))
	{
		return;
	}

	if (PlayerRef->IsMovementControlledByCover())
	{
		return;
	}
	
	float DistSinceLastCheck = FVector::Dist(PlayerRef->GetActorLocation(), LastPlayerPosition);
	if (DistSinceLastCheck < MinDistanceToAvoidGrenade)
	{
		CheckThrowGrenade(true);
	}
	else
	{
		if (CalculateGrenadeTokensTimerHandler.IsValid())
		{
			GetWorld()->GetTimerManager().ClearTimer(CalculateGrenadeTokensTimerHandler);
		}
	}

	LastPlayerPosition = PlayerRef->GetActorLocation();
}

void UEnemyCoordinationComponent::CheckThrowGrenade(bool bNewState)
{
	if (bNewState)
	{
		if (GetWorld()->GetTimerManager().IsTimerActive(CalculateGrenadeTokensTimerHandler))
		{
			return;
		}
		GetWorld()->GetTimerManager().SetTimer(CalculateGrenadeTokensTimerHandler, this, &UEnemyCoordinationComponent::CalculateGrenadeTokenUsers, ThrowGrenadeRate, true, TimeInCoverUntilThrowGrenade);
	}
	else
	{
		if (CalculateGrenadeTokensTimerHandler.IsValid())
		{
			GetWorld()->GetTimerManager().ClearTimer(CalculateGrenadeTokensTimerHandler);
		}
	}
}

void UEnemyCoordinationComponent::AddGrenadeTokenToPool()
{
	GrenadeTokenPool++;
}

void UEnemyCoordinationComponent::ChangeElectricBulletTokenToPool(const bool bIsRequesting, AAgencyCharacter* AgencyCharacter)
{
	if (bIsRequesting)
	{
		if (ElectricBulletTokenPool > 0)
		{
			ElectricBulletTokenPool--;
			AgencyCharacter->SetHasElectricBullet(true);
		}
	}
	else
	{
		ElectricBulletTokenPool++;
			AgencyCharacter->SetHasElectricBullet(false);
	}
}

