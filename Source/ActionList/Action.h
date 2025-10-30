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
	, Rotate UMETA(DisplayName = "Rotate")
	, Flip UMETA(DisplayName = "Flip")
	, Fade UMETA(DisplayName = "Fade")
	, Delay UMETA(DisplayName = "Delay")
	, SpawnCard UMETA(DisplayName = "SpawnCard")
};
/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActionUpdate, float, deltaTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnActionInit);

UCLASS(Abstract, Blueprintable) // Abstract base class.
class ACTIONLIST_API UAction : public UObject
{
	GENERATED_BODY()
private:
	UPROPERTY()
	bool bActionActive_ = false;

	UPROPERTY()
	EActionType actionType_ = EActionType::DEFAULT;

	

	//UPROPERTY()
	//float blockTime = 0.f;

	UPROPERTY()
	bool bDoDelete_ = false;

public:
		
	UPROPERTY(BlueprintReadWrite)
	bool bIsSequence_ = false;

	UPROPERTY(BlueprintReadWrite)
	bool bIsBlocking_ = false;

	UPROPERTY(BlueprintAssignable)
	FOnActionUpdate OnActionUpdate;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnActionInit OnActionInit;
	
	UPROPERTY(BlueprintReadWrite)
	float actionDuration_;
	
	UPROPERTY(BlueprintReadWrite)
	AActor *affectedObject_;

	UPROPERTY(BlueprintReadWrite)
	float actionCurrTime_;	

	UFUNCTION(BlueprintImplementableEvent, Category = "Action")
	void BindDelegates();
	
	UFUNCTION(BlueprintCallable, Category = "Action")
	virtual void Init()
	{		
		BindDelegates();
		//OnActionInit.Broadcast();		
	}	
		
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Action")
	virtual bool IsBlocking(){ return bIsBlocking_; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Action")
	virtual EActionType GetType() { return actionType_; }

	virtual void SetType(EActionType type) { actionType_ = type; }

	

	UFUNCTION(BlueprintCallable, Category = "Action")
	virtual void Execute() PURE_VIRTUAL(UAction::Execute, );
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Action")
	virtual bool CanExecute() const 
	{ 
		return !bActionActive_ 
			&& !bDoDelete_ 
			&& (affectedObject_ == nullptr); 
	}
	
	UFUNCTION(BlueprintCallable, Category = "Action")
	virtual bool Update(float _dt) {return true;}

	UFUNCTION(BlueprintCallable, Category = "Action")
	virtual bool DoDelete() {return bDoDelete_; }

	UFUNCTION(BlueprintCallable, Category = "Action")
	virtual void SetDeleteFlag(bool flag) { bDoDelete_ = flag; }

	UFUNCTION(BlueprintCallable, Category = "Action")
	virtual bool IsActive() const {return bActionActive_;}

	UFUNCTION(BlueprintCallable, Category = "Action")
	virtual bool IsDone() const { return actionCurrTime_ >= actionDuration_; }

	UFUNCTION(BlueprintCallable, Category = "Action")
	virtual void SetActive(bool _active) { bActionActive_ = _active; }

	

};
