// Fill out your copyright notice in the Description page of Project Settings.


#include "Action_SpawnCard.h"

void UAction_SpawnCard::Execute()
{
	OnActionExecute.Broadcast();
}

bool UAction_SpawnCard::Update(float _dt)
{
	OnActionUpdate.Broadcast(_dt);
	return actionCurrTime_ >= actionDuration_;
}

void UAction_SpawnCard::Init()
{
	BindDelegates();
	OnActionInit.Broadcast();
}
