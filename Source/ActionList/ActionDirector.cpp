// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionDirector.h"

void UActionDirector::Init()
{
	Super::Init();
	ActionHistory_.Reserve(MaxHistorySize);
	LogDebug("Director Initialized");
	

}

void UActionDirector::Shutdown()
{
	ClearHistory();
	Super::Shutdown();
}

void UActionDirector::Tick(float dt)
{
	//Super::Tick(dt);
	for (int32 i = ActionHistory_.Num() - 1; i >= 0; --i)
	{
		UAction* currAction = ActionHistory_[i];
		if (currAction && currAction->IsActive())
		{
			if (currAction->IsBlocking()) return;
			if (currAction->Update(dt))
			{
				currAction->SetActive(false);
				LogDebug(FString::Printf
				(TEXT("Action of type %s completed. Index %d")
				 , *UEnum::GetDisplayValueAsText(currAction->GetType()).ToString(), i));
			}
		}
	}
	OnDirectorTick.Broadcast(dt);
}

void UActionDirector::CompleteAllActive()
{
	// advance execution to end time. 
	
}

void UActionDirector::ExecuteAction(UAction* Action)
{
	if (!Action || !Action->CanExecute())
	{
		LogDebug("Action cannot be executed");
		return;
	}
	// Clear and stop execution for any actions after current index (Previously undone actions)
	if (CurrentIndex < ActionHistory_.Num() - 1)
	{
		// Stop any active actions we're about to remove
		for (int32 i = CurrentIndex + 1; i < ActionHistory_.Num(); ++i)
		{
			if (ActionHistory_[i]->IsActive())
			{
				ActionHistory_[i]->SetActive(false);
				// Could call Undo here to reset their state
			}
		}

		ActionHistory_.RemoveAt(CurrentIndex + 1,
								ActionHistory_.Num() - CurrentIndex - 1);
		LogDebug(FString::Printf(TEXT("Cleared %d future actions"),
								 ActionHistory_.Num() - CurrentIndex - 1));
	}
	Action->actionCurrTime = 0.f;
	Action->SetActive(true);
	
	ActionHistory_.Add(Action);
	++CurrentIndex;
	OnActionExecuted.Broadcast(Action);
	LogDebug(FString::Printf(TEXT("Started execution of type %s: Index %d, Duration: %.2f"),
							 *UEnum::GetDisplayValueAsText(Action->GetType()).ToString()
							,CurrentIndex, Action->actionDuration));
}

bool UActionDirector::Undo()
{
	if (!CanUndo())
	{
		LogDebug("Nothing to undo");
		return false;
	}
	UAction* currAction = ActionHistory_[CurrentIndex];
	if (currAction->IsActive())
	{
		currAction->SetActive(false);
	}
	currAction->Undo();
	--CurrentIndex;
	OnActionUndone.Broadcast(currAction);
	LogDebug(FString::Printf(TEXT("Undone action of type %s. Index now %d")
	, *UEnum::GetDisplayValueAsText(currAction->GetType()).ToString(), CurrentIndex));
	return true;
}

bool UActionDirector::Redo()
{
	if (!CanRedo())
	{
		LogDebug("Nothing to redo");
		return false;
	}
	++CurrentIndex;
	UAction* currAction = ActionHistory_[CurrentIndex];
	currAction->actionCurrTime = 0.f;
	currAction->Execute();
	currAction->SetActive(true);

	OnActionRedone.Broadcast(currAction);
	LogDebug(FString::Printf(TEXT("Redoing action of type %s at index %d. Duration: %.2f")
			, *UEnum::GetDisplayValueAsText(currAction->GetType()).ToString()
			, CurrentIndex, currAction->actionDuration));
	return true;
}

void UActionDirector::ClearHistory()
{
	ActionHistory_.Empty();
	CurrentIndex = -1;
	LogDebug("History Cleared");
}

bool UActionDirector::HasActiveActions() const
{
	for (UAction* action : ActionHistory_)
	{
		if(action->IsActive()) return true;
	}
	return false;
}

UActionDirector* UActionDirector::GetDirector(const UObject* WorldContextObject)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject,
														   EGetWorldErrorMode::LogAndReturnNull))
	{
		return Cast<UActionDirector>(World->GetGameInstance());
	}
	return nullptr;
}

void UActionDirector::SetDebugging(bool _doDebug)
{
	bDebugLogging_ = _doDebug;
	if (_doDebug)
	{
		UE_LOG(LogTemp, Log, TEXT("Global Debugging Enabled"));
	}else
		UE_LOG(LogTemp, Log, TEXT("Global Debugging Disabled"));
	
}

void UActionDirector::LogDebug(const FString& Message) const
{
	if (bDebugLogging_)
	{
		UE_LOG(LogTemp, Log, TEXT("ActionDirector: %s"), *Message);
	}
}

//UActionGroup* UActionDirector::CreateGroup(const FString& GroupName)
//{
//	if (GroupName.IsEmpty())
//	{
//		if (isDebugging()) { UE_LOG(LogTemp, Warning, TEXT("Empty Group Name")) };
//		return nullptr;
//	}	
//	if (ActionGroups.Contains(GroupName))
//	{
//		if (isDebugging()) { UE_LOG(LogTemp, Warning, TEXT("Group %s already exists"), *GroupName); }
//		return ActionGroups[GroupName];
//	}
//	UActionGroup* NewGroup = NewObject<UActionGroup>(this);
//	NewGroup->Initialize(GroupName);
//	ActionGroups.Add(GroupName, NewGroup);
//	OnGroupCreated.Broadcast(GroupName);
//
//	if(isDebugging()) { UE_LOG(LogTemp, Log, TEXT("Created new group: %s"), *GroupName); }
//	return NewGroup;
//}

//bool UActionDirector::RemoveGroup(const FString& GroupName)
//{
//	return false;
//}

//TArray<FString> UActionDirector::GetAllGroupNames() const
//{
//	return TArray<FString>();
//}
//
//void UActionDirector::ClearGroup(const FString& GroupName)
//{
//}
//
//void UActionDirector::ClearAllGroups()
//{
//}
