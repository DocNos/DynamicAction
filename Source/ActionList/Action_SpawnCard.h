// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Action.h"
#include "Action_SpawnCard.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class ACTIONLIST_API UAction_SpawnCard : public UAction
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite)
	int playerOwner_;

	UPROPERTY(BlueprintReadWrite)
	FRotator spawnRotation_;

	UPROPERTY(BlueprintReadWrite)
	FVector spawnLocation_;

	UPROPERTY(BlueprintReadWrite)
	int cardValue_;

	virtual void Execute() override;
	virtual bool Update(float) override;
	virtual EActionType GetType() override { return EActionType::SpawnCard; }

	void Initialize(int _playerOwner, int _cardValue,
					FRotator _spawnRotation, FVector _spawnLocation, float Duration)
	{
		//affectedObject_ = Target;
		playerOwner_ = _playerOwner;
		cardValue_ = _cardValue;
		spawnRotation_ = _spawnRotation;
		spawnLocation_ = _spawnLocation;
		actionDuration_ = Duration;
		actionCurrTime_ = 0.0f;
		SetType(EActionType::SpawnCard);
	}

	void Init() override;
	
};
