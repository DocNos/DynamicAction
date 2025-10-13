// Fill out your copyright notice in the Description page of Project Settings.


#include "Action_Flip.h"

void UAction_Flip::Execute()
{
	currRotation_ = affectedObject_->GetActorRotation();
	flippedRotation_ = currRotation_.Add(180, 0, 0);
}

bool UAction_Flip::Update(float _dt)
{
	OnActionUpdate.Broadcast(_dt);
	return actionCurrTime_ >= actionDuration_;
}

void UAction_Flip::Init()
{
	BindDelegates();
	OnActionInit.Broadcast();
}
