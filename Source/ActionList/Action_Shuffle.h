// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Action.h"
#include "Action_Move.h"
#include "Action_Shuffle.generated.h"

/**
 * 
 */
// Action_Shuffle.h
UCLASS(Blueprintable)
class ACTIONLIST_API UAction_Shuffle : public UAction
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = "Shuffle")
	TArray<AActor*> Cards;

	UPROPERTY(BlueprintReadWrite, Category = "Shuffle")
	FVector DeckPosition = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, Category = "Shuffle")
	float SpreadRadius = 150.0f;

	UPROPERTY(BlueprintReadWrite, Category = "Shuffle")
	float LiftHeight = 50.0f;

	UPROPERTY(BlueprintReadWrite, Category = "Shuffle")
	int32 CurrentIteration = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Shuffle")
	int32 TotalIterations = 3;

	// Stores the shuffled positions for director to use
	UPROPERTY(BlueprintReadOnly, Category = "Shuffle")
	TArray<FVector> ShuffledPositions;

	// Stores the new order after shuffling
	UPROPERTY(BlueprintReadOnly, Category = "Shuffle")
	TArray<AActor*> ShuffledCards;

	virtual void Init() override;
	virtual void Execute() override;
	virtual bool Update(float dt) override;

	UFUNCTION(BlueprintCallable, Category = "Shuffle")
	FVector GetShufflePosition(int32 CardIndex, int32 Iteration);

	UFUNCTION(BlueprintCallable, Category = "Shuffle")
	void Initialize(
		const TArray<AActor*>& InCards,
		FVector InDeckPosition,
		float InSpreadRadius,
		float InDuration
	);

private:
	void ShuffleArray();
};


