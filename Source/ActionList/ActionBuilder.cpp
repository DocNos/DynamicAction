// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionBuilder.h"

// Sets default values for this component's properties
UActionBuilder::UActionBuilder()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UActionBuilder::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UActionBuilder::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

//UAction* UActionBuilder::CreateAction(EActionType type)
//{
//	switch (type)
//	{
//		case(EActionType::Move):
//		{
//			UAction_Move* newMove = NewObject<UAction_Move>();
//			return newMove;
//		}
//		default: return nullptr;
//	}
//}

UAction_Move* UActionBuilder::CreateMoveAction(AActor* affectedObject, FVector endPos, float duration)
{
	UAction_Move* newMove = NewObject<UAction_Move>(this);
	newMove->Initialize(affectedObject, endPos, duration);

	return newMove;
}
