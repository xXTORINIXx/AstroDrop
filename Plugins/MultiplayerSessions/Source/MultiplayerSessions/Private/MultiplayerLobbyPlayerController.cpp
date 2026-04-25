#include "MultiplayerLobbyPlayerController.h"
#include "MultiplayerLobbyGameState.h"
#include "MultiplayerLobbyPlayerState.h"
#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "GameFramework/PlayerState.h"

void AMultiplayerLobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UMultiplayerSessionsSubsystem* Subsystem =
			GI->GetSubsystem<UMultiplayerSessionsSubsystem>())
		{
			Subsystem->MultiplayerOnLoginComplete.AddDynamic(
				this,
				&ThisClass::HandleSubsystemLoginComplete
			);

			// DEBUG
			UE_LOG(
				LogTemp,
				Warning,
				TEXT("Bound LoginComplete delegate")
			);

			// Se login ocorreu antes do bind,
			// chama manualmente
			if (Subsystem->IsLoggedIn())
			{
				UE_LOG(
					LogTemp,
					Warning,
					TEXT("Already logged in, syncing nickname...")
				);

				HandleSubsystemLoginComplete(
					true,
					TEXT("")
				);
			}
		}
	}
}

void AMultiplayerLobbyPlayerController::HandleSubsystemLoginComplete(bool bWasSuccessful, const FString& Error)
{
	if (!bWasSuccessful)
	{
		return;
	}

	if (IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get())
	{
		IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
		if (Identity.IsValid())
		{
			const FString Nick = Identity->
				GetPlayerNickname(GetLocalPlayer() ? GetLocalPlayer()->GetControllerId() : 0);
			if (!Nick.IsEmpty())
			{
				ServerSetLobbyPlayerName(Nick);
			}
		}
	}
}

void AMultiplayerLobbyPlayerController::ServerSendLobbyChatMessage_Implementation(const FString& Message)
{
	if (Message.TrimStartAndEnd().IsEmpty())
	{
		return;
	}

	AMultiplayerLobbyGameState* LobbyGameState = GetWorld()
		                                             ? GetWorld()->GetGameState<AMultiplayerLobbyGameState>()
		                                             : nullptr;
	if (!LobbyGameState)
	{
		return;
	}

	FString SenderName = PlayerState ? PlayerState->GetPlayerName() : TEXT("Player");
	FString SenderId = PlayerState && PlayerState->GetUniqueId().IsValid()
		                   ? PlayerState->GetUniqueId()->ToString()
		                   : SenderName;

	if (IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get())
	{
		IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
		if (Identity.IsValid())
		{
			const FString Nick = Identity->
				GetPlayerNickname(GetLocalPlayer() ? GetLocalPlayer()->GetControllerId() : 0);
			if (!Nick.IsEmpty())
			{
				SenderName = Nick;
			}
		}
	}

	FMultiplayerChatMessage NewMessage;
	NewMessage.SenderId = SenderId;
	NewMessage.SenderName = SenderName;
	NewMessage.Message = Message.Left(256);
	NewMessage.Timestamp = FDateTime::UtcNow().ToString(TEXT("%H:%M:%S"));

	LobbyGameState->AddChatMessage(NewMessage);
}

void AMultiplayerLobbyPlayerController::ServerSetLobbyReadyState_Implementation(bool bNewReadyState)
{
	if (AMultiplayerLobbyPlayerState* LobbyPlayerState = GetPlayerState<AMultiplayerLobbyPlayerState>())
	{
		LobbyPlayerState->SetReadyState(bNewReadyState);
		LobbyPlayerState->ForceNetUpdate();
	}
}

void AMultiplayerLobbyPlayerController::ServerSetLobbyVoiceEnabled_Implementation(bool bNewVoiceEnabled)
{
	if (AMultiplayerLobbyPlayerState* LobbyPlayerState = GetPlayerState<AMultiplayerLobbyPlayerState>())
	{
		LobbyPlayerState->SetVoiceEnabled(bNewVoiceEnabled);
		LobbyPlayerState->ForceNetUpdate();
	}
}

void AMultiplayerLobbyPlayerController::
ServerSetLobbyPlayerName_Implementation(
	const FString& NewName)
{
	if (APlayerState* PS =
		GetPlayerState<APlayerState>())
	{
		PS->SetPlayerName(NewName);

		PS->OnRep_PlayerName();

		PS->ForceNetUpdate();

		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Lobby Player Name Set: %s"),
			*NewName
		);
	}
}
