#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MultiplayerLobbyGameMode.generated.h"

UCLASS()
class MULTIPLAYERSESSIONS_API AMultiplayerLobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMultiplayerLobbyGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

	UFUNCTION(BlueprintCallable)
	void PlayerReadyChanged();

protected:
	UFUNCTION(BlueprintCallable)
	void CheckStartMatch();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Battle Royale")
	int32 PlayersToStartMatch = 2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Battle Royale")
	FString MatchMapPath = TEXT("/Game/Maps/AstroDropMap");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Battle Royale")
	bool bAutoStartWhenAllReady = true;

	UPROPERTY()
	bool bMatchStarting = false;
};
