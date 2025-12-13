// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionBuilder.h"

// Sets default values for this component's properties
UActionBuilder::UActionBuilder()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UActionBuilder::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UActionBuilder::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

UAction* UActionBuilder::CreateAction(EActionType type, AActor* affectedObject, float duration)
{
	UAction* newAction = nullptr;
	switch (type)
	{
		case(EActionType::Move): newAction = NewObject<UAction_Move>(this);
		break;


		default: return nullptr;
	}

	newAction->BindDelegates();
	newAction->Init();
	return newAction;
}

UAction_Move* UActionBuilder::CreateMoveAction(AActor* affectedObject, FVector endPos, float duration)
{
	UAction_Move* newMove = NewObject<UAction_Move>(this, MoveActionClass);
	newMove->Initialize(affectedObject, endPos, duration);
	newMove->Init();

	return newMove;
}

UAction_Fade* UActionBuilder::CreateFadeAction(AActor* affectedObject, float startOpacity, float endOpacity, float duration)
{
	UAction_Fade* newFade = NewObject<UAction_Fade>(this, FadeActionClass);
	newFade->Initialize(affectedObject, startOpacity, endOpacity, duration);
	newFade->Init();
	return newFade;
}

UAction_Rotate* UActionBuilder::CreateRotateAction(AActor* affectedObject,FRotator endRotation, float duration)
{
	UAction_Rotate* newRotate = NewObject<UAction_Rotate>(this, RotateActionClass);
	newRotate->Initialize(affectedObject, endRotation, duration);
	newRotate->Init();
	return newRotate;
}

UAction_Flip* UActionBuilder::CreateFlipAction(AActor* affectedObject, float duration)
{
	UAction_Flip* newFlip = NewObject<UAction_Flip>(this, FlipActionClass);
	newFlip->Initialize(affectedObject, duration);
	newFlip->Init();
	return newFlip;
}

UAction_Delay* UActionBuilder::CreateDelayAction(AActor* affectedObject, UAction* delayedAction,
		float preDelay, float Duration)
{
	UAction_Delay* newDelay = NewObject<UAction_Delay>(this, DelayActionClass);
	newDelay->Initialize(affectedObject, delayedAction, preDelay, Duration);
	newDelay->Init();
	return newDelay;
}

UAction_SpawnCard* UActionBuilder::CreateSpawnAction(int playerOwner, int cardValue
, FRotator spawnRotation, FVector spawnLocation, float duration)
{
	UAction_SpawnCard* newSpawn = NewObject<UAction_SpawnCard>(this, SpawnActionClass);
	newSpawn->Initialize(playerOwner, cardValue, spawnRotation, spawnLocation, duration);
	newSpawn->Init();
	return newSpawn;
}

UAction_Shuffle* UActionBuilder::CreateShuffleAction(
	const TArray<AActor*>& Cards,
	FVector DeckPosition,
	float SpreadRadius,
	float Duration)
{
	UAction_Shuffle* newShuffle = NewObject<UAction_Shuffle>(this, ShuffleActionClass);
	newShuffle->Initialize(Cards, DeckPosition, SpreadRadius, Duration);
	newShuffle->Init();

	return newShuffle;
}


UAction_Deal* UActionBuilder::CreateDealAction(
	const TArray<AActor*>& Cards,
	const TArray<FPlayerHand>& PlayerHands,
	int32 CardsPerPlayer,
	FVector DeckPosition,
	float DealSpeed)
{
	UAction_Deal* newDeal = NewObject<UAction_Deal>(this, DealActionClass);
	newDeal->Initialize(Cards, PlayerHands, CardsPerPlayer, DeckPosition, DealSpeed);
	newDeal->Init();

	return newDeal;
}