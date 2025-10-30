// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Action.h"
#include "Tickable.h"
#include "ActionBuilder.h"
#include "Engine/GameInstance.h"
#include "ActionDirector.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActionStarted, UAction*, Action);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActionCompleted, UAction*, Action);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSequenceCompleted, UAction*, Action);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDirectorTick, float, deltaTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpawnCard, UAction_SpawnCard*, SpawnAction);


USTRUCT(BlueprintType)
struct FSequence
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "2D Sequence Array")
	TArray<UAction*> SequenceData;

};

UCLASS(Blueprintable)
class ACTIONLIST_API UActionDirector : public UGameInstance, public FTickableGameObject
{
	GENERATED_BODY()
private:
	//UPROPERTY()
	//TArray<UAction*> ActionHistory_;
	UPROPERTY()
	UActionBuilder* builder_;

	UPROPERTY()
	TArray<UAction*> ActiveActions_;

	UPROPERTY()
	TMap<UAction*, int> DeleteMap_;

	UPROPERTY()
	TArray<UAction*> DeleteList_; 

	UPROPERTY()
	bool bDebugLogging_ = true;
	

public:
	// Events
	UPROPERTY(BlueprintAssignable, Category = "Director|Events")
	FOnDirectorTick OnDirectorTick;

	UPROPERTY(BlueprintAssignable, Category = "Director|Events")
	FOnActionStarted OnActionStarted;

	UPROPERTY(BlueprintAssignable, Category = "Director|Events")
	FOnActionCompleted OnActionCompleted;

	UPROPERTY(BlueprintAssignable, Category = "Director|Events")
	FOnSequenceCompleted OnSequenceCompleted;

	UPROPERTY(BlueprintAssignable, Category = "Director|Events")
	FOnSpawnCard OnSpawnCard;

public:	
	// Overrides ---------------------------------------------------------
	UFUNCTION(BlueprintCallable, Category = "Director")
	virtual void Init() override;
	virtual void Shutdown() override;
	virtual bool IsTickable() const {return true;}
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UActionDirector, STATGROUP_Tickables); }

	UFUNCTION(BlueprintCallable, Category = "Director")
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable, BlueprintPure
	, Category = "Gettor")
	UActionBuilder* 
	GetBuilder() { return builder_;}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Director",
			  meta = (DisplayName = "Get Action Director", WorldContext = "WorldContextObject"))
	static UActionDirector* 
	GetDirector(const UObject* WorldContextObject)	
	{
		if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject,
															   EGetWorldErrorMode::LogAndReturnNull))
		{
			return Cast<UActionDirector>(World->GetGameInstance());
		}
		return nullptr;
	}

	UFUNCTION(BlueprintCallable, Category = "Director")
	void SetDebugging(bool _doDebug)
	{
		bDebugLogging_ = _doDebug;
		if (_doDebug)
		{
			UE_LOG(LogTemp, Log, TEXT("Global Debugging Enabled"));
		}
		else
			UE_LOG(LogTemp, Log, TEXT("Global Debugging Disabled"));

	}

	

	// Core functionality ---------------------------------------------------
	UFUNCTION(BlueprintCallable, Category = "Director|Actions",
			  meta = (ToolTip = "Execute an action immediately"))
	void ExecuteAction(UAction* Action);	

	// Action Control ------------------------------------------------------
	UFUNCTION(BlueprintCallable, Category = "Director|Control"
			, meta = (ToolTip = "Stop an action and remove it from the sequence"))
	void StopAction(UAction* Action);

	UFUNCTION(BlueprintCallable, Category = "Director|Control"
			, meta = (ToolTip = "Stop all actions and remove them from sequence"))
	void StopAllActions();

	UFUNCTION(BlueprintCallable, Category="Director|Control")
	void RemoveActive(UAction* action);

	
	// Sequencing ---------------------------------------------------------
	UFUNCTION(BlueprintCallable)
	TArray<UAction*> NewSequence(int indexOwner, FSequence newSequence);

	UPROPERTY(BlueprintReadWrite)
	TMap<int, FSequence> Sequences_;

	UPROPERTY(BlueprintReadWrite)
	int activeSequences_ = 0;


	// Query functions ----------------------------------------------------
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Director|Query")
	TArray<UAction*> GetDeletes() {return DeleteList_;}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Director|Query")
	TArray<UAction*> GetActiveActions() {return ActiveActions_;}	

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Director|Query"
	, meta = (ToolTip = "Check for currently active actions"))
	bool HasActiveActions() const
	{
		for (UAction* action : ActiveActions_)
		{
			if (action->IsActive()) return true;
		}
		return false;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Director|Actions")
	bool IsObjectBlocked(AActor* object) const;	

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Director")
	bool isDebugging() const { return bDebugLogging_; }

protected:
	void LogDebug(const FString& Message) const
	{
		if (bDebugLogging_)
		{
			UE_LOG(LogTemp, Error, TEXT("ActionDirector: %s"), *Message);
		}
	}
	void ProcessQueue();

};
	//UPROPERTY()
	//TArray<UAction*> QueuedActions_;

	//UFUNCTION(BlueprintCallable, Category = "Director|Actions"
	//		  , meta = (ToolTip = "Execute a series simultaneously"))
	//void ExecuteSimultaneous(const TArray<UAction*>& actions);
	//
	//UFUNCTION(BlueprintCallable, Category = "Director|Actions"
	//		  , meta = (ToolTip = "Create a new sequence of actions"))
	//void ExecuteSequence(const TArray<UAction*>& sequence);


	//UFUNCTION(BlueprintCallable, Category = "Director|Actions",
	//		  meta = (ToolTip = "Add an action to execute after current queue"))
	//void QueueAction(UAction* action);
	// Sequence control - TODO grouping

	//UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Director|Query")
	//TArray<UAction*> GetQueuedActions() { return QueuedActions_; }

	//UFUNCTION(BlueprintCallable, Category = "Director|Control"
	//		, meta = (ToolTip = "Pause a specific action for specified time"))
	//void PauseAction(UAction* action, float pauseTime);

	//UFUNCTION(BlueprintCallable, Category = "Director|Control")
	//void ResumeAllActions();

	//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGroupCreated, const FString&, GroupName);
	//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGroupRemoved, const FString&, GroupName);
	
	//UPROPERTY()
	//TMap<FString, UActionGroup*> ActionGroups;

	//UPROPERTY()
	//TArray<FString> GroupExecutionOrder;  // Tracks order of last execution

	//UPROPERTY()
	//FString DefaultGroupName = "Default";

	// Events
	//UPROPERTY(BlueprintAssignable, Category = "Director")
	//FOnGroupCreated OnGroupCreated;
	//
	//UPROPERTY(BlueprintAssignable, Category = "Director")
	//FOnGroupRemoved OnGroupRemoved;
	//
	//// Groups
	//UFUNCTION(BlueprintCallable, Category = "Director")
	//UActionGroup* CreateGroup(const FString& GroupName);
	//
	//UFUNCTION(BlueprintCallable, Category = "Director")
	//bool RemoveGroup(const FString& GroupName);

	//UFUNCTION(BlueprintCallable, Category = "Director")
	//UActionGroup* GetOrCreateGroup(const FString& GroupName);

	//UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Director")
	//bool HasGroup(const FString& GroupName) const;

	//UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Director")
	//TArray<FString> GetAllGroupNames() const;
	//
	//UFUNCTION(BlueprintCallable, Category = "Action Director")
	//void ClearGroup(const FString& GroupName);
	//
	//UFUNCTION(BlueprintCallable, Category = "Action Director")
	//void ClearAllGroups();
	
	/* Command Execution
	UFUNCTION(BlueprintCallable, Category = "Action Director", meta = (CallInEditor = "true"))
	void ExecuteCommand(UActionCommand* Command, const FString& GroupName = "Default");

	UFUNCTION(BlueprintCallable, Category = "Action Director")
	void ExecuteCommandInMultipleGroups(UActionCommand* Command, const TArray<FString>& GroupNames);

	// Group-specific undo/redo
	UFUNCTION(BlueprintCallable, Category = "Action Director")
	bool UndoGroup(const FString& GroupName);

	UFUNCTION(BlueprintCallable, Category = "Action Director")
	bool RedoGroup(const FString& GroupName);

	// Global undo/redo
	UFUNCTION(BlueprintCallable, Category = "Action Director")
	bool UndoLastModified();

	UFUNCTION(BlueprintCallable, Category = "Action Director")
	bool RedoLastModified();

	// Batch operations
	UFUNCTION(BlueprintCallable, Category = "Action Director")
	void UndoGroups(const TArray<FString>& GroupNames);

	UFUNCTION(BlueprintCallable, Category = "Action Director")
	void RedoGroups(const TArray<FString>& GroupNames);

	UFUNCTION(BlueprintCallable, Category = "Action Director")
	void UndoAllGroups();

	UFUNCTION(BlueprintCallable, Category = "Action Director")
	void RedoAllGroups();

	// Group management
	UFUNCTION(BlueprintCallable, Category = "Action Director")
	void SetGroupActive(const FString& GroupName, bool bActive);

	

	UFUNCTION(BlueprintCallable, Category = "Action Director")
	bool MergeGroups(const FString& SourceGroup, const FString& TargetGroup);

	// Query functions
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Action Director")
	bool CanUndoGroup(const FString& GroupName) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Action Director")
	bool CanRedoGroup(const FString& GroupName) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Action Director")
	UActionGroup* GetGroup(const FString& GroupName) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Action Director")
	int32 GetGroupCount() const { return ActionGroups.Num(); }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Action Director")
	int32 GetGroupHistorySize(const FString& GroupName) const;

	

private:
	void UpdateExecutionOrder(const FString& GroupName);
	FString GetLastModifiedGroup() const; */



