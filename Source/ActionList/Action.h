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
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActionUpdate, float, deltaTime);


UCLASS(Abstract, Blueprintable) // Abstract base class.
class ACTIONLIST_API UAction : public UObject
{
	GENERATED_BODY()
private:
	UPROPERTY()
	bool bActionActive_ = false;

	UPROPERTY()
	EActionType actionType_ = EActionType::DEFAULT;

	UPROPERTY()
	bool bIsBlocking_ = false;

	UPROPERTY()
	bool bDoDelete_ = false;

public:
		
	UPROPERTY(BlueprintAssignable)
	FOnActionUpdate OnActionUpdate;
	
	UPROPERTY(BlueprintReadWrite)
	float actionDuration_;
	
	UPROPERTY(BlueprintReadWrite)
	AActor *affectedObject_;

	UPROPERTY(BlueprintReadWrite)
	float actionCurrTime_;	

	



	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Action")
	virtual bool IsBlocking(){ return bIsBlocking_; }

	UFUNCTION(BlueprintCallable)
	virtual EActionType GetType() { return actionType_; }

	UFUNCTION(BlueprintCallable, Category = "Action")
	virtual void Execute() PURE_VIRTUAL(UAction::Execute, );

	UFUNCTION(BlueprintCallable, Category = "Action")
	virtual void Undo() PURE_VIRTUAL(UAction::Undo, );

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Action")
	virtual bool CanExecute() const { return true; }
	
	UFUNCTION(Blueprintable, Category = "Action")
	virtual bool Update(float _dt) {return true;}

	UFUNCTION(Blueprintable, Category = "Action")
	virtual bool DoDelete() {return bDoDelete_; }

	UFUNCTION(Blueprintable, Category = "Action")
	virtual void SetDeleteFlag(bool flag) { bDoDelete_ = flag; }

	UFUNCTION(BlueprintCallable, Category = "Action")
	virtual bool IsActive() const {return bActionActive_;}

	UFUNCTION(BlueprintCallable, Category = "Action")
	virtual void SetActive(bool _active) { bActionActive_ = _active; }

	

};
