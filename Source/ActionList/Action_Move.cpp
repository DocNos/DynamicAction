// Fill out your copyright notice in the Description page of Project Settings.


#include "Action_Move.h"
#include "ActionGroup.h"

void UAction_Move::Execute()
{
	startPos_ = affectedObject_->GetTransform().GetLocation();
}




bool UAction_Move::CanExecute() const
{
	return false;
}

bool UAction_Move::Update(float _dt)
{
	//actionCurrTime_ += _dt;
	OnActionUpdate.Broadcast(_dt);
	return (actionCurrTime_ >= actionDuration_);
}

//UAction_Move *UAction_Move::Create(AActor *actionable
//, FVector endPos, float duration)
//{
//	actionType = EActionType::Move;
//	affectedObject = actionable;
//	currPos_ = actionable->GetTransform().GetLocation();
//	actionDuration = duration;
//	endPos_ = endPos;
//	return this;
//}

EActionType UAction_Move::GetType()
{
	return EActionType::Move;
}
