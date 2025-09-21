// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Action.h"
#include "Tickable.h"
#include "Engine/GameInstance.h"
#include "ActionDirector.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActionExecuted, UAction*, Action);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActionUndone, UAction*, Action);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActionRedone, UAction*, Action);



UCLASS(Blueprintable)
class ACTIONLIST_API UActionDirector : public UGameInstance, public FTickableGameObject
{
	GENERATED_BODY()
private:
	UPROPERTY()
	TArray<UAction*> ActionHistory_;

	// Current position in history (-1 means no history)
	int32 CurrentIndex = -1;

	UPROPERTY()
	int32 MaxHistorySize = 100;

	UPROPERTY()
	bool bDebugLogging_ = true;
	

public:
	// Overrides
	virtual void Init() override;
	virtual void Shutdown() override;

	virtual bool IsTickable() const {return true;}
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UActionDirector, STATGROUP_Tickables); }
	

	// Events
	UPROPERTY(BlueprintAssignable, Category = "Director|Events")
	FOnActionExecuted OnActionExecuted;

	UPROPERTY(BlueprintAssignable, Category = "Director|Events")
	FOnActionUndone OnActionUndone;

	UPROPERTY(BlueprintAssignable, Category = "Director|Events")
	FOnActionRedone OnActionRedone;

	// Core functionality	
	UFUNCTION(BlueprintCallable, Category = "Director|Core"
	, meta = (Tooltip = "Skip time execution") )
	void CompleteAllActive();

	UFUNCTION(BlueprintCallable, Category = "Director|Core",
			  meta = (ToolTip = "Execute an action and add it to history"))
	void ExecuteAction(UAction* Action);

	UFUNCTION(BlueprintCallable, Category = "Director|Core",
			  meta = (ToolTip = "Undo the last action"))
	bool Undo();

	UFUNCTION(BlueprintCallable, Category = "Director|Core",
			  meta = (ToolTip = "Redo the previously undone action"))
	bool Redo();

	// History management
	UFUNCTION(BlueprintCallable, Category = "Director|History",
			  meta = (ToolTip = "Clear all action history"))
	void ClearHistory();

	// Query functions
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Director|Query"
	, meta = (ToolTip = "Check for currently active actions"))
	bool HasActiveActions() const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Director|Query"
	, meta = (ToolTip = "Check if there is an action available to undo"))
	bool CanUndo() const { return CurrentIndex >= 0; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Director|Query"
	, meta = (ToolTip = "Redo a previously undone (future) action"))
	bool CanRedo() const { return CurrentIndex < ActionHistory_.Num() - 1; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Director|Query")
	int32 GetHistorySize() const { return ActionHistory_.Num(); }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Director|Query")
	int32 GetCurrentIndex() const { return CurrentIndex; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Director",
			  meta = (DisplayName = "Get Action Director", WorldContext = "WorldContextObject"))
	static UActionDirector* GetDirector
	(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Director")
	void SetDebugging(bool _doDebug);

	UFUNCTION(BlueprintCallable, Category = "Director")
	bool isDebugging() { return bDebugLogging_; }

	

protected:
	void LogDebug(const FString& Message) const;

};
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



