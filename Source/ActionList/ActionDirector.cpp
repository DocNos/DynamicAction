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
			if (UAction_Shuffle* ShuffleAction = Cast<UAction_Shuffle>(currAction))
			{
				if (ShuffleAction->ShuffledPositions.Num() > 0 &&
					ShuffleAction->actionCurrTime_ == 0) // First frame
				{
					ExecuteShuffleSequence(ShuffleAction);
				}
			}

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

void UActionDirector::PerformCardShuffle(
	const TArray<AActor*>& Cards
	,int32 NumShuffles
	,float ShuffleDuration
	,float shuffleRadius)
{
	if (!builder_ || Cards.Num() == 0)
	{
		LogDebug_Red("Cannot shuffle: No builder or no cards");
		return;
	}

	// Calculate deck position from first card
	FVector DeckPos = Cards[0]->GetActorLocation();

	TArray<UAction*> FullShuffleSequence;
	TArray<AActor*> CurrentCardOrder = Cards;

	for (int32 i = 0; i < NumShuffles; ++i)
	{
		// Create shuffle action using builder
		float IterationDuration = ShuffleDuration / NumShuffles;
		float SpreadRadius = shuffleRadius + (i * 50.0f); // Increase spread each iteration

		UAction_Shuffle* ShuffleAction = builder_->CreateShuffleAction(
			CurrentCardOrder,
			DeckPos,
			SpreadRadius,
			IterationDuration
		);

		// Set iteration information
		ShuffleAction->CurrentIteration = i;
		ShuffleAction->TotalIterations = NumShuffles;

		// Execute to generate shuffled positions and order
		ShuffleAction->Execute();

		// Create the visual movement sequence
		ExecuteShuffleSequence(ShuffleAction);

		// Add the shuffle action itself to track completion
		FullShuffleSequence.Add(ShuffleAction);

		// Update card order for next iteration
		CurrentCardOrder = ShuffleAction->ShuffledCards;

		// Add delay between shuffle iterations using builder
		if (i < NumShuffles - 1)
		{
			UAction_Delay* DelayAction = builder_->CreateDelayAction(
				nullptr,    // affectedObject
				nullptr,    // delayedAction
				0.0f,       // preDelay
				0.2f        // duration
			);
			FullShuffleSequence.Add(DelayAction);
		}
	}

	// Add a delay before returning cards to deck
	UAction_Delay* PreReturnDelay = builder_->CreateDelayAction(
		nullptr, nullptr, 0.0f, 0.3f
	);
	FullShuffleSequence.Add(PreReturnDelay);

	// Return cards to deck position at the end using builder
	TArray<UAction*> ReturnSequence;
	for (int32 i = 0; i < CurrentCardOrder.Num(); ++i)
	{
		// Calculate stacked position
		FVector StackPosition = DeckPos + FVector(0, 0, i * 0.5f);

		// Create move action using builder
		UAction_Move* ReturnAction = builder_->CreateMoveAction(
			CurrentCardOrder[i],
			StackPosition,
			0.3f
		);
		ReturnAction->bIsBlocking_ = false;
		ReturnSequence.Add(ReturnAction);

		// Reset rotation to zero using builder
		UAction_Rotate* ResetRotation = builder_->CreateRotateAction(
			CurrentCardOrder[i],
			FRotator::ZeroRotator,
			0.3f
		);
		ResetRotation->bIsBlocking_ = false;
		ReturnSequence.Add(ResetRotation);
	}

	// Add the full shuffle sequence
	NewSequence(0, FullShuffleSequence, SeqType::Sequential, "ShuffleIterations");

	// Add return to deck as simultaneous movements
	NewSequence(0, ReturnSequence, SeqType::Simultaneous, "ReturnToDeck");

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

		// Move card from deck to destination
		UAction_Move* MoveAction = builder_->CreateMoveAction(
			Card,
			DealAction->CardDestinations[i],
			DealAction->DealSpeed
		);
		MoveAction->bIsBlocking_ = false;
		DealSequence.Add(MoveAction);

		// Flip card if needed
		if (DealAction->CardFlipStates[i])
		{
			// Face up - might need to flip from face down
			UAction_Flip* FlipAction = builder_->CreateFlipAction(
				Card,
				DealAction->DealSpeed * 0.5f
			);
			DealSequence.Add(FlipAction);
		}

		// Optional: Add a slight rotation for natural look
		FRotator SlightRotation = FRotator(
			0,
			FMath::FRandRange(-5.0f, 5.0f),
			0
		);
		UAction_Rotate* RotateAction = builder_->CreateRotateAction(
			Card,
			Card->GetActorRotation() + SlightRotation,
			DealAction->DealSpeed
		);
		RotateAction->bIsBlocking_ = false;
		DealSequence.Add(RotateAction);
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
	int32 CardsPerPlayer,
	bool bFaceUp,
	float DealSpeed)
{
	if (!builder_ || Cards.Num() == 0 || PlayerPositions.Num() == 0)
	{
		LogDebug_Red("Cannot deal: Invalid parameters");
		return;
	}

	// Create player hand structures from positions
	TArray<FPlayerHand> PlayerHands;
	for (const FVector& Pos : PlayerPositions)
	{
		FPlayerHand Hand;
		Hand.Position = Pos;
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
	DealAction->DelayBetweenCards = 0.1f;
	DealAction->bDealRoundRobin = true; // Deal one to each player in turn

	// Execute the deal sequence
	ExecuteDealSequence(DealAction);
}