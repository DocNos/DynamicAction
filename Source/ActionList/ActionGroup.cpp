// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionGroup.h"

void UActionGroup::AddAction(UAction* action)
{
	if(!action || !action->CanExecute() || !action->IsActive()) return;
	
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
		, action->GetType(), *GroupName_); // Dereference string? 
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


