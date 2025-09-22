// Fill out your copyright notice in the Description page of Project Settings.


#include "Action_Move.h"
#include "ActionGroup.h"

void UAction_Move::Execute()
{
}



void UAction_Move::Undo()
{
}

bool UAction_Move::CanExecute() const
{
	return false;
}

bool UAction_Move::Update(float _dt)
{
	actionCurrTime += _dt;
	return (actionCurrTime >= actionDuration);
}

UAction_Move *UAction_Move::Create(AActor *actionable
, FVector endPos, float duration)
{
	actionType = EActionType::Move;
	affectedObject = actionable;
	currPos_ = actionable->GetTransform().GetLocation();
	actionDuration = duration;
	endPos_ = endPos;
	return this;
}

EActionType UAction_Move::GetType()
{
	return actionType;
}
