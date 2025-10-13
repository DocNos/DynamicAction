// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Action.h"
#include "Action_Fade.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class ACTIONLIST_API UAction_Fade : public UAction
{
	GENERATED_BODY()
private:

	UPROPERTY()
	float startOpacity_;
	float currOpacity_;
	float endOpacity_;

public:

	virtual void Execute() override;
	virtual bool Update(float) override;
	virtual EActionType GetType() override {return EActionType::Fade; }
	
	void Initialize(AActor* Target, float _startOpacity, 
	float _endOpacity , float Duration)
	{
		affectedObject_ = Target;
		startOpacity_ = _startOpacity;
		endOpacity_ = _startOpacity;
		actionDuration_ = Duration;
		actionCurrTime_ = 0.0f;
		SetType(EActionType::Fade);
	}

	void Init() override;


};
