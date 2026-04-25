#include "MultiplayerLobbyGameMode.h"
#include "MultiplayerLobbyGameState.h"
#include "MultiplayerLobbyPlayerController.h"
#include "MultiplayerLobbyPlayerState.h"
#include "GameFramework/PlayerState.h"

AMultiplayerLobbyGameMode::AMultiplayerLobbyGameMode()
{
	GameStateClass = AMultiplayerLobbyGameState::StaticClass();
	PlayerControllerClass = AMultiplayerLobbyPlayerController::StaticClass();
	PlayerStateClass = AMultiplayerLobbyPlayerState::StaticClass();
}

void AMultiplayerLobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (AMultiplayerLobbyPlayerState* LobbyPlayerState = NewPlayer ? NewPlayer->GetPlayerState<AMultiplayerLobbyPlayerState>() : nullptr)
	{
		LobbyPlayerState->SetReadyState(false);
		LobbyPlayerState->SetVoiceEnabled(true);
		LobbyPlayerState->ForceNetUpdate();
	}
}

void AMultiplayerLobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
}

void AMultiplayerLobbyGameMode::PlayerReadyChanged()
{
	CheckStartMatch();
}

void AMultiplayerLobbyGameMode::CheckStartMatch()
{
	if (!bAutoStartWhenAllReady || bMatchStarting || !GameState)
	{
		return;
	}

	if (GameState->PlayerArray.Num() < PlayersToStartMatch)
	{
		return;
	}

	for (APlayerState* PS : GameState->PlayerArray)
	{
		AMultiplayerLobbyPlayerState* LobbyPS = Cast<AMultiplayerLobbyPlayerState>(PS);
		if (!LobbyPS || !LobbyPS->IsReady())
		{
			return;
		}
	}

	bMatchStarting = true;
	bUseSeamlessTravel = true;

	if (UWorld* World = GetWorld())
	{
		UE_LOG(LogTemp, Log, TEXT("All players ready. Traveling to match map: %s"), *MatchMapPath);
		World->ServerTravel(FString::Printf(TEXT("%s?listen"), *MatchMapPath));
	}
}
