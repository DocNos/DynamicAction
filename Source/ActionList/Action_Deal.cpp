// Fill out your copyright notice in the Description page of Project Settings.


#include "Action_Deal.h"

// Action_Deal.cpp
#include "Action_Deal.h"
#include "Engine/World.h"

void UAction_Deal::Initialize(
	const TArray<AActor*>& InCards,
	const TArray<FPlayerHand>& InPlayerHands,
	int32 InCardsPerPlayer,
	FVector InDeckPosition,
	float InDealSpeed)
{
	Cards = InCards;
	PlayerHands = InPlayerHands;
	CardsPerPlayer = InCardsPerPlayer;
	DeckPosition = InDeckPosition;
	DealSpeed = InDealSpeed;

	// Calculate total duration based on number of cards to deal
	int32 TotalCards = FMath::Min(Cards.Num(), PlayerHands.Num() * CardsPerPlayer);
	actionDuration_ = (DealSpeed + DelayBetweenCards) * TotalCards;

	SetType(EActionType::Deal); 
}

void UAction_Deal::Init()
{
	// If deck position not set, use first card's position
	if (Cards.Num() > 0 && DeckPosition.IsZero())
	{
		DeckPosition = Cards[0]->GetActorLocation();
	}
	OnActionInit.Broadcast();
	BindDelegates();
}

void UAction_Deal::Execute()
{
	GenerateDealSequence();
	OnActionExecute.Broadcast();
}

bool UAction_Deal::Update(float dt)
{
	actionCurrTime_ += dt;

	// Progress tracking for visual feedback
	float Progress = FMath::Clamp(actionCurrTime_ / actionDuration_, 0.0f, 1.0f);
	OnActionUpdate.Broadcast(Progress);

	return actionCurrTime_ >= actionDuration_;
}

void UAction_Deal::GenerateDealSequence()
{
	DealtCards.Empty();
	CardDestinations.Empty();
	CardRotations.Empty();
	PlayerAssignments.Empty();
	CardFlipStates.Empty();

	int32 CardIndex = Cards.Num() - 1;

	if (bDealRoundRobin)
	{
		// Deal one card to each player, then repeat
		for (int32 Round = 0; Round < CardsPerPlayer; ++Round)
		{
			for (int32 PlayerIdx = 0; PlayerIdx < PlayerHands.Num(); ++PlayerIdx)
			{
				if (CardIndex <= 0)
				{
					bDealingComplete = true;
					return;
				}

				DealtCards.Add(Cards[CardIndex]);
				CardDestinations.Add(CalculateCardPosition(PlayerIdx, Round));
				CardRotations.Add(CalculateCardRotation(PlayerIdx, Round));
				PlayerAssignments.Add(PlayerIdx);
				CardFlipStates.Add(PlayerHands[PlayerIdx].bFaceUp);
				CardIndex--;
			}
		}
	}
	else
	{
		// Deal all cards to first player, then second, etc.
		for (int32 PlayerIdx = 0; PlayerIdx < PlayerHands.Num(); ++PlayerIdx)
		{
			for (int32 CardNum = 0; CardNum < CardsPerPlayer; ++CardNum)
			{
				if (CardIndex <= 0)
				{
					bDealingComplete = true;
					return;
				}

				DealtCards.Add(Cards[CardIndex]);
				CardDestinations.Add(CalculateCardPosition(PlayerIdx, CardNum));
				PlayerAssignments.Add(PlayerIdx);
				CardFlipStates.Add(PlayerHands[PlayerIdx].bFaceUp);

				CardIndex--;
			}
		}
	}

	bDealingComplete = true;
}

FVector UAction_Deal::CalculateCardPosition(int32 PlayerIndex, int32 CardIndexInHand)
{
	if (!PlayerHands.IsValidIndex(PlayerIndex))
	{
		return DeckPosition;
	}

	const FPlayerHand& Hand = PlayerHands[PlayerIndex];

	// Calculate position with spread along player's local X axis
	FVector Position = Hand.Position;

	// Apply spread in local hand space
	FVector LocalOffset = FVector(CardIndexInHand * Hand.HandSpread, 0, CardIndexInHand * Hand.CardStackOffset);
	FVector WorldOffset = Hand.Rotation.RotateVector(LocalOffset);
	Position += WorldOffset;

	return Position;
}


FRotator UAction_Deal::CalculateCardRotation(int32 PlayerIndex, int32 CardIndexInHand)
{
	if (!PlayerHands.IsValidIndex(PlayerIndex))
	{
		return FRotator::ZeroRotator;
	}

	const FPlayerHand& Hand = PlayerHands[PlayerIndex];

	// Base rotation matches player's hand orientation
	FRotator CardRotation = Hand.Rotation;

	// Add fan angle for card spread
	float FanAngle = -15.0f + (30.0f * CardIndexInHand / FMath::Max(CardsPerPlayer - 1, 1));
	CardRotation.Yaw += FanAngle;

	// If face down, add 180 degree pitch rotation
	if (!Hand.bFaceUp)
	{
		CardRotation.Pitch += 180.0f;
	}

	return CardRotation;
}

