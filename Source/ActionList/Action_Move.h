// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Action.h"
#include "Action_Move.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class ACTIONLIST_API UAction_Move : public UAction
{
	GENERATED_BODY()
private:
	//EActionType actionType_ = EActionType::Move;


public:	
	
	virtual void Execute() override;
	//virtual void Undo() override;
	virtual bool CanExecute() const override;
	//UFUNCTION(BlueprintCallable)
	virtual bool Update(float) override;
	virtual EActionType GetType() override;
	//float actionDuration;
	//AActor* affectedObject;
	//float actionCurrTime;
	//EActionType type;

public:
	// // Clean one-line creation from anywhere
	// UAction_Move* MoveAction = UAction_Move::Create(this, MyActor, TargetPos, 2.0f);
	UFUNCTION(BlueprintCallable, Category = "Action Factory")
	static UAction_Move* Create(UObject* Outer, AActor* actionable
	, FVector _endPos, float duration)
	{
		UAction_Move* NewAction = NewObject<UAction_Move>(Outer);
		NewAction->Initialize(actionable, _endPos, duration);
		return NewAction;
	}
	void Initialize(AActor* Target, FVector Destination, float Duration)
	{
		affectedObject_ = Target;
		endPos_ = Destination;
		actionDuration_ = Duration;
		actionCurrTime_ = 0.0f;

		if (Target)
		{
			startPos_ = Target->GetActorLocation();
			currPos_ = startPos_;
		}
	}

	// Might need startpos for interp.
	UPROPERTY(BlueprintReadWrite, Category = "Move")
	FVector currPos_;
	
	UPROPERTY(BlueprintReadWrite, Category = "Move")
	FVector endPos_;

	UPROPERTY(BlueprintReadWrite, Category = "Move")
	FVector startPos_;

};
