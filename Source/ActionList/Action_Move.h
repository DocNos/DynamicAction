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
	EActionType actionType = EActionType::Move;


public:	
	
	virtual void Execute() override;
	virtual void Undo() override;
	virtual bool CanExecute() const override;	
	virtual bool Update(float) override;
	virtual EActionType GetType() override;
	//float actionDuration;
	//AActor* affectedObject;
	//float actionCurrTime;
	//EActionType type;

public:
	UFUNCTION(BlueprintCallable)
	UAction_Move *Create(AActor *actionable, FVector endPos, float duration);

	// Might need startpos for interp.
	UPROPERTY(BlueprintReadWrite, Category = "Move")
	FVector currPos_;
	
	UPROPERTY(BlueprintReadWrite, Category = "Move")
	FVector endPos_;
};
