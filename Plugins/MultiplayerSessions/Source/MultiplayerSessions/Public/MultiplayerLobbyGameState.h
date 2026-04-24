#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MultiplayerLobbyTypes.h"
#include "MultiplayerLobbyGameState.generated.h"

UCLASS()
class MULTIPLAYERSESSIONS_API AMultiplayerLobbyGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AMultiplayerLobbyGameState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	void AddChatMessage(const FMultiplayerChatMessage& NewMessage);

	UFUNCTION(BlueprintPure)
	const TArray<FMultiplayerChatMessage>& GetRecentMessages() const { return RecentMessages; }

protected:
	UPROPERTY(ReplicatedUsing = OnRep_RecentMessages, BlueprintReadOnly)
	TArray<FMultiplayerChatMessage> RecentMessages;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lobby|Chat")
	int32 MaxChatMessages = 50;

	UFUNCTION()
	void OnRep_RecentMessages();
};
