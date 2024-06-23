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

UCLASS(config=Game)
class ASkateboardingSimCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	/**
	* Called when the character jumps over an obstacle.
	* 
	* @param OverlappedComponent The component that triggered the overlap.
	* @param OtherActor The other actor involved in the overlap.
	* @param OtherComp The other component involved in the overlap.
	* @param OtherBodyIndex The index of the other body.
	* @param bFromSweep Whether the overlap was caused by a sweep.
	* @param SweepResult The result of the sweep.
	*/
	UFUNCTION()
	void OnJumpedOverObstacle(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
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

	/** Handles the stop push action, resetting to default speed */
	void StopSlowDown();
	
	/** Adds a point to the character's score. */
	void AddPoint();

	void SkateJump();

	void SkateStopJumping();
	void Landed(const FHitResult& Hit);

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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsIdle = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsWalking = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsJumping = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsSkating = true;

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

