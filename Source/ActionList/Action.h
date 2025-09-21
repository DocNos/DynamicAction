// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Action.generated.h"


UENUM(BlueprintType)
enum class EActionType : uint8
{
	DEFAULT	UMETA(DisplayName = "DEFAULT")
	, Move	UMETA(DisplayName = "Move")
};
/**
 * 
 */
UCLASS(Abstract, Blueprintable) // Abstract base class.
class ACTIONLIST_API UAction : public UObject
{
	GENERATED_BODY()
private:
	UPROPERTY()
	bool bActionActive;

	UPROPERTY()
	EActionType actionType = EActionType::DEFAULT;

	UPROPERTY()
	bool bIsBlocking_ = false;

public:
	UPROPERTY(BlueprintReadWrite)
	float actionDuration;
	
	UPROPERTY(BlueprintReadWrite)
	AActor *affectedObject;

	UPROPERTY(BlueprintReadWrite)
	float actionCurrTime;	

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Action")
	virtual bool IsBlocking(){ return bIsBlocking_; }

	UFUNCTION(BlueprintCallable)
	virtual EActionType GetType() { return actionType; }

	UFUNCTION(BlueprintCallable, Category = "Action")
	virtual void Execute() PURE_VIRTUAL(UAction::Execute, );

	UFUNCTION(BlueprintCallable, Category = "Action")
	virtual void Undo() PURE_VIRTUAL(UAction::Undo, );

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Action")
	virtual bool CanExecute() const { return true; }
	
	UFUNCTION(Blueprintable, Category = "Action")
	virtual bool Update(float _dt) {return true;}

	UFUNCTION(BlueprintCallable, Category = "Action")
	virtual bool IsActive() const {return bActionActive;}

	UFUNCTION(BlueprintCallable, Category = "Action")
	virtual void SetActive(bool _active) { bActionActive = _active; }

	

};
