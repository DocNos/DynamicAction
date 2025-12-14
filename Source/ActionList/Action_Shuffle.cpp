// Fill out your copyright notice in the Description page of Project Settings.

#include "Action_Shuffle.h"
#include "Engine/World.h"
#include "Math/UnrealMathUtility.h"


void UAction_Shuffle::Init()
{
	Super::Init();
	SetType(EActionType::Shuffle);

	// If deck position not set, use first card's position
	if (Cards.Num() > 0 && DeckPosition.IsZero())
	{
		DeckPosition = Cards[0]->GetActorLocation();
	}
	BindDelegates();
	OnActionInit.Broadcast();
}

void UAction_Shuffle::Execute()
{
	// Shuffle the array
	ShuffleArray();

	// Calculate positions for visual shuffle effect
	ShuffledPositions.Empty();
	for (int32 i = 0; i < ShuffledCards.Num(); ++i)
	{
		FVector ShufflePos = GetShufflePosition(i, CurrentIteration);
		ShuffledPositions.Add(ShufflePos);
	}

	// The director will use ShuffledPositions to create move actions
	OnActionExecute.Broadcast();
}

bool UAction_Shuffle::Update(float dt)
{
	actionCurrTime_ += dt;
	OnActionUpdate.Broadcast(dt);

	if (actionCurrTime_ >= actionDuration_)
	{
		return true;
	}

	return false;
}

FVector UAction_Shuffle::GetShufflePosition(int32 CardIndex, int32 Iteration)
{
	if (Cards.Num() == 0) return DeckPosition;

	FVector Position = DeckPosition;

	// Different patterns for each iteration - NO Z-AXIS CHANGES
	switch (Iteration % 3)
	{
		case 0: // Fan spread - FLAT on table
		{
			float AngleStep = 180.0f / FMath::Max(Cards.Num() - 1, 1);
			float StartAngle = -90.0f;
			float Angle = StartAngle + (AngleStep * CardIndex);
			float RadAngle = FMath::DegreesToRadians(Angle);

			Position.X += SpreadRadius * FMath::Cos(RadAngle);
			Position.Y += SpreadRadius * FMath::Sin(RadAngle) * 0.5f; // Flatten the arc
			// Position.Z stays the same - NO LIFT
			break;
		}

		case 1: // Split into two piles - FLAT on table
		{
			bool bLeftPile = CardIndex < Cards.Num() / 2;
			int32 PileIndex = bLeftPile ? CardIndex : CardIndex - (Cards.Num() / 2);

			Position.X += bLeftPile ? -SpreadRadius * 0.6f : SpreadRadius * 0.6f;
			Position.Y += PileIndex * 2.0f; // Slight offset per card
			// Position.Z stays the same - NO LIFT

			// Add slight rotation offset
			float RotOffset = FMath::FRandRange(-5.0f, 5.0f);
			Position.X += RotOffset;
			break;
		}

		case 2: // Circular wash shuffle - FLAT on table
		{
			float AngleStep = 360.0f / Cards.Num();
			float RadAngle = FMath::DegreesToRadians(AngleStep * CardIndex);
			float RadiusVariation = SpreadRadius * FMath::FRandRange(0.7f, 1.0f);

			Position.X += RadiusVariation * FMath::Cos(RadAngle);
			Position.Y += RadiusVariation * FMath::Sin(RadAngle);
			// Position.Z stays the same - NO LIFT

			// Add some chaos (but only X/Y)
			Position.X += FMath::FRandRange(-20.0f, 20.0f);
			Position.Y += FMath::FRandRange(-20.0f, 20.0f);
			break;
		}
	}

	return Position;
}


void UAction_Shuffle::Initialize(
	const TArray<AActor*>& InCards,
	FVector InDeckPosition,
	float InSpreadRadius,
	float InDuration)
{
	Cards = InCards;
	DeckPosition = InDeckPosition;
	SpreadRadius = InSpreadRadius;
	actionDuration_ = InDuration;

	// Set default values
	LiftHeight = 50.0f;
	TotalIterations = 3;
	CurrentIteration = 0;

}


void UAction_Shuffle::ShuffleArray()
{
	ShuffledCards = Cards;

	// Fisher-Yates shuffle
	for (int32 i = ShuffledCards.Num() - 1; i > 0; --i)
	{
		int32 j = FMath::RandRange(0, i);
		ShuffledCards.Swap(i, j);
	}
}