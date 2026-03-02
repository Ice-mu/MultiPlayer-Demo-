// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "PickupsGameMode.generated.h"
//GameMode 游戏的基本逻辑，根据游戏状态判断要做什么，需要持有游戏状态的指针
UCLASS(minimalapi)
class APickupsGameMode : public AGameMode
{
	GENERATED_BODY()

protected:

	UPROPERTY()
	class APickupsGameState* MyGameState;

	APickupsGameMode();

	virtual void BeginPlay() override;

	virtual bool ShouldSpawnAtStartSpot(AController* Player) override; //重写内置函数，重生是否在固定原点

	//处理各种比赛逻辑
	virtual void HandleMatchHasStarted() override; //比赛开始时的逻辑
	virtual void HandleMatchHasEnded() override;
	virtual bool ReadyToStartMatch_Implementation() override;
	virtual bool ReadyToEndMatch_Implementation() override;

	void RestartMap() const;
};



