// Fill out your copyright notice in the Description page of Project Settings.


#include "ElectricFenceElement.h"
#include "Components/BoxComponent.h"

AElectricFenceElement::AElectricFenceElement()
{
	// Change the mesh to a cube
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshAsset(TEXT("/Game/Meshes/StaticMeshes/Placeholder/Primitives/Shape_Cube.Shape_Cube"));
	if (CubeMeshAsset.Succeeded())
	{
		ObjectMesh->SetStaticMesh(CubeMeshAsset.Object);
	}

	MeshScale = FVector(1.f, 0.1f, 2.f);
	MeshPosition = FVector(0.0f, 0.f, -100.f);
	ObjectMesh->SetRelativeLocation(MeshPosition);
	if (BoxRoot)
	{
		BoxRoot->SetBoxExtent(FVector(50.f, 10.f, 100.f));
		BoxRoot->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
	}
}
