// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
// #include "Action.h"
#include "Action_Move.h"
#include "Components/ActorComponent.h"
#include "ActionBuilder.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONLIST_API UActionBuilder : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UActionBuilder();

	//UFUNCTION(BlueprintCallable, Category="Action Builder")
	//UAction* CreateAction(EActionType type);

	UFUNCTION(BlueprintCallable, Category="Action Builder")
	UAction_Move* CreateMoveAction(AActor* affectedObject, FVector endPos, float duration);

	UPROPERTY(EditAnywhere, Category = "Action Builder")
	TSubclassOf<UAction> ActionClass;

	UPROPERTY(EditAnywhere, Category = "Action Builder")
	float DefaultDuration = 1.0f;

	//UPROPERTY(EditAnywhere, Category = "Action Builder")
	//TArray<FVector> Waypoints;

	//UFUNCTION(CallInEditor, Category = "Action Builder")
	//void TestAction();

	//UFUNCTION(CallInEditor, Category = "Action Builder")
	//void AddToDirector();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
