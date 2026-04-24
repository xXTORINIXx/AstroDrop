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
};
