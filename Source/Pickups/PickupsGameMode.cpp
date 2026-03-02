// Copyright Epic Games, Inc. All Rights Reserved.

#include "PickupsGameMode.h"
#include "PickupsCharacter.h" 
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h" //各种GamePlay逻辑
#include "PickupsGameState.h"// 需要获取游戏状态

APickupsGameMode::APickupsGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void APickupsGameMode::BeginPlay()
{
	Super::BeginPlay();
	//获取 GameState对象，强制转换为PickupsGameState类型
	MyGameState = GetGameState<APickupsGameState>();
}

bool APickupsGameMode::ShouldSpawnAtStartSpot(AController* Player)
{
	return false;
}

/**
 * 重写 HandleMatchHasStarted 函数
 *
 * 这个函数在比赛正式开始（所有玩家准备就绪）时被调用。
 * 在这里执行比赛开始时的逻辑，如：
 */
void APickupsGameMode::HandleMatchHasStarted()
{
	// 调用父类的 HandleMatchHasStarted
	// 父类会处理标准的比赛开始逻辑（如广播事件、更新游戏状态等）
	Super::HandleMatchHasStarted();

	/**
	 * 在屏幕上显示调试信息
	 *
	 * GEngine->AddOnScreenDebugMessage 是UE的调试工具：
	 * 参数1：-1               - 每次调用都创建新消息（不替换现有消息）
	 * 参数2：2.0f             - 消息显示2秒后消失
	 * 参数3：FColor::Green    - 消息文字颜色为绿色
	 * 参数4："The game has started!" - 要显示的消息内容
	 *
	 * 这行代码会在游戏视口左上角显示"游戏开始"的提示，
	 * 帮助开发者在测试时确认比赛开始事件被正确触发。
	 */
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, "Game Started!");
}

void APickupsGameMode::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();

	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "The game has ended!"); //显示调试信息

	//获取所有角色信息并存储在Characters中
	TArray<AActor*> Characters; 
	UGameplayStatics::GetAllActorsOfClass(this, APickupsCharacter::StaticClass(), Characters);

	//销毁所有角色
	for (AActor* Character : Characters)
	{
		Character->Destroy();
	}
	//设置计时器
	FTimerHandle TimerHandle;
	/**
	 * 设置计时器，5秒后重启地图 调用RestartMap
	 *
	 * GetWorldTimerManager() 获取世界的计时器管理器
	 * SetTimer 参数说明：
	 * 参数1：TimerHandle     - 输出参数，存储计时器句柄
	 * 参数2：this            - 拥有此计时器的对象（当前GameMode）
	 * 参数3：&APickupsGameMode::RestartMap - 要调用的成员函数指针
	 * 参数4：5.0f            - 延迟时间（秒）
	 *
	 * 5秒后，RestartMap 函数会被调用，重新开始当前地图
	 * 这给玩家短暂的时间看到游戏结束的提示
	 */
	GetWorldTimerManager().SetTimer(TimerHandle, this, &APickupsGameMode::RestartMap, 5.0f);
}

bool APickupsGameMode::ReadyToStartMatch_Implementation()
{   // 开始游戏的条件，这里不做检查直接返回，表示所有玩家一加入就开始游戏
	//可做扩展：准备后游戏才开始
	return true;
}

bool APickupsGameMode::ReadyToEndMatch_Implementation()
{
	/**
	 * 判断比赛是否可以结束的条件
	 * 1. MyGameState != nullptr - 确保游戏状态已初始化
	 * 2. !MyGameState->HasPickups() - 检查是否还有可拾取物品
	 */
	return MyGameState != nullptr && !MyGameState->HasPickups();
}

void APickupsGameMode::RestartMap() const
{	/**
     * 使用服务器旅行（ServerTravel）切换到指定地图
     * 
     * GetWorld()->ServerTravel 参数说明：
     * 参数1：GetWorld()->GetName() - 目标地图的名称（当前地图）
     * 参数2：false - 是否绝对路径（false 表示相对路径）
     * 参数3：false - 是否跳过游戏模式协商
     * 
     * ServerTravel 的作用：
     * 1. 在服务器上加载指定地图
     * 2. 自动将所有连接的客户端带到新地图
     * 3. 保持网络连接不断开
     * 
     * 这相当于"重新开始当前关卡"
     */
	GetWorld()->ServerTravel(GetWorld()->GetName(), false, false);
}
