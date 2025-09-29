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
	TArray<UAction*> QueuedActions_;

	UPROPERTY()
	bool bDebugLogging_ = true;
	

public:
	// Overrides
	UFUNCTION(BlueprintCallable, Category = "Director")
	virtual void Init() override;
	virtual void Shutdown() override;

	virtual bool IsTickable() const {return true;}
	UFUNCTION(BlueprintCallable, Category = "Director")
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UActionDirector, STATGROUP_Tickables); }
	
	UFUNCTION(BlueprintCallable, BlueprintPure
	, Category = "Gettor")
	UActionBuilder* getBuilder() { return builder_;}

	// Events
	UPROPERTY(BlueprintAssignable, Category = "Director|Events")
	FOnDirectorTick OnDirectorTick;

	UPROPERTY(BlueprintAssignable, Category = "Director|Events")
	FOnActionStarted OnActionStarted;

	UPROPERTY(BlueprintAssignable, Category = "Director|Events")
	FOnActionCompleted OnActionCompleted;

	UPROPERTY(BlueprintAssignable, Category = "Director|Events")
	FOnSequenceCompleted OnSequenceCompleted;

	// Core functionality	
	UFUNCTION(BlueprintCallable, Category = "Director|Actions",
			  meta = (ToolTip = "Execute an action immediately"))
	void ExecuteAction(UAction* Action);

	UFUNCTION(BlueprintCallable, Category = "Director|Actions",
			  meta = (ToolTip = "Add an action to execute after current queue"))
	void QueueAction(UAction* action);

	UFUNCTION(BlueprintCallable, Category = "Director|Actions"
			, meta = (ToolTip = "Execute a series simultaneously"))
	void ExecuteSimultaneous(const TArray<UAction*>& actions);

	UFUNCTION(BlueprintCallable, Category = "Director|Actions"
			, meta = (ToolTip = "Create a new sequence of actions"))
	void ExecuteSequence(const TArray<UAction*>& sequence);

	// Sequence control - TODO grouping
	UFUNCTION(BlueprintCallable, Category = "Director|Control"
			, meta = (ToolTip = "Stop an action and remove it from the sequence"))
	void StopAction(UAction* Action);

	UFUNCTION(BlueprintCallable, Category = "Director|Control"
			, meta = (ToolTip = "Stop all actions and remove them from sequence"))
	void StopAllActions();

	// Action Creation
	// UFUNCTION(BlueprintCallable, Category = "Director|Actions")
	// UAction* CreateAction(EActionType type);


	// Query functions
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Director|Query")
	TArray<UAction*> GetActiveActions() {return ActiveActions_;}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Director|Query"
	, meta = (ToolTip = "Check for currently active actions"))
	bool HasActiveActions() const;
	

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Director",
			  meta = (DisplayName = "Get Action Director", WorldContext = "WorldContextObject"))
	static UActionDirector* GetDirector
	(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Director")
	void SetDebugging(bool _doDebug);

	UFUNCTION(BlueprintCallable, Category = "Director")
	bool isDebugging() const { return bDebugLogging_; }

	

protected:
	void LogDebug(const FString& Message) const;
	void ProcessQueue();

};
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



