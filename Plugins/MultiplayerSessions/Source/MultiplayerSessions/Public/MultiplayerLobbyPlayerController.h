#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MultiplayerLobbyPlayerController.generated.h"

UCLASS()
class MULTIPLAYERSESSIONS_API AMultiplayerLobbyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void ServerSendLobbyChatMessage(const FString& Message);

	UFUNCTION(Server, Reliable)
	void ServerSetLobbyReadyState(bool bNewReadyState);

	UFUNCTION(Server, Reliable)
	void ServerSetLobbyVoiceEnabled(bool bNewVoiceEnabled);

	UFUNCTION(Server, Reliable)
	void ServerSetLobbyPlayerName(const FString& NewName);

protected:
	UFUNCTION()
	void HandleSubsystemLoginComplete(bool bWasSuccessful, const FString& Error);
};
