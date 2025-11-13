// Fill out your copyright notice in the Description page of Project Settings.


#include "Action_Await.h"

void UAction_Await::Init()
{
	BindDelegates();
	OnActionInit.Broadcast();
}

void UAction_Await::Execute()
{

	OnActionExecute.Broadcast();
}

bool UAction_Await::Update(float _dt)
{

	OnActionUpdate.Broadcast(_dt);
	return actionCurrTime_ >= actionDuration_;
	
}
