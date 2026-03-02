// Fill out your copyright notice in the Description page of Project Settings.
// 拾取物品类，实现

#include "Pickup.h"
#include "PickupsCharacter.h"
#include "PickupsGameState.h"
#include "GameFramework/RotatingMovementComponent.h"

/**
 * APickup 类的构造函数
 * 当拾取物品被创建（在编辑器中放置或在游戏中生成）时自动调用。
 */
APickup::APickup()
{
	/**
	* 创建静态网格体组件作为主要视觉表现
	*
	* CreateDefaultSubobject 是 UE 中创建组件的标准方法：
	* - 第一个参数：组件的内部名称（用于在编辑器中识别）
	* - 返回：指向新创建组件的指针
	*/
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	/**
	 * 设置碰撞配置文件的名称
	 * 
	 * "OverlapAll" 是一个预定义的碰撞预设，意味着：
	 * - 这个物体会与所有其他物体产生重叠事件（OnOverlap）
	 * - 但不会阻挡物理运动（没有碰撞阻挡）
	 * - 适合可拾取物品，玩家可以穿过并触发拾取事件
	 */
	Mesh->SetCollisionProfileName("OverlapAll");
	/**
	 * 将网格体组件设置为根组件
	 *
	 * 根组件是 Actor 变换（位置、旋转、缩放）的基础。
	 * 所有其他组件都会附加到根组件或其子组件上。
	 * 这里 Mesh 就是整个拾取物品的基础。
	 */
	RootComponent = Mesh;

	//初始化旋转运动组件，每秒旋转90度
	RotatingMovement = CreateDefaultSubobject<URotatingMovementComponent>("Rotating Movement");
	RotatingMovement->RotationRate = FRotator(0.0, 90.0f, 0);
	/**
	 * 启用网络复制
	 *
	 * bReplicates = true 表示这个 Actor 支持网络同步。
	 * 在多人游戏中：
	 * - 服务器上生成的这个物品会被复制到所有客户端
	 * - 客户端能看到这个物品的位置、旋转和状态变化
	 * - 拾取逻辑仍然在服务器上执行（服务器权威）
	 *
	 * 这对于多人拾取游戏至关重要！
	 */
	bReplicates = true;

	//禁用默认tick 旋转由旋转组件管理，碰撞也不需要逐帧检查。
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void APickup::BeginPlay()
{
	Super::BeginPlay();
	/**
	* 绑定碰撞重叠事件，
	*
	* OnComponentBeginOverlap 是一个多播委托（Multicast Delegate），
	* 当有其它物体（如玩家角色）与这个碰撞组件产生重叠时触发。
	*
	* AddDynamic 的语法解析：
	* - 第一个参数 this：指定事件处理函数所属的对象（当前 Pickup 实例）
	* - 第二个参数 &APickup::OnBeginOverlap：指定当事件触发时要调用的成员函数
	*
	* 这行代码的意思是："开始游戏后，当有人碰到我的 Mesh 组件时，请调用我的 OnBeginOverlap 函数"
	*/
	Mesh->OnComponentBeginOverlap.AddDynamic(this, &APickup::OnBeginOverlap);
}

/**
 * 碰撞重叠事件处理函数
 *
 * 当有其他物体（如玩家角色）碰到拾取物品时自动调用。
 * 这里实现拾取物品的核心逻辑：加分、更新游戏状态、播放音效、销毁物品。
 *
 * @param OverlappedComp   发生重叠的组件（当前物体的 Mesh 组件）
 * @param OtherActor       与当前物体重叠的其他 Actor（可能是玩家角色）
 * @param OtherComp        其他 Actor 的哪个组件发生了重叠
 * @param OtherBodyIndex   其他组件的身体索引（用于复杂碰撞体）
 * @param bFromSweep       是否是从 Sweep 检测触发的
 * @param Hit              如果 bFromSweep 为 true，这里包含碰撞详细信息
 */
void APickup::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	/**
	* 尝试将重叠的 Actor 转换为玩家角色类型
	*
	* Cast 是 UE 中的类型转换，类似于 C++ 的 dynamic_cast：
	* - 如果 OtherActor 是 APickupsCharacter 类型或其派生类，转换成功
	* - 否则返回 nullptr
	*/
	APickupsCharacter* Character = Cast<APickupsCharacter>(OtherActor);
	//确保重叠的是玩家角色，且在服务器发生 ，客户端触发不处理
	if (Character == nullptr || !HasAuthority())
	{
		return;
	}

	APickupsGameState* GameState = Cast<APickupsGameState>(GetWorld()->GetGameState()); //获取GameState

	if (GameState != nullptr)
	{
		GameState->RemovePickup(); //改变GameState状态
	}
	// 修改分数，拾取物品数，都是在服务器情况下才能调用
	Character->ClientPlaySound2D(PickupSound); //RPC 服务器调用，客户端远程执行。
	Character->AddScore(10);//加分，
	Character->AddPickup();//加拾取物

	//完成操作后，自毁。
	Destroy();
}

