#include "MultiplayerLobbyPlayerState.h"
#include "Net/UnrealNetwork.h"

AMultiplayerLobbyPlayerState::AMultiplayerLobbyPlayerState()
{
	bReplicates = true;
}

void AMultiplayerLobbyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMultiplayerLobbyPlayerState, bIsReady);
	DOREPLIFETIME(AMultiplayerLobbyPlayerState, bVoiceEnabled);
}

void AMultiplayerLobbyPlayerState::SetReadyState(bool bInReady)
{
	bIsReady = bInReady;
}

void AMultiplayerLobbyPlayerState::SetVoiceEnabled(bool bInVoiceEnabled)
{
	bVoiceEnabled = bInVoiceEnabled;
}

void AMultiplayerLobbyPlayerState::OnRep_ReadyState()
{
}

void AMultiplayerLobbyPlayerState::OnRep_VoiceEnabled()
{
}
