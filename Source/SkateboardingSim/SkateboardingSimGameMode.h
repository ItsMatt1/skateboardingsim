// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SkateboardingSimGameMode.generated.h"

/**
* @brief Game mode class for the Skateboarding Simulator.
* 
* This class defines the game mode for the Skateboarding Simulator, including
* a timer mechanism with getter and setter functions accessible via Blueprints.
*/
UCLASS(minimalapi)
class ASkateboardingSimGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	/**
	* @brief Default constructor.
	* 
	* Initializes the game mode with default settings.
	*/
	ASkateboardingSimGameMode();

	/** Called when the game starts or when spawned. */
	void BeginPlay();

	/**
	* Starts the timer decrement process.
	* 
	* Sets a repeating timer that calls DecrementTimer() every second.
	*/
	void StartTimerDecrement();

	/**
	* Decrements the timer every second.
	* 
	* If the timer reaches zero, stops the timer and loads a new map if EndMapName is set.
	*/
	void DecrementTimer();

	/**
	* Sets the name of the map to load when the timer reaches zero.
	* 
	* @param MapName The name of the map to load.
	*/
	void SetEndMapName(FName MapName);

public:
	/**
	* @brief Gets the current timer seconds.
	* 
	* This function returns the current value of the TimerSeconds variable.
	* 
	* @return The current timer seconds as an integer.
	*/
	UFUNCTION(BlueprintCallable, Category="Timer")
	int32 GetTimerSeconds() const
	{
		return TimerSeconds;
	}

	/**
	* @brief Sets the timer seconds.
	* 
	* This function sets a new value for the TimerSeconds variable.
	* 
	* @param NewTimerSeconds The new timer seconds value as an integer.
	*/
	UFUNCTION(BlueprintCallable, Category="Timer")
	inline void SetTimerSeconds(int32 NewTimerSeconds)
	{
		TimerSeconds = NewTimerSeconds;
	}

private:
	/** The timer seconds. Default value is 120.0f. */
	int32 TimerSeconds = 120.0f;
	
	/** Handle for the timer. */
	FTimerHandle TimerHandle;
	
	/** The name of the map to load when the timer ends. */
	FName EndMapName = "MainMenuMap";
};



