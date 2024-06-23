// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkateboardingSimCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "TimerManager.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Components/BoxComponent.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ASkateboardingSimCharacter

ASkateboardingSimCharacter::ASkateboardingSimCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input.
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the 
	// and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Setup obstacle detection
	ObstacleDetector = CreateDefaultSubobject<UBoxComponent>(TEXT("ObstacleDetector"));
	ObstacleDetector->SetupAttachment(RootComponent);
	ObstacleDetector->OnComponentBeginOverlap.AddDynamic(this, &ASkateboardingSimCharacter::OnJumpedOverObstacle);

	GetCharacterMovement()->RotationRate = FRotator(0.0f, 180.0f, 0.0f); // Slower turning rate for smooth curves
	GetCharacterMovement()->GroundFriction = 0.2f; // Low friction for sliding effect

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ASkateboardingSimCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void ASkateboardingSimCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//////////////////////////////////////////////////////////////////////////
// Input

void ASkateboardingSimCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this,
			&ASkateboardingSimCharacter::SkateJump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this,
			&ASkateboardingSimCharacter::SkateStopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this,
			&ASkateboardingSimCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this,
			&ASkateboardingSimCharacter::Look);

		// Pushing
		EnhancedInputComponent->BindAction(PushAction, ETriggerEvent::Started, this,
			&ASkateboardingSimCharacter::Push);
		EnhancedInputComponent->BindAction(PushAction, ETriggerEvent::Completed, this,
			&ASkateboardingSimCharacter::ReturnNormalSpeed);

		// Slowing Down
		EnhancedInputComponent->BindAction(SlowDownAction, ETriggerEvent::Started, this,
			&ASkateboardingSimCharacter::SlowDown);
		EnhancedInputComponent->BindAction(SlowDownAction, ETriggerEvent::Completed, this,
			&ASkateboardingSimCharacter::ReturnNormalSpeed);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error,
			TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input "
			"system. If you intend to use the legacy system, then you will need to update this C++ file."),
			*GetNameSafe(this));
	}
}

void ASkateboardingSimCharacter::Move(const FInputActionValue& Value)
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

void ASkateboardingSimCharacter::Look(const FInputActionValue& Value)
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

void ASkateboardingSimCharacter::Push()
{
	GetCharacterMovement()->MaxWalkSpeed = PushedMaxWalkSpeed;
}

void ASkateboardingSimCharacter::ReturnNormalSpeed()
{
	GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkSpeed;
}

void ASkateboardingSimCharacter::SlowDown()
{
	GetCharacterMovement()->MaxWalkSpeed = SlowDownMaxWalkSpeed;
}

void ASkateboardingSimCharacter::OnJumpedOverObstacle(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Check if the other actor is a jumpable obstacle
	if (OtherActor && OtherActor != this && OtherComp)
	{
		AddPoint();
	}
}

void ASkateboardingSimCharacter::AddPoint()
{
	Points++;
	UE_LOG(LogTemplateCharacter, Log, TEXT("Points: %d"), Points);
}

void ASkateboardingSimCharacter::SkateJump()
{
	// Only jump if character is on the ground
	if (GetCharacterMovement()->IsMovingOnGround())
	{
		bIsJumping = true;
		bIsSkating = false;
		Jump();
	}
}

void ASkateboardingSimCharacter::SkateStopJumping()
{
	// Nothing needed here since OnLanded will handle it
}

void ASkateboardingSimCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	UE_LOG(LogTemp, Error, TEXT("Landed"));

	// Call the function to reset states after landing
	OnLanded();
}

void ASkateboardingSimCharacter::OnLanded()
{
	bIsJumping = false;
	bIsSkating = true;
	StopJumping();
}