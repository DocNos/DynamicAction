// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Action.h"
#include "UObject/NoExportTypes.h"
#include "ActionGroup.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class ACTIONLIST_API UActionGroup : public UObject
{
	GENERATED_BODY()
private:
	UPROPERTY()
	FString GroupName_;
	
	UPROPERTY()
	TArray<UAction*> History_;

	int32 currIndex_ = -1;

	UPROPERTY()
	bool bIsActive_ = true;

public:
	UFUNCTION(BlueprintCallable, Category = "Grouping"
	, meta = (Tooltip="Initialize this group"))
	void Initialize(const FString& _groupName);

	UFUNCTION(BlueprintCallable, Category = "Grouping"
	, meta = (Tooltip = "Add existing action to the group"))
	void AddAction(UAction* action);

	UFUNCTION(BlueprintCallable, Category = "Grouping")
	bool Undo();

	UFUNCTION(BlueprintCallable, Category = "Grouping")
	bool Redo();

	UFUNCTION(BlueprintCallable, Category = "Grouping")
	void Clear();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Grouping")
	bool CanUndo() const { return currIndex_ >= 0 && bIsActive_; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Action Group")
	bool CanRedo() const;

	UFUNCTION(BlueprintCallable, Category = "Action Group")
	void SetActive(bool _bIsActive) { bIsActive_ = _bIsActive; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Grouping")
	FString GetGroupName() const { return GroupName_; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Grouping")
	int32 GetHistoryCount() const { return History_.Num(); }

};
