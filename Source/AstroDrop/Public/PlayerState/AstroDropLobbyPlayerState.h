#pragma once

#include "CoreMinimal.h"
#include "MultiplayerLobbyPlayerState.h"
#include "AstroDropLobbyPlayerState.generated.h"

UCLASS()
class ASTRODROP_API AAstroDropLobbyPlayerState
	: public AMultiplayerLobbyPlayerState
{
	GENERATED_BODY()

public:
	virtual void SetReadyState(bool bInReady) override;
};