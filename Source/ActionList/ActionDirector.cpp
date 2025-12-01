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
	Super::Shutdown();
}

void UActionDirector::Tick(float dt)
{	
	if(!DirectorClass) return;
	ProcessQueue();
	ProcessActive(dt);
	OnDirectorTick.Broadcast(dt);
}

void UActionDirector::ProcessActive(float dt)
{
	for (int32 i = 0; i < ActiveActions_.Num(); ++i)
	{
		UAction* currAction = ActiveActions_[i];
		if (currAction && currAction->IsActive())
		{
			//if (currAction->IsBlocking()) return;
			if (currAction->Update(dt))
			{
				currAction->SetDone(true);
				currAction->SetDeleteFlag(true);
				DeleteMap_.Add(currAction, i);
				//currAction->SetActive(false);
				OnActionCompleted.Broadcast(currAction);
			}
		}
	}
}

void UActionDirector::ProcessQueue()
{

	TArray<FSequence> CompleteSequences;

	for (int32 currID = 0; currID < Sequences_.Num(); ++currID)
	{
		FSequence& currSeq = Sequences_[currID];
		if (currSeq.currActive >= currSeq.SequenceData.Num())
		{
			currSeq.bIsDone = true;
			CompleteSequences.Add(currSeq);
			OnSequenceCompleted.Broadcast(currSeq);
			continue;
		}
		if(currSeq.bIsDone) continue;
		switch (currSeq.type)
		{
			case(SeqType::Sequential): ProcessSequence_Sequential(currSeq); break;
			case(SeqType::Simultaneous): 
			{
				ProcessSequence_Simultaneous(currSeq); 
				CompleteSequences.Add(currSeq);
			}break;
			case(SeqType::Blocking): ProcessSequence_Blocking(currSeq); break;
			default: break;
		}
		
	}
	
	for (auto seq : CompleteSequences)
	{
		int index = Sequences_.Find(seq);
		Sequences_.RemoveAt(index);
	}
	activeSequences_ = Sequences_.Num();
	
}

void UActionDirector::ProcessSequence_Sequential(FSequence& currSeq)
{
	// Current action of current sequence
	UAction* currAction = currSeq.SequenceData[currSeq.currActive];
	if (!currAction) { ++currSeq.currActive; return; }

	if (!currAction->IsActive() && !currAction->DoDelete())
	{
		ExecuteAction(currAction);
	}
	if (currAction->IsDone()) { ++currSeq.currActive; }

}

void UActionDirector::ProcessSequence_Simultaneous(FSequence& sequence)
{
	for (auto action : sequence.SequenceData)
	{
		ExecuteAction(action);
		++sequence.currActive;
	}

}

void UActionDirector::ProcessSequence_Blocking(FSequence& currSeq)
{
	UAction* currAction = currSeq.SequenceData[currSeq.currActive];
	

}

void UActionDirector::NewSequence
(int indexOwner, TArray<UAction*> newSequence, SeqType type, FString _sequenceName)
{
	for (auto action : newSequence)
	{
		action->bIsSequence_ = true;
	}
	FSequence newSeq = {type, newSequence, 0, false, indexOwner, _sequenceName};
	Sequences_.Add(newSeq);
	++activeSequences_;
	//return newSeq;
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
	OnActionStarted.Broadcast(Action);
	Action->Execute();

	ActiveActions_.Add(Action);
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
		action->SetActive(false);
		LogDebug_Red(FString::Printf(TEXT("Finished action") ) ); // %s"),
						//		 action->GetOuter()->GetFName()));
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
