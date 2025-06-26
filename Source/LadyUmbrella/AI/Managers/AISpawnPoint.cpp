// 
// AISpawnPoint.cpp
// 
// Set-up Actor
// 
// Copyright Zulo Interactive. All Rights Reserved.
//


#include "AISpawnPoint.h"

#include "ArenaManager.h"
#include "Components/ArrowComponent.h"
#include "Components/BillboardComponent.h"
#include "LadyUmbrella/AI/SplinePath.h"
#include "LadyUmbrella/Util/Logger.h"

// Sets default values
AAISpawnPoint::AAISpawnPoint()
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorHiddenInGame(true);

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Child Static Mesh"));
	StaticMesh->SetupAttachment(Root);
	StaticMesh->SetMobility(EComponentMobility::Static);
	StaticMesh->SetVisibility(false);
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// ¡Esto es CLAVE!
	StaticMesh->SetUsingAbsoluteLocation(false);
	StaticMesh->SetUsingAbsoluteRotation(false);
	StaticMesh->SetUsingAbsoluteScale(false);

	StaticMesh->SetRelativeLocation(FVector::ZeroVector);
	StaticMesh->SetRelativeRotation(FRotator::ZeroRotator);
	StaticMesh->SetRelativeScale3D(FVector(1.f));

	Billboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> TextureArena(TEXT("/Script/Engine.Texture2D'/Game/Textures/Sprites/Icons/ArenaManager/T_EnemyIcon.T_EnemyIcon'"));
	if (TextureArena.Succeeded())
	{
		Billboard->SetSprite(TextureArena.Object);
	}
	Billboard->SetupAttachment(StaticMesh);

#if WITH_EDITORONLY_DATA
	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Child Arrow"));
	Arrow->SetupAttachment(StaticMesh);
	Arrow->ArrowColor = FColor::Red;
	Arrow->ArrowLength = 100.f;
	Arrow->bIsEditorOnly = true;
#endif

	Target = ETarget::Enemy;
	bHasPath = false;
	bIsReinforcement = false;
	EnterArenaGroup = -1;
	SpawnOrder = 0;
}


void AAISpawnPoint::SetZone(int newzone)
{
	NumZone = newzone;
}

int AAISpawnPoint::GetZone() const
{
	return NumZone;
}

void AAISpawnPoint::SetArenaReference(AArenaManager* AttachedArena)
{
	OwningArenaManager = AttachedArena;
}

// Called when the game starts or when spawned
void AAISpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	if (IsValid(OwningArenaManager) && bIsActive)
	{
		OwningArenaManager->OnRegisterSpawnsDelegate.AddUFunction(this,"RegisterInArenaManager");
	}
}
#if WITH_EDITOR
void AAISpawnPoint::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AAISpawnPoint, Target))
	{
		switch (Target)
		{
		case ETarget::Enemy:
			if (EnemyIcon)
			{
				Billboard->SetSprite(EnemyIcon);
			}
			break;
		case ETarget::Interactable:
			if (InteractableIcon)
			{
				Billboard->SetSprite(InteractableIcon);
			}
			break;
		default:
			break;
		}
	}
}

void AAISpawnPoint::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (!IsValid(Path) && bHasPath)
	{
		FVector Location = GetActorLocation() + FVector(0, 200, 0);
		FActorSpawnParameters Params;
		Params.Owner = this;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
		Path = GetWorld()->SpawnActor<ASplinePath>(SplineClass, Location, FRotator::ZeroRotator, Params);
	}	
}
#endif
// Called every frame
void AAISpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAISpawnPoint::Destroyed()
{
	Super::Destroyed();
}

void AAISpawnPoint::RegisterInArenaManager()
{
	if (bIsActive)
	{
		OwningArenaManager->RegisterSpawn(this);
	}
}

