// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "PickupsGameState.generated.h"

/*游戏状态类：对局中道具数量，所有玩家的得分 
 * 
 */
UCLASS()
class PICKUPS_API APickupsGameState : public AGameState
{
	GENERATED_BODY()

protected:

	UPROPERTY(Replicated, BlueprintReadOnly)// 复制到客户端
	int32 PickupsRemaining; //剩余拾取物的数量

	virtual void BeginPlay() override; //重写开始游戏函数

	UFUNCTION(BlueprintCallable)
	TArray<APlayerState*> GetPlayerStatesOrderedByScore() const; // 获取所有玩家的得分，并排序

public:

	void RemovePickup() { PickupsRemaining--; } //当有人拾取物品时被调用，剩余数量--

	bool HasPickups() const { return PickupsRemaining > 0; } //GameMode判断是否游戏结束，返回剩余物品数量是否大于0
	
};
