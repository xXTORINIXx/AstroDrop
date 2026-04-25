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
	
};