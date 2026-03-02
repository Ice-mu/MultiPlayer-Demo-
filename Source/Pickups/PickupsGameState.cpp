// Fill out your copyright notice in the Description page of Project Settings.

// 包含当前类对应的头文件
#include "PickupsGameState.h"
// 包含可拾取物品类的头文件，用于后续获取场景中所有的可拾取物品
#include "Pickup.h"
// 包含游戏静态工具函数库，提供了如获取所有Actor等实用功能
#include "Kismet/GameplayStatics.h"
// 包含网络复制相关的头文件，用于定义属性复制的方式
#include "Net/UnrealNetwork.h"
// 包含玩家状态类的头文件，用于操作玩家状态对象
#include "GameFramework/PlayerState.h"

//游戏状态需要以下功能
//1. 把需要同步给所有玩家的变量标记为网络复制
//2. 重写BeginPlay() 函数，初始化游戏状态
//3. 获取所有玩家的分数，生成排行榜。


/**
 * 重写函数：获取需要网络复制的属性列表
 *
 * 这个函数是虚幻网络系统的一部分，用于指定哪些类的成员变量需要在服务器和客户端之间进行同步。
 *
 * @param OutLifetimeProps 用于存储需要复制的属性列表的数组引用
 */
void APickupsGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	// 调用父类的同名函数，确保父类定义的复制属性也被添加
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 宏指令：将 PickupsRemaining 变量标记为需要网络复制
	DOREPLIFETIME(APickupsGameState, PickupsRemaining);
}

/**
 * 重写函数：游戏开始播放时调用
 *
 * BeginPlay 在游戏开始或Actor被生成到世界时调用，用于初始化逻辑。
 * 在这个函数中，我们计算初始的可拾取物品数量。
 */
void APickupsGameState::BeginPlay()
{
	// 调用父类的 BeginPlay，确保父类的初始化逻辑被执行
	Super::BeginPlay();

	// 声明一个数组，用于存储找到的所有可拾取物品Actor的指针，Actor指针数组。
	TArray<AActor*> Pickups;

	// 使用游戏静态工具函数获取世界中所有类型为 APickup 的Actor
	// 参数1：当前世界的上下文对象（this 指向当前的 GameState），会自己调用GetWorld。
	// 参数2：要查找的Actor的类，这里使用 APickup::StaticClass() 获取 APickup 类的类信息，UE 的反射机制，匹配类的信息
	// 参数3：输出参数，用于存储找到的Actor数组
	UGameplayStatics::GetAllActorsOfClass(this, APickup::StaticClass(), Pickups);

	// 计算找到的可拾取物品的数量，并将其赋值给 PickupsRemaining 变量
	PickupsRemaining = Pickups.Num();
}

/**
 * 成员函数：获取按分数排序的玩家状态数组
 *
 * 这个函数返回一个根据玩家分数降序排列的玩家状态列表，通常用于显示排行榜。
 *
 * @return 按分数从高到低排序的 TArray<APlayerState*> 数组
 */
TArray<APlayerState*> APickupsGameState::GetPlayerStatesOrderedByScore() const
{
	// 将父类继承来的 PlayerArray（存储所有玩家状态的数组）复制一份
	// 这样做的目的是为了不修改原始的 PlayerArray，而是对其副本进行排序
	TArray<APlayerState*> PlayerStates(PlayerArray);

	// 对 PlayerStates 数组进行排序,按分数降序排序
	// Sort 函数接受一个 Lambda 表达式作为自定义排序规则
	PlayerStates.Sort([](const APlayerState& A, const APlayerState& B) { return A.GetScore() > B.GetScore(); });

	// 返回排序后的玩家状态数组（副本）
	return PlayerStates;
}