// Fill out your copyright notice in the Description page of Project Settings.
// 上述注释为虚幻引擎自动生成的版权声明占位符，提示开发者在此处填写版权信息

// 包含当前类对应的头文件
#include "PickupsPlayerController.h"
// 包含用户控件基类的头文件，用于创建和操作UI控件
#include "Blueprint/UserWidget.h"

/**
 * 重写 BeginPlay 函数
 *
 * BeginPlay 在游戏开始或 PlayerController 被生成到世界中时自动调用。
 * 在这个函数中，我们处理玩家控制器特有的初始化逻辑，特别是与本地玩家相关的UI创建。
 */
void APickupsPlayerController::BeginPlay()
{
	// 调用父类的 BeginPlay，确保父类的初始化逻辑被执行
	// 这对于保持引擎默认行为（如输入设置、相机初始化等）至关重要
	Super::BeginPlay();

	/**
	 * IsLocalController() 检查这个 PlayerController 是否控制本地玩家
	 *
	 * 在多玩家游戏中，每个客户端都有自己的 PlayerController 实例：
	 * - 本地客户端：IsLocalController() 返回 true
	 * - 远程客户端/服务器上的其他玩家控制器：返回 false
	 *
	 * 这个检查确保 UI 只会在拥有它的客户端上创建，避免在服务器或远程客户端上重复创建
	 *
	 * ScoreboardMenuClass != nullptr 检查确保计分板类已被正确设置
	 * ScoreboardMenuClass 应该是一个 UPROPERTY(EditDefaultsOnly) 变量，在蓝图中赋值
	 */
	if (IsLocalController() && ScoreboardMenuClass != nullptr)
	{
		/**
		 * CreateWidget 函数：动态创建 UI 控件实例
		 *
		 * 参数1：this - 创建控件的父对象（当前 PlayerController）
		 * 参数2：ScoreboardMenuClass - 要创建的控件类（通常是蓝图类）
		 *
		 * 函数返回：UUserWidget* 类型的指针，指向新创建的控件实例
		 *
		 * CreateWidget 不会自动将控件显示在屏幕上，只是创建了控件对象
		 */
		ScoreboardMenu = CreateWidget<UUserWidget>(this, ScoreboardMenuClass);

		/**
		 * 检查 ScoreboardMenu 是否成功创建
		 * 如果创建失败（可能因为无效的类或内存不足），ScoreboardMenu 将为 nullptr
		 */
		if (ScoreboardMenu != nullptr)
		{
			/**
			 * AddToViewport 函数：将控件添加到视口并显示
			 *
			 * 参数 0：ZOrder 顺序（层级）
			 * - 数值越大，控件显示在越上层
			 * - 使用 0 表示默认层级
			 *
			 *  这句话只设置渲染层级，不决定显示的位置
			 */
			ScoreboardMenu->AddToViewport(0);
		}
	}
}