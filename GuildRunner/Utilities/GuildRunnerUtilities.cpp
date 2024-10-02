// Fill out your copyright notice in the Description page of Project Settings.


#include "GuildRunnerUtilities.h"

FVector UGuildRunnerUtilities::SnapVectorToVector(const FVector V1, const FVector V2)
{
	const auto X = FMath::GridSnap(V1.X, V2.X);
	const auto Y = FMath::GridSnap(V1.Y, V2.Y);
	const auto Z = FMath::GridSnap(V1.Z, V2.Z);

	return FVector(X, Y, Z);
}

bool UGuildRunnerUtilities::IsFloatEven(const float Value)
{
	return FMath::IsNearlyZero(FMath::Fmod(Value, 2.f));
}
