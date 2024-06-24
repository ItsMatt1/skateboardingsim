// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "SkateboardingSimCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
* @brief Game mode class for the Skateboarding Simulator.
* 
* This class defines the game mode for the Skateboarding Simulator, including
* a timer mechanism and a points system with getter and setter functions accessible via Blueprints.
*/
UCLASS(config=Game)
class ASkateboardingSimCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	/**
	* @brief Gets the current points of the character.
	* 
	* This function returns the current value of the Points variable.
	* 
	* @return The current points as an integer.
	*/
	UFUNCTION(BlueprintCallable, Category="Points")
	int32 GetPoints() const
	{
		return Points;
	}

	/**
	* @brief Sets the points of the character.
	* 
	* This function sets a new value for the Points variable.
	* 
	* @param NewPoints The new points value as an integer.
	*/
	UFUNCTION(BlueprintCallable, Category="Points")
	inline void SetPoints(int32 NewPoints)
	{
		Points = NewPoints;
	}
	
public:
	/* Default Constructor */
	ASkateboardingSimCharacter();
	
    /** Returns CameraBoom subobject **/
    FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
    /** Returns FollowCamera subobject **/
    FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

protected:
	/**
	* Sets up the player input component.
	* 
	* @param PlayerInputComponent The input component to set up.
	*/
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	/** Called when the game starts or when spawned. */
	virtual void BeginPlay();
	
	virtual void Tick(float DeltaTime);

private:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Handles the character push action, increasing speed */
	void Push();

	/** Handles the stop push action, resetting to default speed */
	void ReturnNormalSpeed();

	/** Handles the slow down action, decreasing speed. */
	void SlowDown();

	/**
	* Checks if the character is currently over an obstacle.
	* 
	* This function performs a line trace downwards from the JumpDetectionBox's location.
	* If an obstacle is detected, it increments the score by calling AddPoint().
	* The state of whether the character is over an obstacle is updated accordingly.
	* 
	* @note This function is called during Tick when the character is jumping.
	*/
	void CheckForObstacle();

	/**
	* Fades out the rolling sound over time until it stops completely.
	* This function reduces the volume of the RollingAudioComponent gradually
	* when the character is slowing down or has stopped moving.
	*
	* @param DeltaTime The time since the last tick.
	*/
	void FadeOutRollingSound(float DeltaTime);

	/** Adds a point to the character's score. */
	void AddPoint();

	/** Initiates the jump action if the character is on the ground */
	void SkateJump();

	/** Handles the stop jumping action */
	void SkateStopJumping();

	/** Handles additional logic after the character lands */
	void OnLanded();

	/** 
	* Called when the character lands on the ground
	* 
	* @param Hit The hit result from the landing 
	*/
	virtual void Landed(const FHitResult& Hit) override;

public:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom = nullptr;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera = nullptr;

	/** A box for obstacle detection */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UBoxComponent* ObstacleDetector = nullptr;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext = nullptr;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction = nullptr;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction = nullptr;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction = nullptr;

	/** Push Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* PushAction = nullptr;

	/** Slow Down Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SlowDownAction = nullptr;

	/* Bool to keep track of states for animation purposes */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsIdle = true;

	/* Bool to keep track of states for animation purposes */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsWalking = false;

	/* Bool to keep track of states for animation purposes */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsJumping = false;

	/* Bool to keep track of states for animation purposes */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsSkating = true;

	/** Bool to keep track of whether the character is currently over an obstacle. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsOverObstacle = false;

	/** Box component used for detecting jumps over obstacles. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Detection", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* JumpDetectionBox = nullptr;

	/** A sound for when a obstacle is jumped over. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* PointSound = nullptr;

	/** A sound for the player is moving. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* RollingSound = nullptr;

	/**
	* Component for playing the skate sound effect.
	* This component is initialized in the constructor and attached to the root component.
	* It is used to provide continuous audio feedback while the character is skating.
	*/
	UPROPERTY()
	UAudioComponent* RollingAudioComponent;

	/**
	* Sound to play when the character performs a jump.
	* This sound is triggered in the SkateJump function if the character is on the ground.
	* The sound asset should be set in the Unreal Editor.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* JumpSound;

private:
	/** Current points of the character. */
	int32 Points = 0;

	/** Default maximum walking speed. */
	float DefaultMaxWalkSpeed = 500.f;

	/** Maximum walking speed when pushing. */
	float PushedMaxWalkSpeed = 1000.f;

	/** Maximum walking speed when slowing down. */
	float SlowDownMaxWalkSpeed = 200.f;
};

