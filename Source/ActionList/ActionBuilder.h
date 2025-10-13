// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
// #include "Action.h"
#include "Action_Move.h"
#include "Action_Fade.h"
#include "Action_Rotate.h"
#include "Action_Flip.h"
#include "Action_Delay.h"
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
	UAction* CreateAction(EActionType type, AActor* affectedObject, float duration);

	UFUNCTION(BlueprintCallable, Category="Action Builder")
	UAction_Move* CreateMoveAction(AActor* affectedObject, FVector endPos, float duration);

	UFUNCTION(BlueprintCallable, Category="Action Builder")
	UAction_Fade* CreateFadeAction(AActor* affectedObject, float startOpacity, float endOpacity,
	float duration);

	UFUNCTION(BlueprintCallable, Category="Action Builder")
	UAction_Rotate* CreateRotateAction(AActor* affectedObject, FRotator startRotation,
	FRotator endRotation, float duration);

	UFUNCTION(BlueprintCallable, Category="Action Builder")
	UAction_Flip* CreateFlipAction(AActor* affectedObject, float duration);

	UFUNCTION(BlueprintCallable, Category= "Action Builder")
	UAction_Delay* CreateDelayAction(AActor* affectedObject, float preDelay, float duration);

	UFUNCTION(BlueprintCallable, Category="Action Builder")
	void SetMoveClass(TSubclassOf<UAction_Move> moveClass) { MoveActionClass = moveClass;}

	UPROPERTY(EditAnywhere, Category = "Action Builder")
	TSubclassOf<UAction_Move> MoveActionClass;

	UFUNCTION(BlueprintCallable, Category = "Action Builder")
	void SetFadeClass(TSubclassOf<UAction_Fade> fadeClass) { FadeActionClass = fadeClass; }

	UPROPERTY(EditAnywhere, Category = "Action Builder")
	TSubclassOf<UAction_Fade> FadeActionClass;

	UFUNCTION(BlueprintCallable, Category = "Action Builder")
	void SetRotateClass(TSubclassOf<UAction_Rotate> rotateClass) { RotateActionClass = rotateClass; }

	UPROPERTY(EditAnywhere, Category = "Action Builder")
	TSubclassOf<UAction_Rotate> RotateActionClass;
	
	UFUNCTION(BlueprintCallable, Category = "Action Builder")
	void SetFlipClass(TSubclassOf<UAction_Flip> flipClass) { FlipActionClass = flipClass; }

	UPROPERTY(EditAnywhere, Category = "Action Builder")
	TSubclassOf<UAction_Flip> FlipActionClass;

	UFUNCTION(BlueprintCallable, Category="Action Builder")
	void SetDelayClass(TSubclassOf<UAction_Delay> delayClass) { DelayActionClass = delayClass; }

	UPROPERTY(EditAnywhere, Category= "Action Builder")
	TSubclassOf<UAction_Delay> DelayActionClass;

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
