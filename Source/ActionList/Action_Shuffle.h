// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Action.h"
#include "Action_Move.h"
#include "Action_Shuffle.generated.h"

USTRUCT(BlueprintType)
struct FPhysicsCardData
{
	GENERATED_BODY()

	AActor* Card;
	int32 OriginalIndex;
	float SettleTime;
};

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

	//UPROPERTY(BlueprintReadWrite, Category = "Shuffle")
	//float LiftHeight;

	UPROPERTY(BlueprintReadWrite, Category = "Shuffle")
	int32 CurrentIteration = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Shuffle")
	int32 TotalIterations = 3;

	// Stores the shuffled positions for director to use
	UPROPERTY(BlueprintReadWrite, Category = "Shuffle")
	TArray<FVector> ShuffledPositions;

	// Stores the new order after shuffling
	UPROPERTY(BlueprintReadWrite, Category = "Shuffle")
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
		//, float liftHeight
		, int numIterations
		, bool usePhysics
		, float ScatterForce
		, float minPhysSettleTime
		, float maxPhysSettleTime
		, float settleVelThreshold
	);

public:
	// Physics shuffle properties
	UPROPERTY(BlueprintReadWrite, Category = "Shuffle|Physics")
	bool bUsePhysicsShuffle = false;

	UPROPERTY(BlueprintReadWrite, Category = "Shuffle|Physics")
	float ScatterForce = 500.0f;

	UPROPERTY(BlueprintReadWrite, Category = "Shuffle|Physics")
	float MinPhysicsSettleTime = 2.0f;

	UPROPERTY(BlueprintReadWrite, Category = "Shuffle|Physics")
	float MaxPhysicsSettleTime = 5.0f;

	UPROPERTY(BlueprintReadWrite, Category = "Shuffle|Physics")
	float SettleVelocityThreshold = 10.0f;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnPhysicsShuffleComplete, const TArray<AActor*>&, NewOrder);

	UPROPERTY(BlueprintAssignable)
	FOnPhysicsShuffleComplete OnPhysicsShuffleComplete;

	

	TArray<FPhysicsCardData> PhysicsShuffleData;
	TArray<AActor*> PhysicsShuffleOrder;
	bool bPhysicsActive = false;
	float PhysicsSettleTime = 0.0f;

	void ExecuteStandardShuffle();
	void ExecutePhysicsShuffle();
	void UpdatePhysicsShuffle(float dt);
	void FinalizePhysicsShuffle();


private:
	void ShuffleArray();
};


