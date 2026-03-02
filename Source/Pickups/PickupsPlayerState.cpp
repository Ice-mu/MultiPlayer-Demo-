// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupsPlayerState.h"
#include "Net/UnrealNetwork.h"

//每个人拾取的数量复制到所有客户端。
void APickupsPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	// 调用父类的同名函数，确保父类定义的复制属性也被添加
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APickupsPlayerState, nums_Pickups);
}

//为什么不用重写BeginPlay()初始化，UObject 自动初始化为0/NULL；