// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Action.h"
#include "Action_Await.generated.h"


//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(F)

/**
 * 
 */
UCLASS(Blueprintable)
class ACTIONLIST_API UAction_Await : public UAction
{
	GENERATED_BODY()



public:
	UAction* waitedAction_;

	void Initialize(UAction* _awaitedAction)
	{
		waitedAction_ = _awaitedAction;
		SetType(EActionType::AwaitAction);
	}

//------------Core	

	
	virtual void Init() override;	
	virtual void Execute() override;	
	virtual bool Update(float _dt) override;
	
};
