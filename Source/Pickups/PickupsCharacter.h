// Copyright Epic Games, Inc. All Rights Reserved.
// GameCharacter 负责角色行为逻辑的实现，包括一些角色功能动作，由角色触发的事件，改变PlayerState的函数。
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "PickupsCharacter.generated.h"


UCLASS(config=Game)
class APickupsCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

public:
	APickupsCharacter();
	

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickups Character") //下落时音效
	USoundBase* FallSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickups Character") //落地时音效
	USoundBase* LandSound;

	//一些角色具体行为，触发事件的实现
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;     //游戏结束
	virtual void Landed(const FHitResult& Hit) override;                         //落地
	virtual void FellOutOfWorld(const UDamageType& DmgType) override;           //调出世界

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	void AddScore(const float Score) const; //拾取物品时，加分
	void AddPickup() const;                 //拾取物品时，加自己持有的数量

	UFUNCTION(Client, Unreliable) //表示这是一个客户端RPC，在服务器调用，客户端执行。
	void ClientPlaySound2D(USoundBase* Sound);
};

