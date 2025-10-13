// Fill out your copyright notice in the Description page of Project Settings.


#include "Action_Delay.h"

void UAction_Delay::Execute()
{
	currPreDelay_ = 0.f;
	
}

bool UAction_Delay::Update(float dt)
{
	
	if (currPreDelay_ >= preDelayDuration_)
	{
		actionCurrTime_ += dt;
		return (actionCurrTime_ >= actionDuration_);
	}
	else
	{
		currPreDelay_ += dt;
	}
	return false;
}

void UAction_Delay::Init()
{
	BindDelegates();
	OnActionInit.Broadcast();
}
