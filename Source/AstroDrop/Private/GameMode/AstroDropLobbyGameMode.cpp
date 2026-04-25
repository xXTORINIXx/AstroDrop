#include "GameMode/AstroDropLobbyGameMode.h"

#include "PlayerController/AstroDropPlayerController.h"
#include "PlayerState/AstroDropLobbyPlayerState.h"
#include "GameFramework/GameStateBase.h"

AAstroDropLobbyGameMode::AAstroDropLobbyGameMode()
{
	PlayerControllerClass =
		AAstroDropPlayerController::StaticClass();

	PlayerStateClass =
		AAstroDropLobbyPlayerState::StaticClass();
}

void AAstroDropLobbyGameMode::PostLogin(
	APlayerController* NewPlayer
)
{
	Super::PostLogin(NewPlayer);
}

void AAstroDropLobbyGameMode::PlayerReadyChanged()
{
	CheckStartMatch();
}

void AAstroDropLobbyGameMode::CheckStartMatch()
{
	if (!GameState || bMatchStarting)
	{
		return;
	}

	const int32 NumberOfPlayers =
		GameState->PlayerArray.Num();

	if (NumberOfPlayers < PlayersToStartMatch)
	{
		return;
	}

	for (APlayerState* PS : GameState->PlayerArray)
	{
		AAstroDropLobbyPlayerState* LobbyPS =
			Cast<AAstroDropLobbyPlayerState>(PS);

		if (!LobbyPS || !LobbyPS->GetReadyState())
		{
			return;
		}
	}

	// Todos Ready
	bMatchStarting = true;

	if (UWorld* World = GetWorld())
	{
		bUseSeamlessTravel = true;

		World->ServerTravel(
			FString::Printf(
				TEXT("%s?listen"),
				*MatchMapPath
			)
		);
	}
}