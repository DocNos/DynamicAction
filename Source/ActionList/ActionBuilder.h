// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
// #include "Action.h"
#include "Action_Move.h"
#include "Action_Fade.h"
#include "Action_Rotate.h"
#include "Action_Flip.h"
#include "Action_Delay.h"
#include "Action_SpawnCard.h"
#include "Action_Shuffle.h"
#include "Action_Deal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "ActionBuilder.generated.h"

//USTRUCT(Blueprintable)
//struct ActionSequence : public FTableRowBase
//{
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
//	EActionType ActionType = EActionType::Move;
//
//	
//};



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONLIST_API UActionBuilder : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UActionBuilder();

	//UFUNCTION(BlueprintCallable, Category="Action Builder")
	//UAction* CreateAction(EActionType type);

	UFUNCTION(BlueprintCallable, Category="Action Builder")
	UAction* CreateAction(EActionType type, AActor* affectedObject, float duration);

	UFUNCTION(BlueprintCallable, Category="Action Builder")
	UAction_Move* CreateMoveAction(AActor* affectedObject, FVector endPos, float duration);

	UFUNCTION(BlueprintCallable, Category="Action Builder")
	UAction_Fade* CreateFadeAction(AActor* affectedObject, float startOpacity, float endOpacity,
	float duration);

	UFUNCTION(BlueprintCallable, Category="Action Builder")
	UAction_Rotate* CreateRotateAction(AActor* affectedObject,
	FRotator endRotation, float duration);

	UFUNCTION(BlueprintCallable, Category="Action Builder")
	UAction_Flip* CreateFlipAction(AActor* affectedObject, float duration);

	UFUNCTION(BlueprintCallable, Category= "Action Builder")
	UAction_Delay* CreateDelayAction(AActor* affectedObject, UAction* DelayedAction,
					float preDelay, float duration);

	UFUNCTION(BlueprintCallable, Category = "Action Builder")
	UAction_SpawnCard* CreateSpawnAction(int playerOwner, int cardValue, FRotator spawnRotation,
						FVector spawnLocation, float duration);

	UFUNCTION(BlueprintCallable, Category = "Action Builder")
	UAction_Shuffle* CreateShuffleAction(
		const TArray<AActor*>& Cards,
		FVector DeckPosition,
		float SpreadRadius,
		float Duration
		//, float liftHeight
		, int numIterations
		, bool usePhysics
		, float scatterForce
		, float minPhysSettleTime
		, float maxPhysSettleTime
		, float settleVelThreshold
	);

	UFUNCTION(BlueprintCallable, Category = "Action Builder")
	UAction_Deal* CreateDealAction(
		const TArray<AActor*>& Cards,
		const TArray<FPlayerHand>& PlayerHands,
		int32 CardsPerPlayer,
		FVector DeckPosition,
		float DealSpeed
	);

	UFUNCTION(BlueprintCallable, Category="Action Builder")
	void SetMoveClass(TSubclassOf<UAction_Move> moveClass) { MoveActionClass = moveClass;}

	UPROPERTY(EditAnywhere, Category = "Action Builder")
	TSubclassOf<UAction_Move> MoveActionClass;

	UFUNCTION(BlueprintCallable, Category = "Action Builder")
	void SetFadeClass(TSubclassOf<UAction_Fade> fadeClass) { FadeActionClass = fadeClass; }

	UPROPERTY(EditAnywhere, Category = "Action Builder")
	TSubclassOf<UAction_Fade> FadeActionClass;

	UFUNCTION(BlueprintCallable, Category = "Action Builder")
	void SetRotateClass(TSubclassOf<UAction_Rotate> rotateClass) { RotateActionClass = rotateClass; }

	UPROPERTY(EditAnywhere, Category = "Action Builder")
	TSubclassOf<UAction_Rotate> RotateActionClass;
	
	UFUNCTION(BlueprintCallable, Category = "Action Builder")
	void SetFlipClass(TSubclassOf<UAction_Flip> flipClass) { FlipActionClass = flipClass; }

	UPROPERTY(EditAnywhere, Category = "Action Builder")
	TSubclassOf<UAction_Flip> FlipActionClass;

	UFUNCTION(BlueprintCallable, Category="Action Builder")
	void SetDelayClass(TSubclassOf<UAction_Delay> delayClass) { DelayActionClass = delayClass; }

	UPROPERTY(EditAnywhere, Category= "Action Builder")
	TSubclassOf<UAction_Delay> DelayActionClass;

	UFUNCTION(BlueprintCallable, Category = "Action Builder")
	void SetSpawnClass(TSubclassOf<UAction_SpawnCard> spawnClass) { SpawnActionClass = spawnClass; }

	UPROPERTY(EditAnywhere, Category = "Action Builder")
	TSubclassOf<UAction_SpawnCard> SpawnActionClass;

	UFUNCTION(BlueprintCallable, Category = "Action Builder")
	void SetShuffleClass(TSubclassOf<UAction_Shuffle> shuffleClass) { ShuffleActionClass = shuffleClass; }

	UPROPERTY(EditAnywhere, Category = "Action Builder")
	TSubclassOf<UAction_Shuffle> ShuffleActionClass;

	UFUNCTION(BlueprintCallable, Category = "Action Builder")
	void SetDealClass(TSubclassOf<UAction_Deal> dealClass) { DealActionClass = dealClass; }

	UPROPERTY(EditAnywhere, Category = "Action Builder")
	TSubclassOf<UAction_Deal> DealActionClass;

	UPROPERTY(EditAnywhere, Category = "Action Builder")
	float DefaultDuration = 1.0f;

	//UFUNCTION(BlueprintCallable, Category="Action Builder|Sequencing")
	////TArray<UAction*> CreateSequence(TArray<EActionType> actions, float duration
		//				, float predelay, float delay);
	

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
