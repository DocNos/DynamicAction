// Fill out your copyright notice in the Description page of Project Settings.


#include "Action_Fade.h"

void UAction_Fade::Execute()
{
	currOpacity_ = startOpacity_;
	
}

bool UAction_Fade::Update(float _dt)
{
	OnActionUpdate.Broadcast(_dt);
	return (actionCurrTime_ >= actionDuration_);
}

void UAction_Fade::Init()
{
	BindDelegates();
	//Super::Init();
	OnActionInit.Broadcast();
}
