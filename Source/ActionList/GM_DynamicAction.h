// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Action.h"
#include "ActionGroup.h"
#include "ActionDirector.h"
#include "GameFramework/GameModeBase.h"
#include "GM_DynamicAction.generated.h"

UENUM(BlueprintType)
enum class EActionType : uint8
{
	DEFAULT	UMETA(DisplayName = "DEFAULT")
	, Move	UMETA(DisplayName = "Move")
};
/**
 * 
 */
UCLASS()
class ACTIONLIST_API AGM_DynamicAction : public AGameModeBase
{
	GENERATED_BODY()
	
};
