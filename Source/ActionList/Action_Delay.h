// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Action.h"
#include "Action_Delay.generated.h"

/**
 * 
*	Delay an action until completion
* 
 */
UCLASS(Blueprintable)
class ACTIONLIST_API UAction_Delay : public UAction
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	float currPreDelay_;

	UPROPERTY(BlueprintReadWrite)
	float preDelayDuration_;

	UPROPERTY(BlueprintReadWrite)
	UAction* delayedAction_;

	virtual void Execute() override;
	virtual bool Update(float) override;
	virtual EActionType GetType() override { return EActionType::Delay; }

	void Initialize(AActor* Target, UAction* _delayedAction,
					float _preDelay, float Duration)
	{
		affectedObject_ = Target;
		delayedAction_ = _delayedAction;
		bIsBlocking_ = true;
		preDelayDuration_ = _preDelay;
		actionDuration_ = Duration;
		actionCurrTime_ = 0.0f;
		SetType(EActionType::Delay);
	}

	void Init() override;
	
};
