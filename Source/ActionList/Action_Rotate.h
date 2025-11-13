// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Action.h"
#include "Action_Rotate.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class ACTIONLIST_API UAction_Rotate : public UAction
{
	GENERATED_BODY()
private:
	UPROPERTY()
	FRotator endRotation_;

	UPROPERTY()
	FRotator startRotation_;

public:

	virtual void Execute() override;
	virtual bool Update(float) override;
	virtual EActionType GetType() override { return EActionType::Rotate; }

	void Initialize(AActor* Target, 
	FRotator _endRotation, float Duration)
	{
		affectedObject_ = Target;
		//startRotation_ = affectedObject_->GetActorRotation();
		endRotation_ = _endRotation;
		actionDuration_ = Duration;
		actionCurrTime_ = 0.0f;
		SetType(EActionType::Rotate);
	}

	void Init() override;
	
};
