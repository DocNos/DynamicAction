// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionDirector.h"

void UActionDirector::Init()
{
	Super::Init();

	CreateGroup(DefaultGroupName);
	if (isDebugging())
	{
		UE_LOG(LogTemp, Warning, TEXT("Director initialized with default group"));
	}

}

void UActionDirector::Shutdown()
{
	ClearAllGroups();
	ActionGroups.Empty();
	Super::Shutdown();
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
	FString enableText = (bDebugLogging_) ? ("Enabled") : ("Disabled");
	UE_LOG(LogTemp, Log, TEXT("Global Debugging %s"), enableText);
}

UActionGroup* UActionDirector::CreateGroup(const FString& GroupName)
{
	if (GroupName.IsEmpty())
	{
		if (isDebugging()) { UE_LOG(LogTemp, Warning, TEXT("Empty Group Name")) };
		return nullptr;
	}	
	if (ActionGroups.Contains(GroupName))
	{
		if (isDebugging()) { UE_LOG(LogTemp, Warning, TEXT("Group %s already exists"), *GroupName); }
		return ActionGroups[GroupName];
	}
	UActionGroup* NewGroup = NewObject<UActionGroup>(this);
	NewGroup->Initialize(GroupName);
	ActionGroups.Add(GroupName, NewGroup);
	OnGroupCreated.Broadcast(GroupName);

	if(isDebugging()) { UE_LOG(LogTemp, Log, TEXT("Created new group: %s"), *GroupName); }
	return NewGroup;
}

bool UActionDirector::RemoveGroup(const FString& GroupName)
{
	return false;
}

TArray<FString> UActionDirector::GetAllGroupNames() const
{
	return TArray<FString>();
}

void UActionDirector::ClearGroup(const FString& GroupName)
{
}

void UActionDirector::ClearAllGroups()
{
}
