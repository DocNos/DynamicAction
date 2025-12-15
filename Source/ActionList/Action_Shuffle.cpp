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
	if (bUsePhysicsShuffle)
	{
		ExecutePhysicsShuffle();
	}
	else
	{
		ExecuteStandardShuffle();
	}
	// Shuffle the array
	//ShuffleArray();
	//
	//// Calculate positions for visual shuffle effect
	//ShuffledPositions.Empty();
	//for (int32 i = 0; i < ShuffledCards.Num(); ++i)
	//{
	//	FVector ShufflePos = GetShufflePosition(i, CurrentIteration);
	//	ShuffledPositions.Add(ShufflePos);
	//}
	//
	//// The director will use ShuffledPositions to create move actions
	//OnActionExecute.Broadcast();
}

void UAction_Shuffle::ExecuteStandardShuffle()
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

	OnActionExecute.Broadcast();
}

void UAction_Shuffle::ExecutePhysicsShuffle()
{
	// Enable physics and scatter cards
	ShuffledPositions.Empty();
	PhysicsShuffleOrder.Empty();

	for (int32 i = 0; i < Cards.Num(); ++i)
	{
		AActor* Card = Cards[i];
		if (!Card) continue;

		// Enable physics on the card
		if (UPrimitiveComponent* RootComp = Cast<UPrimitiveComponent>(Card->GetRootComponent()))
		{
			RootComp->SetSimulatePhysics(true);
			RootComp->SetEnableGravity(true);

			// Apply random impulse to scatter
			FVector RandomImpulse = FVector(
				FMath::FRandRange(-ScatterForce, ScatterForce),
				FMath::FRandRange(-ScatterForce, ScatterForce),
				FMath::FRandRange(100.0f, 300.0f)  // Upward impulse
			);

			// Add some random torque for spinning
			FVector RandomTorque = FVector(
				FMath::FRandRange(-50.0f, 50.0f),
				FMath::FRandRange(-50.0f, 50.0f),
				FMath::FRandRange(-100.0f, 100.0f)
			);

			RootComp->AddImpulse(RandomImpulse);
			RootComp->AddTorqueInRadians(RandomTorque);
		}

		// Store initial position for tracking
		PhysicsShuffleData.Add(FPhysicsCardData{ Card, i, 0.0f });
	}

	// Start tracking physics settling
	bPhysicsActive = true;
	PhysicsSettleTime = 0.0f;

	OnActionExecute.Broadcast();
}


bool UAction_Shuffle::Update(float dt)
{
	actionCurrTime_ += dt;

	if (bPhysicsActive)
	{
		UpdatePhysicsShuffle(dt);
	}

	OnActionUpdate.Broadcast(dt);

	if (actionCurrTime_ >= actionDuration_)
	{
		if (bPhysicsActive)
		{
			FinalizePhysicsShuffle();
		}
		return true;
	}

	return false;
}

void UAction_Shuffle::UpdatePhysicsShuffle(float dt)
{
	PhysicsSettleTime += dt;

	// Check if cards have settled (after minimum time)
	if (PhysicsSettleTime > MinPhysicsSettleTime)
	{
		bool bAllSettled = true;

		for (auto& CardData : PhysicsShuffleData)
		{
			if (!CardData.Card) continue;

			if (UPrimitiveComponent* RootComp = Cast<UPrimitiveComponent>(CardData.Card->GetRootComponent()))
			{
				FVector Velocity = RootComp->GetPhysicsLinearVelocity();
				if (Velocity.Size() > SettleVelocityThreshold)
				{
					bAllSettled = false;
					break;
				}
			}
		}

		if (bAllSettled || PhysicsSettleTime > MaxPhysicsSettleTime)
		{
			FinalizePhysicsShuffle();
		}
	}
}

void UAction_Shuffle::FinalizePhysicsShuffle()
{
	// Sort cards by their final X position (or Y, or distance from center)
	// This determines the new shuffled order
	PhysicsShuffleData.Sort([](const FPhysicsCardData& A, const FPhysicsCardData& B)
	{
		// Sort by X position (leftmost to rightmost)
		return A.Card->GetActorLocation().X < B.Card->GetActorLocation().X;

		// Alternative: Sort by distance from deck center
		// FVector DeckPos = ...; 
		// float DistA = FVector::Dist2D(A.Card->GetActorLocation(), DeckPos);
		// float DistB = FVector::Dist2D(B.Card->GetActorLocation(), DeckPos);
		// return DistA < DistB;
	});

	// Create new shuffled order based on physics results
	ShuffledCards.Empty();
	for (const auto& CardData : PhysicsShuffleData)
	{
		ShuffledCards.Add(CardData.Card);

		// Disable physics
		if (UPrimitiveComponent* RootComp = Cast<UPrimitiveComponent>(CardData.Card->GetRootComponent()))
		{
			RootComp->SetSimulatePhysics(false);
			RootComp->SetEnableGravity(false);
		}
	}

	bPhysicsActive = false;
	OnPhysicsShuffleComplete.Broadcast(ShuffledCards);
}


FVector UAction_Shuffle::GetShufflePosition(int32 CardIndex, int32 Iteration)
{
	if (Cards.Num() == 0) return DeckPosition;

	float CardZ = DeckPosition.Z;
	//if (CardIndex < Cards.Num() && Cards[CardIndex])
	//{
	//	CardZ = Cards[CardIndex]->GetActorLocation().Z;
	//}
	FVector Position = DeckPosition;
	Position.Z = CardZ;

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
    const TArray<AActor*>& InCards
	,FVector InDeckPosition
	,float InSpreadRadius
	,float InDuration
	//, float liftHeight
	, int numIterations
	, bool usePhysics
	, float scatterForce
	, float minPhysSettleTime
	, float maxPhysSettleTime
	, float settleVelThreshold)
{
    Cards = InCards;
    DeckPosition = InDeckPosition;
    SpreadRadius = InSpreadRadius;
    actionDuration_ = InDuration;
	bUsePhysicsShuffle = usePhysics;
	ScatterForce = scatterForce;
	MinPhysicsSettleTime = minPhysSettleTime;
	MaxPhysicsSettleTime = maxPhysSettleTime;
	SettleVelocityThreshold = settleVelThreshold;

    // Set default values
    //LiftHeight = liftHeight;
    TotalIterations = numIterations;
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