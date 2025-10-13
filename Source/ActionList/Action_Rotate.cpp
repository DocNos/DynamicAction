// Fill out your copyright notice in the Description page of Project Settings.


#include "Action_Rotate.h"

void UAction_Rotate::Execute()
{
	
}

bool UAction_Rotate::Update(float _dt)
{
	float Alpha = actionCurrTime_ / actionDuration_;
	FRotator NewRot = FMath::Lerp(startRotation_, endRotation_, Alpha);
	affectedObject_->SetActorRotation(NewRot);
	OnActionUpdate.Broadcast(_dt);
	return actionCurrTime_ >= actionDuration_;
}

void UAction_Rotate::Init()
{
	BindDelegates();
	OnActionInit.Broadcast();
}
