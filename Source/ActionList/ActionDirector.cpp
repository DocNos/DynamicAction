// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionDirector.h"

void UActionDirector::Init()
{	
	builder_ = NewObject<UActionBuilder>();	
	Super::Init();
	LogDebug("Director Initialized");

}

void UActionDirector::Shutdown()
{
	StopAllActions();
	ActiveActions_.Empty();
	Super::Shutdown();
}

void UActionDirector::Tick(float dt)
{	
	if(!DirectorClass) return;
	ProcessQueue();
	ProcessActive(dt);
	OnDirectorTick.Broadcast(dt);
}

void UActionDirector::ProcessActive(float dt)
{
	for (int32 i = 0; i < ActiveActions_.Num(); ++i)
	{
		UAction* currAction = ActiveActions_[i];
		if (currAction && currAction->IsActive())
		{
			//if (currAction->IsBlocking()) return;
			if (currAction->Update(dt))
			{
				currAction->SetDone(true);
				currAction->SetDeleteFlag(true);
				DeleteMap_.Add(currAction, i);
				//currAction->SetActive(false);
				OnActionCompleted.Broadcast(currAction);
			}
		}
	}
}

void UActionDirector::ProcessQueue()
{

	TArray<FSequence> CompleteSequences;

	for (int32 currID = 0; currID < Sequences_.Num(); ++currID)
	{
		FSequence& currSeq = Sequences_[currID];
		if (currSeq.currActive >= currSeq.SequenceData.Num())
		{
			currSeq.bIsDone = true;
			CompleteSequences.Add(currSeq);
			OnSequenceCompleted.Broadcast(currSeq);
			continue;
		}
		if(currSeq.bIsDone) continue;
		switch (currSeq.type)
		{
			case(SeqType::Sequential): ProcessSequence_Sequential(currSeq); break;
			case(SeqType::Simultaneous): 
			{
				ProcessSequence_Simultaneous(currSeq); 
				CompleteSequences.Add(currSeq);
			}break;
			case(SeqType::Blocking): ProcessSequence_Blocking(currSeq); break;
			default: break;
		}
		
	}
	
	for (auto seq : CompleteSequences)
	{
		int index = Sequences_.Find(seq);
		Sequences_.RemoveAt(index);
	}
	activeSequences_ = Sequences_.Num();
	
}

void UActionDirector::ProcessSequence_Sequential(FSequence& currSeq)
{
	// Current action of current sequence
	UAction* currAction = currSeq.SequenceData[currSeq.currActive];
	if (!currAction) { ++currSeq.currActive; return; }

	if (!currAction->IsActive() && !currAction->DoDelete())
	{
		ExecuteAction(currAction);
	}
	if (currAction->IsDone()) { ++currSeq.currActive; }

}

void UActionDirector::ProcessSequence_Simultaneous(FSequence& sequence)
{
	for (auto action : sequence.SequenceData)
	{
		ExecuteAction(action);
		++sequence.currActive;
	}
	OnSequenceCompleted.Broadcast(sequence);

}

void UActionDirector::ProcessSequence_Blocking(FSequence& currSeq)
{
	UAction* currAction = currSeq.SequenceData[currSeq.currActive];
	

}

void UActionDirector::NewSequence
(int indexOwner, TArray<UAction*> newSequence, SeqType type, FString _sequenceName)
{
	for (auto action : newSequence)
	{
		action->bIsSequence_ = true;
	}
	FSequence newSeq = {type, newSequence, 0, false, indexOwner, _sequenceName};
	Sequences_.Add(newSeq);
	++activeSequences_;
	//return newSeq;
}


void UActionDirector::ExecuteAction(UAction* Action)
{
	if (!Action) //|| !Action->CanExecute())
	{
		LogDebug("Action cannot be executed");
		return;
	}
	if (IsObjectBlocked(Action->affectedObject_) && !Action->IsBlocking())
	{
		LogDebug(FString::Printf(TEXT("%s blocked, queuing action of type %s") 
				, Action->affectedObject_, 
				*UEnum::GetDisplayValueAsText(Action->GetType()).ToString()));
		return;
	}
	Action->actionCurrTime_ = 0.f;
	Action->SetActive(true);
	OnActionStarted.Broadcast(Action);
	Action->Execute();

	ActiveActions_.Add(Action);
	LogDebug(FString::Printf(TEXT("Started execution of type %s: Duration: %.2f"),
							 *UEnum::GetDisplayValueAsText(Action->GetType()).ToString()
							,Action->actionDuration_));
}

void UActionDirector::RemoveActive(UAction* action)
{
	//if(DeleteList_.IsEmpty()) return;
	if (action->DoDelete())
	{
		ActiveActions_.RemoveAt(DeleteMap_[action]);
		action->SetActive(false);
		LogDebug_Red(FString::Printf(TEXT("Finished action") ) ); // %s"),
						//		 action->GetOuter()->GetFName()));
	}
	
}

bool UActionDirector::IsObjectBlocked(AActor* object) const
{
	for (UAction* Action : ActiveActions_)
	{
		if (Action && Action->IsActive() && Action->IsBlocking()
			&& Action->affectedObject_ == object)
		{
			return true;
		}
	}
	return false;
}

void UActionDirector::StopAction(UAction* Action)
{
	if (!Action) return;

	Action->SetActive(false);
	Action->SetDeleteFlag(true);

	LogDebug(FString::Printf(TEXT("Stopped action: %s"),
							 *Action->GetClass()->GetName()));
}

void UActionDirector::StopAllActions()
{
	for (UAction* Action : ActiveActions_)
	{
		if (Action)
		{
			StopAction(Action);
			Action->SetDeleteFlag(true);
		}
		
	}
	LogDebug("Stopped all actions");
}

void UActionDirector::PerformCardShuffle(const TArray<AActor*>& Cards 
	, FVector deckPosition
	,int32 NumShuffles
	,float ShuffleDuration
	,float shuffleRadius
	,float returnDuration
	//, float liftHeight
	, int numIterations
	, bool usePhysics
	, float scatterForce
	, float minPhysSettleTime
	, float maxPhysSettleTime
	, float settleVelThreshold)
{
	if (!builder_ || Cards.Num() == 0)
	{
		LogDebug_Red("Cannot shuffle: No builder or no cards");
		return;
	}

	// Calculate deck position from first card
	FVector DeckPos = deckPosition;
	FRotator initialRotation = Cards[0]->GetActorRotation();
	TArray<UAction*> FullShuffleSequence;
	TArray<AActor*> CurrentCardOrder = Cards;
	int32 numCards = Cards.Num();

	// Store original Z positions of cards
	TArray<float> OriginalZPositions;
	for (AActor* Card : Cards)
	{
		OriginalZPositions.Add(Card->GetActorLocation().Z);
	}
	
	// Track the ACTUAL shuffled order
	TArray<AActor*> FinalShuffledOrder = Cards;


	float totalDelayTime = (NumShuffles - 1) * 0.2f + 0.3f + 0.5f;
	float shuffleTime = ShuffleDuration - totalDelayTime;
	float timePerShuffleIteration = shuffleTime / NumShuffles;

	// Shuffle
	for (int32 i = 0; i < NumShuffles; ++i)
	{
		float SpreadRadius = shuffleRadius + (i * 50.0f);

		UAction_Shuffle* ShuffleAction = builder_->CreateShuffleAction(
			CurrentCardOrder, DeckPos, SpreadRadius
			, timePerShuffleIteration
			//, liftHeight
			, numIterations
			, usePhysics
			, scatterForce
			, minPhysSettleTime
			, maxPhysSettleTime
			, settleVelThreshold);
		
		ShuffleAction->CurrentIteration = i;
		ShuffleAction->TotalIterations = NumShuffles;
		OnDirectorRef_Shuffle.Broadcast(ShuffleAction);
		ShuffleAction->Execute();
		FullShuffleSequence.Add(ShuffleAction);

		for (int32 j = 0; j < numCards; ++j)
		{
			AActor* Card = ShuffleAction->ShuffledCards[j];
			if (!Card) continue;

			UAction_Move* MoveAction = builder_->CreateMoveAction(
				Card,
				ShuffleAction->ShuffledPositions[j],
				timePerShuffleIteration  // All cards should move simultaneously
			);
			MoveAction->bIsBlocking_ = false;
			FullShuffleSequence.Add(MoveAction);
		}

		CurrentCardOrder = ShuffleAction->ShuffledCards;
		FinalShuffledOrder = ShuffleAction->ShuffledCards;

		if (i < NumShuffles - 1)
		{
			UAction_Delay* DelayAction = builder_->CreateDelayAction(
				nullptr, nullptr, 0.0f, 0.2f);
			FullShuffleSequence.Add(DelayAction);
		}
	}

	// Add delay before return
	UAction_Delay* PreReturnDelay = builder_->CreateDelayAction(
		nullptr, nullptr, 0.0f, 0.3f);
	FullShuffleSequence.Add(PreReturnDelay);

	// FIXED: Return cards in shuffled order to correct stack positions
	float CardStackHeight = 0.5f;  // Height between stacked cards
	float BaseZ = DeckPos.Z;  // Base Z position of deck

	for (int32 i = 0; i < FinalShuffledOrder.Num(); ++i)
	{
		// Each card goes to its new position in the shuffled stack
		FVector StackPosition = FVector(
			DeckPos.X,
			DeckPos.Y,
			BaseZ + (i * CardStackHeight)  // Stack based on new shuffled index
		);

		//float returnDuration = 0.3f;  // Fixed duration for all cards

		UAction_Move* ReturnAction = builder_->CreateMoveAction(
			FinalShuffledOrder[i], StackPosition, returnDuration);
		ReturnAction->bIsBlocking_ = false;
		FullShuffleSequence.Add(ReturnAction);

		UAction_Rotate* ResetRotation = builder_->CreateRotateAction(
			FinalShuffledOrder[i], initialRotation, returnDuration);
		ResetRotation->bIsBlocking_ = false;
		FullShuffleSequence.Add(ResetRotation);
	}

	// Add the full shuffle sequence
	NewSequence(0, FullShuffleSequence, SeqType::Sequential, "ShuffleIterations");

	LogDebug(FString::Printf(TEXT("Performing %d shuffle iterations with %d cards"),
							 NumShuffles, Cards.Num()));
}


void UActionDirector::ExecuteDealSequence(UAction_Deal* DealAction)
{
	if (!DealAction || !builder_) return;

	// Execute to generate the deal sequence
	DealAction->Execute();
	
	if (DealAction->DealtCards.Num() == 0) return;

	TArray<UAction*> DealSequence;

	for (int32 i = 0; i < DealAction->DealtCards.Num(); ++i)
	{
		AActor* Card = DealAction->DealtCards[i];
		if (!Card) continue;

		// Add delay between cards (except for first card)
		if (i > 0 && DealAction->DelayBetweenCards > 0)
		{
			UAction_Delay* DelayAction = builder_->CreateDelayAction(
				nullptr, nullptr, 0.0f, DealAction->DelayBetweenCards
			);
			DealSequence.Add(DelayAction);
		}

		// BROADCAST when card starts moving!
		int32 PlayerIdx = DealAction->PlayerAssignments[i];
		DealAction->OnCardDealt.Broadcast(PlayerIdx+1, Card);

		// Move card
		UAction_Move* MoveAction = builder_->CreateMoveAction(
			Card, DealAction->CardDestinations[i], DealAction->DealSpeed);
		MoveAction->bIsBlocking_ = false;
		DealSequence.Add(MoveAction);


		// Rotate to proper orientation BEFORE flip
		FRotator TargetRotation = DealAction->CardRotations[i];

		if (DealAction->CardFlipStates[i])
		{
			// Rotate to face-down position first
			FRotator FaceDownRotation = TargetRotation;
			FaceDownRotation.Pitch = 180.0f;  // Face down

			UAction_Rotate* PreFlipRotate = builder_->CreateRotateAction(
				Card, FaceDownRotation, DealAction->DealSpeed * 0.5f);
			PreFlipRotate->bIsBlocking_ = false;
			DealSequence.Add(PreFlipRotate);

			// Then flip to face up
			UAction_Flip* FlipAction = builder_->CreateFlipAction(
				Card, DealAction->DealSpeed * 0.5f);
			DealSequence.Add(FlipAction);

			// Final rotation adjustment
			UAction_Rotate* PostFlipRotate = builder_->CreateRotateAction(
				Card, TargetRotation, DealAction->DealSpeed * 0.3f);
			PostFlipRotate->bIsBlocking_ = false;
			DealSequence.Add(PostFlipRotate);
		}
		else
		{
			// Just rotate to target
			UAction_Rotate* RotateAction = builder_->CreateRotateAction(
				Card, TargetRotation, DealAction->DealSpeed);
			RotateAction->bIsBlocking_ = false;
			DealSequence.Add(RotateAction);
		}

	}

	// Execute as sequential to maintain proper dealing order
	NewSequence(0, DealSequence, SeqType::Sequential, "DealCards");

	LogDebug(FString::Printf(TEXT("Dealing %d cards to %d players"),
							 DealAction->DealtCards.Num(),
							 DealAction->PlayerHands.Num()));
}


void UActionDirector::DealCards(
	const TArray<AActor*>& Cards,
	const TArray<FVector>& PlayerPositions,
	const TArray<FRotator>& PlayerRotations,
	float dealDelay
	, int32 CardsPerPlayer,
	bool bFaceUp,
	float DealSpeed)
{
	if (!builder_ || Cards.Num() == 0 || PlayerPositions.Num() == 0)
	{
		LogDebug_Red("Cannot deal: Invalid parameters");
		return;
	}

	TArray<FPlayerHand> PlayerHands;
	for (int32 i = 0; i < PlayerPositions.Num(); ++i)
	{
		FPlayerHand Hand;
		Hand.Position = PlayerPositions[i];

		// Calculate rotation to face table center
		FVector DeckPos = Cards[0]->GetActorLocation();
		FVector ToCenter = DeckPos - PlayerPositions[i];
		ToCenter.Z = 0;

		// Set rotation to face center
		if (!ToCenter.IsNearlyZero())
		{
			Hand.Rotation = ToCenter.Rotation();
			Hand.Rotation.Yaw -= 90.f;
		}
		else if (i < PlayerRotations.Num())
		{
			Hand.Rotation = PlayerRotations[i];
		}
		else
		{
			Hand.Rotation = FRotator::ZeroRotator;
		}

		Hand.HandSpread = 35.0f;
		Hand.CardStackOffset = 2.0f;
		Hand.bFaceUp = bFaceUp;
		PlayerHands.Add(Hand);
	}

	// Calculate deck position
	FVector DeckPos = Cards.Num() > 0 ? Cards[0]->GetActorLocation() : FVector::ZeroVector;

	// Create the deal action
	UAction_Deal* DealAction = builder_->CreateDealAction(
		Cards,
		PlayerHands,
		CardsPerPlayer,
		DeckPos,
		DealSpeed
	);

	OnDirectorRef.Broadcast(DealAction);
	// Set additional parameters
	DealAction->DelayBetweenCards = dealDelay;
	DealAction->bDealRoundRobin = true; // Deal one to each player in turn

	// Execute the deal sequence
	ExecuteDealSequence(DealAction);
}


/*
void UActionDirector::ExecuteShuffleSequence(UAction_Shuffle* ShuffleAction)
{
	if (!ShuffleAction || !builder_) return;

	// Execute the shuffle to get positions
	ShuffleAction->Execute();

	if (ShuffleAction->ShuffledCards.Num() == 0) return;

	TArray<UAction*> ShuffleMovements;

	// Use the builder to create move and rotate actions
	for (int32 i = 0; i < ShuffleAction->ShuffledCards.Num(); ++i)
	{
		AActor* Card = ShuffleAction->ShuffledCards[i];
		if (!Card) continue;

		// Create move action using builder
		UAction_Move* MoveAction = builder_->CreateMoveAction(
			Card,
			ShuffleAction->ShuffledPositions[i],
			0.4f
		);
		MoveAction->bIsBlocking_ = false;
		ShuffleMovements.Add(MoveAction);

		// Create rotation for visual interest
		FRotator RandomRotation = FRotator(
			0,
			FMath::FRandRange(-30.0f, 30.0f),
			FMath::FRandRange(-10.0f, 10.0f)
		);

		UAction_Rotate* RotateAction = builder_->CreateRotateAction(
			Card,
			Card->GetActorRotation() + RandomRotation,
			0.4f
		);
		RotateAction->bIsBlocking_ = false;
		ShuffleMovements.Add(RotateAction);
	}

	// Execute all movements simultaneously
	NewSequence(0, ShuffleMovements, SeqType::Simultaneous,
				FString::Printf(TEXT("Shuffle_Visual_%d"),
								ShuffleAction->CurrentIteration));

	LogDebug(FString::Printf(TEXT("Executing shuffle with %d cards"),
							 ShuffleAction->ShuffledCards.Num()));
}


*/