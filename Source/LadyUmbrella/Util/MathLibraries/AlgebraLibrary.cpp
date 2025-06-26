// Fill out your copyright notice in the Description page of Project Settings.


#include "AlgebraLibrary.h"

float UAlgebraLibrary::GetXYDegreeAngleFromVectorToVector(const FVector& _FromVector, const FVector& _ToVector)
{
	if (_FromVector.IsZero() || _ToVector.IsZero())
	{
		return 0.0f;
	}

	const FVector2D normalizedFromVector = FVector2D(_FromVector.X, _FromVector.Y).GetSafeNormal();
	const FVector2D normalizedToVector = FVector2D(_ToVector.X, _ToVector.Y).GetSafeNormal();

	return FMath::RadiansToDegrees(FMath::Atan2(normalizedToVector.X * normalizedFromVector.X - normalizedToVector.Y * normalizedFromVector.X, FVector2D::DotProduct(normalizedToVector, normalizedFromVector)));
}
