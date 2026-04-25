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