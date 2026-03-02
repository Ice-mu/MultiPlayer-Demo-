// Fill out your copyright notice in the Description page of Project Settings.
// 实现拾取物 的物理表现 动画 音效  
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pickup.generated.h"

UCLASS()
class PICKUPS_API APickup : public AActor
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
	UStaticMeshComponent* Mesh; // 网格体视觉，给所有客户端

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
	class URotatingMovementComponent* RotatingMovement; //旋转动画，给所有客户端

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup")
	USoundBase* PickupSound; //音效，给所有客户端

	APickup();

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);
};
