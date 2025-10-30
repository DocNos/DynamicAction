// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionDirector.h"

void UActionDirector::Init()
{	
	builder_ = NewObject<UActionBuilder>();	
	Super::Init();
	LogDebug("Director Initialized");

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
			//if (currAction->IsBlocking()) return;
			if (currAction->Update(dt))
			{
				currAction->SetDeleteFlag(true);
				DeleteMap_.Add(currAction, i);
				if (!currAction->bIsSequence_)
				{
					currAction->SetActive(false);					
					//DeleteList_.Add(currAction);
					OnActionCompleted.Broadcast(currAction);
					LogDebug(FString::Printf
					(TEXT("Action of type %s completed. Index %d")
					 , *UEnum::GetDisplayValueAsText(currAction->GetType()).ToString(), i));
				}
				
			}
		}
	}
	//ProcessDelete();
	ProcessQueue();
	OnDirectorTick.Broadcast(dt);
}

TArray<UAction*> UActionDirector::NewSequence(int indexOwner, FSequence newSequence)
{
	for (auto action : newSequence.SequenceData)
	{
		action->bIsSequence_ = true;
	}
	Sequences_[indexOwner] = newSequence;
	++activeSequences_;
	return newSequence.SequenceData;
}

void UActionDirector::ProcessQueue()
{
	if (activeSequences_ > 0)
	{
		for (int i = 0; i < activeSequences_; ++i)
		{
			int len = Sequences_[i].SequenceData.Num();
			if (len > 0)
			{
				TArray<UAction*> seq_ = Sequences_[i].SequenceData;
				UAction* curr = seq_[0];
				ExecuteAction(curr);
				// need to move index once execute first item
			}
			else
			{
				// delete sequence
			}
			
			
		}
		
	}
	
}

void UActionDirector::ExecuteAction(UAction* Action)
{
	if (!Action) //|| !Action->CanExecute())
	{
		LogDebug("Action cannot be executed");
		return;
	}
	if (IsObjectBlocked(Action->affectedObject_) && !Action->IsBlocking())
	{
		LogDebug(FString::Printf(TEXT("%s blocked, queuing action of type %s") 
				, Action->affectedObject_, 
				*UEnum::GetDisplayValueAsText(Action->GetType()).ToString()));
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

void UActionDirector::RemoveActive(UAction* action)
{
	//if(DeleteList_.IsEmpty()) return;
	if (action->DoDelete())
	{
		ActiveActions_.RemoveAt(DeleteMap_[action]);
	}
	
}

bool UActionDirector::IsObjectBlocked(AActor* object) const
{
	for (UAction* Action : ActiveActions_)
	{
		if (Action && Action->IsActive() && Action->IsBlocking()
			&& Action->affectedObject_ == object)
		{
			return true;
		}
	}
	return false;
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
			Action->SetDeleteFlag(true);
		}
		
	}
	LogDebug("Stopped all actions");
}

// void UActionDirector::ExecuteSimultaneous(const TArray<UAction*>& Actions)
// {
// 	int32 ExecutedCount = 0;
// 
// 	for (UAction* Action : Actions)
// 	{
// 		if (Action && !Action->IsBlocking())
// 		{
// 			Action->actionCurrTime_ = 0.0f;
// 			Action->Execute();
// 			Action->SetActive(true);
// 			ActiveActions_.Add(Action);
// 			OnActionStarted.Broadcast(Action);
// 			ExecutedCount++;
// 		}
// 	}
// 
// 	LogDebug(FString::Printf(TEXT("Started %d simultaneous actions"), ExecutedCount));
// }


//void UActionDirector::ExecuteSequence(const TArray<UAction*>& actions)
//{
//	if(actions.Num() == 0) return;
//	if (actions[0])
//	{
//		ExecuteAction(actions[0]);
//	}
//	for (int32 i = 1; i < actions.Num(); ++i)
//	{
//		if (actions[i])
//		{
//			QueueAction(actions[i]);
//		}
//	}
//	LogDebug(FString::Printf(TEXT("Started new sequence of %d actions"), actions.Num()));
//}

//void UActionDirector::QueueAction(UAction* action)
//{
//	if (!action)
//	{
//		LogDebug("Action cannot be queued");
//		return;
//	}
//	QueuedActions_.Add(action);
//	LogDebug( FString::Printf( TEXT("Queued action of type: %s"),
//							 *UEnum::GetDisplayValueAsText(action->GetType()).ToString()));
//}


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
