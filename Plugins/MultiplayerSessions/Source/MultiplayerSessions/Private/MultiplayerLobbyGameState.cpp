#include "MultiplayerLobbyGameState.h"
#include "Net/UnrealNetwork.h"

AMultiplayerLobbyGameState::AMultiplayerLobbyGameState()
{
	bReplicates = true;
}

void AMultiplayerLobbyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMultiplayerLobbyGameState, RecentMessages);
}

void AMultiplayerLobbyGameState::AddChatMessage(const FMultiplayerChatMessage& NewMessage)
{
	RecentMessages.Add(NewMessage);

	while (RecentMessages.Num() > MaxChatMessages)
	{
		RecentMessages.RemoveAt(0);
	}

	ForceNetUpdate();
}

void AMultiplayerLobbyGameState::OnRep_RecentMessages()
{
}
