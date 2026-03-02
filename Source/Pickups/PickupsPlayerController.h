// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PickupsPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PICKUPS_API APickupsPlayerController : public APlayerController
{
	GENERATED_BODY()
protected:
    /**
     * 计分板控件类
     *
     * TSubclassOf<UUserWidget> 是一种类型安全的模板类
     * 用于存储对UUserWidget派生类的引用，而不是具体实例
     *
     * EditDefaultsOnly - 只能在蓝图类默认值中编辑
     * BlueprintReadOnly - 蓝图可读取但不能修改
     * Category - 在编辑器中的分类名称
     */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup Player Controller")
	TSubclassOf<class UUserWidget> ScoreboardMenuClass;  //记分牌，类型在蓝图中后续指定，派生自UUserWidget就可以

    /**
     * 计分板控件实例指针
     *
     * 存储动态创建的计分板UI实例
     * UPROPERTY() 标记确保UE的垃圾回收系统知道这个引用
     * 防止控件被意外销毁
     */
	UPROPERTY()
	class UUserWidget* ScoreboardMenu;

	virtual void BeginPlay() override;
};
