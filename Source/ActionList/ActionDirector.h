// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
//#include "GM_DynamicAction.h"
#include "ActionDirector.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGroupCreated, const FString&, GroupName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGroupRemoved, const FString&, GroupName);




UCLASS(Blueprintable)
class ACTIONLIST_API UActionDirector : public UGameInstance
{
	GENERATED_BODY()
private:
	UPROPERTY()
	TMap<FString, UActionGroup*> ActionGroups;

	UPROPERTY()
	TArray<FString> GroupExecutionOrder;  // Tracks order of last execution

	UPROPERTY()
	int32 MaxHistorySize = 100;

	UPROPERTY()
	FString DefaultGroupName = "Default";

	

public:
	virtual void Init() override;
	virtual void Shutdown() override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Director",
			  meta = (DisplayName = "Get Action Director", WorldContext = "WorldContextObject"))
	static UActionDirector* GetDirector
	(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Director")
	void SetDebugging(bool _doDebug);

	UFUNCTION(BlueprintCallable, Category = "Director")
	bool isDebugging() { return bDebugLogging_; }

	UPROPERTY(BlueprintReadOnly)
	bool bDebugLogging_ = true;

	// Events
	UPROPERTY(BlueprintAssignable, Category = "Director")
	FOnGroupCreated OnGroupCreated;

	UPROPERTY(BlueprintAssignable, Category = "Director")
	FOnGroupRemoved OnGroupRemoved;

	// Groups
	UFUNCTION(BlueprintCallable, Category = "Director")
	UActionGroup* CreateGroup(const FString& GroupName);

	UFUNCTION(BlueprintCallable, Category = "Director")
	bool RemoveGroup(const FString& GroupName);

	//UFUNCTION(BlueprintCallable, Category = "Director")
	//UActionGroup* GetOrCreateGroup(const FString& GroupName);

	//UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Director")
	//bool HasGroup(const FString& GroupName) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Director")
	TArray<FString> GetAllGroupNames() const;

	UFUNCTION(BlueprintCallable, Category = "Action Director")
	void ClearGroup(const FString& GroupName);

	UFUNCTION(BlueprintCallable, Category = "Action Director")
	void ClearAllGroups();
	
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


};
