// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionDirector.h"

void UActionDirector::Init()
{
	Super::Init();
	LogDebug("Director Initialized");
	builder_ = NewObject<UActionBuilder>();	

}

void UActionDirector::Shutdown()
{
	StopAllActions();
	ActiveActions_.Empty();
	QueuedActions_.Empty();
	Super::Shutdown();
}

void UActionDirector::Tick(float dt)
{
	//Super::Tick(dt);
	for (int32 i = ActiveActions_.Num() - 1; i >= 0; --i)
	{
		UAction* currAction = ActiveActions_[i];
		if (currAction && currAction->IsActive())
		{
			if (currAction->IsBlocking()) return;
			if (currAction->Update(dt))
			{
				currAction->SetActive(false);
				currAction->SetDeleteFlag(true);
				OnActionCompleted.Broadcast(currAction);
				LogDebug(FString::Printf
				(TEXT("Action of type %s completed. Index %d")
				 , *UEnum::GetDisplayValueAsText(currAction->GetType()).ToString(), i));
			}
		}
	}
	for (auto action : ActiveActions_)
	{
		if(action->DoDelete()) ActiveActions_.Remove(action);
	}
	for (auto action : QueuedActions_)
	{
		if (action->DoDelete()) QueuedActions_.Remove(action);
	}
	
	// Switch queued actions to active
	if (ActiveActions_.Num() == 0 && QueuedActions_.Num() > 0)
	{
		ProcessQueue();
	}
	OnDirectorTick.Broadcast(dt);
}

void UActionDirector::ProcessQueue()
{
	if (QueuedActions_.Num() > 0)
	{
		UAction* next = QueuedActions_[0];
		QueuedActions_.RemoveAt(0);
		ExecuteAction(next);

		if (QueuedActions_.Num() == 0)
		{
			OnSequenceCompleted.Broadcast(next);
			LogDebug("Sequence Complete");
		}
	}
	
}

void UActionDirector::ExecuteAction(UAction* Action)
{
	if (!Action || !Action->CanExecute())
	{
		LogDebug("Action cannot be executed");
		return;
	}
	
	Action->actionCurrTime_ = 0.f;
	Action->SetActive(true);
	Action->Execute();

	ActiveActions_.Add(Action);
	OnActionStarted.Broadcast(Action);
	LogDebug(FString::Printf(TEXT("Started execution of type %s: Duration: %.2f"),
							 *UEnum::GetDisplayValueAsText(Action->GetType()).ToString()
							,Action->actionDuration_));
}

void UActionDirector::QueueAction(UAction* action)
{
	if (!action || !action->CanExecute())
	{
		LogDebug("Action cannot be queued");
		return;
	}
	QueuedActions_.Add(action);
	LogDebug( FString::Printf( TEXT("Queued action of type: %s"),
							 *UEnum::GetDisplayValueAsText(action->GetType()).ToString()));
}

void UActionDirector::ExecuteSimultaneous(const TArray<UAction*>& Actions)
{
	int32 ExecutedCount = 0;

	for (UAction* Action : Actions)
	{
		if (Action && Action->CanExecute())
		{
			Action->actionCurrTime_ = 0.0f;
			Action->Execute();
			Action->SetActive(true);
			ActiveActions_.Add(Action);
			OnActionStarted.Broadcast(Action);
			ExecutedCount++;
		}
	}

	LogDebug(FString::Printf(TEXT("Started %d simultaneous actions"), ExecutedCount));
}

void UActionDirector::ExecuteSequence(const TArray<UAction*>& actions)
{
	if(actions.Num() == 0) return;
	if (actions[0])
	{
		ExecuteAction(actions[0]);
	}
	for (int32 i = 1; i < actions.Num(); ++i)
	{
		if (actions[i])
		{
			QueueAction(actions[i]);
		}
	}
	LogDebug(FString::Printf(TEXT("Started new sequence of %d actions"), actions.Num()));
}

void UActionDirector::StopAction(UAction* Action)
{
	if (!Action) return;

	Action->SetActive(false);
	Action->SetDeleteFlag(true);

	LogDebug(FString::Printf(TEXT("Stopped action: %s"),
							 *Action->GetClass()->GetName()));
}

void UActionDirector::StopAllActions()
{
	for (UAction* Action : ActiveActions_)
	{
		if (Action)
		{
			StopAction(Action);
		}
	}
	for (UAction* action : QueuedActions_)
	{
		if (action)
		{
			StopAction(action);
		}
	}

	//ActiveActions_.Empty();
	//QueuedActions.Empty();

	LogDebug("Stopped all actions");
}



bool UActionDirector::HasActiveActions() const
{
	for (UAction* action : ActiveActions_)
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
