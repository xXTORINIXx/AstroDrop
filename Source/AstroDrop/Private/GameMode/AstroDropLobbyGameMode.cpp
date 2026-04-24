// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/AstroDropLobbyGameMode.h"

#include "GameFramework/GameStateBase.h"
#include "PlayerController/AstroDropPlayerController.h"


AAstroDropLobbyGameMode::AAstroDropLobbyGameMode()
{
	PlayerControllerClass = AAstroDropPlayerController::StaticClass();
	
}

void AAstroDropLobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	const int32 NumberOfPlayers = GameState ? GameState->PlayerArray.Num() : 0;
	if (NumberOfPlayers >= PlayersToStartMatch)
	{
		if (UWorld* World = GetWorld())
		{
			bUseSeamlessTravel = true;
			World->ServerTravel(FString::Printf(TEXT("%s?listen"), *MatchMapPath));
		}
	}
}

