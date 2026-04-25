#include "PlayerState/AstroDropLobbyPlayerState.h"
#include "GameMode/AstroDropLobbyGameMode.h"

void AAstroDropLobbyPlayerState::SetReadyState(bool bInReady)
{
	if (bIsReady == bInReady)
	{
		return;
	}

	// usa lógica do plugin
	Super::SetReadyState(bInReady);

	if (HasAuthority())
	{
		if (AAstroDropLobbyGameMode* GM =
			GetWorld()->GetAuthGameMode<AAstroDropLobbyGameMode>())
		{
			GM->PlayerReadyChanged();
		}
	}
}