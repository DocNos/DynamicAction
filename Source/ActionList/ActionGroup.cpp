// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionGroup.h"
#include "ActionDirector.h"

void UActionGroup::Initialize(const FString& _groupName)
{
	GroupName_ = _groupName;
	History_.Empty();
	currIndex_ = -1;
	bIsActive_ = true;
}

void UActionGroup::AddAction(UAction* action)
{
	if(!action || !action->CanExecute() || !action->IsActive() || !bIsActive_) return;
	
	// Clear redo history
	// if (CurrentIndex < CommandHistory.Num() - 1)
	// {
	// 	CommandHistory.RemoveAt(CurrentIndex + 1,
	// 							CommandHistory.Num() - CurrentIndex - 1);
	// }

	action->Execute();
	History_.Add(action);
	++currIndex_;
	if (UActionDirector::GetDirector(this)->isDebugging())
	{
		UE_LOG(LogTemp, Log, TEXT("Action of type %s add to %s")
		, *UEnum::GetDisplayValueAsText(action->GetType()).ToString(), *GroupName_); // Dereference string? 
	}
}

bool UActionGroup::Undo()
{
	return true;
}



bool UActionGroup::Redo()
{
	return true;
}

void UActionGroup::Clear()
{
	History_.Empty();
	currIndex_ = -1;
}

bool UActionGroup::CanRedo() const
{
	return (currIndex_ < (History_.Num() - 1)) && bIsActive_; 
}
