// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkateboardingSimGameMode.h"
#include "SkateboardingSimCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

ASkateboardingSimGameMode::ASkateboardingSimGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void ASkateboardingSimGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	StartTimerDecrement();
}

void ASkateboardingSimGameMode::StartTimerDecrement()
{
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ASkateboardingSimGameMode::DecrementTimer, 1.0f, true);
}

void ASkateboardingSimGameMode::DecrementTimer()
{
	if (TimerSeconds > 0)
	{
		TimerSeconds--;
	}
	else
	{
		// Para o timer quando chegar a zero
		GetWorldTimerManager().ClearTimer(TimerHandle);

		// Carrega o novo mapa
		if (!EndMapName.IsNone())
		{
			UGameplayStatics::OpenLevel(this, EndMapName);
		}
	}
}

void ASkateboardingSimGameMode::SetEndMapName(FName MapName)
{
	EndMapName = MapName;
}
