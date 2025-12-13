// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Action.h"
#include "Action_Deal.generated.h"

USTRUCT(BlueprintType)
struct FPlayerHand
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FVector Position;

	UPROPERTY(BlueprintReadWrite)
	float HandSpread = 30.0f;

	UPROPERTY(BlueprintReadWrite)
	float CardStackOffset = 2.0f;

	UPROPERTY(BlueprintReadWrite)
	bool bFaceUp = false;
};

UCLASS(Blueprintable)
class ACTIONLIST_API UAction_Deal : public UAction
{
	GENERATED_BODY()

public:
	// Input parameters
	UPROPERTY(BlueprintReadWrite, Category = "Deal")
	TArray<AActor*> Cards;

	UPROPERTY(BlueprintReadWrite, Category = "Deal")
	TArray<FPlayerHand> PlayerHands;

	UPROPERTY(BlueprintReadWrite, Category = "Deal")
	int32 CardsPerPlayer = 5;

	UPROPERTY(BlueprintReadWrite, Category = "Deal")
	FVector DeckPosition;

	UPROPERTY(BlueprintReadWrite, Category = "Deal")
	float DealSpeed = 0.3f;

	UPROPERTY(BlueprintReadWrite, Category = "Deal")
	float DelayBetweenCards = 0.1f;

	UPROPERTY(BlueprintReadWrite, Category = "Deal")
	bool bDealRoundRobin = true; // true = one to each player then repeat

	// Output - for director to process
	UPROPERTY(BlueprintReadOnly, Category = "Deal")
	TArray<AActor*> DealtCards;

	UPROPERTY(BlueprintReadOnly, Category = "Deal")
	TArray<FVector> CardDestinations;

	UPROPERTY(BlueprintReadOnly, Category = "Deal")
	TArray<int32> PlayerAssignments;

	UPROPERTY(BlueprintReadOnly, Category = "Deal")
	TArray<bool> CardFlipStates;

	// State tracking
	UPROPERTY(BlueprintReadOnly, Category = "Deal")
	int32 CurrentCardIndex = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Deal")
	bool bDealingComplete = false;

	// Methods
	UFUNCTION(BlueprintCallable, Category = "Deal")
	void Initialize(
		const TArray<AActor*>& InCards,
		const TArray<FPlayerHand>& InPlayerHands,
		int32 InCardsPerPlayer,
		FVector InDeckPosition,
		float InDealSpeed
	);

	virtual void Init() override;
	virtual void Execute() override;
	virtual bool Update(float dt) override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Deal")
	FVector CalculateCardPosition(int32 PlayerIndex, int32 CardIndexInHand);

private:
	void GenerateDealSequence();
};