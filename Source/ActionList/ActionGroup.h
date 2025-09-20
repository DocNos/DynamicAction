// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GM_DynamicAction.h"
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
	void Initialize(const FString& InGroupName);

	UFUNCTION(BlueprintCallable, Category = "Grouping")
	void AddAction(UAction* action);

	UFUNCTION(BlueprintCallable, Category = "Grouping")
	bool Undo();

	UFUNCTION(BlueprintCallable, Category = "Grouping")
	bool Redo();

	UFUNCTION(BlueprintCallable, Category = "Grouping")
	void Clear();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Grouping")
	FString GetGroupName() const { return GroupName_; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Grouping")
	int32 GetHistoryCount() const { return History_.Num(); }

};
