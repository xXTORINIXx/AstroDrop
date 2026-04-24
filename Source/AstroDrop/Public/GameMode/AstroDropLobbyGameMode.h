// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MultiplayerLobbyGameMode.h"
#include "AstroDropLobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ASTRODROP_API AAstroDropLobbyGameMode : public AMultiplayerLobbyGameMode
{
	GENERATED_BODY()
public:
	AAstroDropLobbyGameMode();
	virtual void PostLogin(APlayerController* NewPlayer) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Lobby")
	int32 PlayersToStartMatch = 2;

	UPROPERTY(EditDefaultsOnly, Category = "Lobby")
	FString MatchMapPath = TEXT("/Game/Maps/BlasterMap");
		
};
