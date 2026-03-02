// Copyright Epic Games, Inc. All Rights Reserved.

#include "PickupsCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PickupsPlayerState.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"



//////////////////////////////////////////////////////////////////////////
// APickupsCharacter

APickupsCharacter::APickupsCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void APickupsCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void APickupsCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APickupsCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APickupsCharacter::Look);

	}

}

void APickupsCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void APickupsCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void APickupsCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{//当角色被摧毁时，播放下落音效
	Super::EndPlay(EndPlayReason);

	if (EndPlayReason == EEndPlayReason::Destroyed)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), FallSound);
	}
}

void APickupsCharacter::Landed(const FHitResult& Hit)
{ //Land是自带的函数，角色落地时，会自动调用，这里重写额外添加了音效
	Super::Landed(Hit);

	UGameplayStatics::PlaySound2D(GetWorld(), LandSound);
}

//掉落，-10 摧毁角色，并且重新设置角色。
void APickupsCharacter::FellOutOfWorld(const UDamageType& DmgType)
{//DmgType 伤害类型，角色调出世界自动调用这个虚函数
	
	 /**
	 * 临时保存控制器的引用
	 *
	 * 注意：我们需要在 Destroy() 之前保存 Controller 指针，
	 * 因为 Destroy() 会销毁角色，同时可能会清除 Controller 的 Possess 关系，
	 * 导致 Controller 指针变为 nullptr。但我们需要 Controller 来重生玩家。
	 */
	AController* TempController = Controller;

	AddScore(-10);

	Destroy();

	AGameMode* GameMode = GetWorld()->GetAuthGameMode<AGameMode>();

	if (GameMode != nullptr)
	{
		GameMode->RestartPlayer(TempController); //重新创建一个Char给Controller
	}
}

//加分 UE的PlayerState自己带Score相关的内容
void APickupsCharacter::AddScore(const float Score) const
{
	APlayerState* MyPlayerState = GetPlayerState();

	if (MyPlayerState != nullptr)
	{
		const float CurrentScore = MyPlayerState->GetScore();

		MyPlayerState->SetScore(CurrentScore + Score);
	}
}


void APickupsCharacter::AddPickup() const
{
	APickupsPlayerState* MyPlayerState = GetPlayerState<APickupsPlayerState>(); //Get并转换为子类

	if (MyPlayerState != nullptr)
	{
		MyPlayerState->AddPickup();
	}
}

/**
 * Client RPC 函数的实现
 *
 * 这个函数是 ClientPlaySound2D 的客户端实现版本。
 * 当服务器调用 ClientPlaySound2D 时，这个函数会在拥有此角色的客户端上执行。
 *
 * 注意：Client RPC 函数的实现需要添加 "_Implementation" 后缀
 *
 * @param Sound 要在客户端播放的声音资源
 * 需要服务器确认，但是只在特定客户端上播放的声音，拾取音效，自己听见就行了，
 */
void APickupsCharacter::ClientPlaySound2D_Implementation(USoundBase* Sound)
{
	UGameplayStatics::PlaySound2D(GetWorld(), Sound);
}



