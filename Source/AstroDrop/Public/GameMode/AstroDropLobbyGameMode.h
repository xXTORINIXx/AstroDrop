#pragma once

#include "CoreMinimal.h"
#include "MultiplayerLobbyGameMode.h"
#include "AstroDropLobbyGameMode.generated.h"

UCLASS()
class ASTRODROP_API AAstroDropLobbyGameMode
	: public AMultiplayerLobbyGameMode
{
	GENERATED_BODY()

public:
	AAstroDropLobbyGameMode();

	virtual void PostLogin(
		APlayerController* NewPlayer
	) override;

	void PlayerReadyChanged();

protected:
	void CheckStartMatch();

	UPROPERTY(EditDefaultsOnly, Category="Match")
	int32 PlayersToStartMatch = 2;

	UPROPERTY(EditDefaultsOnly, Category="Match")
	FString MatchMapPath =
		TEXT("/Game/Maps/AstroDropMap");

	UPROPERTY()
	bool bMatchStarting = false;
};