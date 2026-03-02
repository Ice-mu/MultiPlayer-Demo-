// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PickupsPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class PICKUPS_API APickupsPlayerState : public APlayerState
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(Replicated, BlueprintReadOnly)// 复制到客户端
	int32 nums_Pickups;

public:

	void AddPickup() { nums_Pickups++; } // 自己拾取物品时++

};
