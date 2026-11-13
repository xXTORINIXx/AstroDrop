// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/AstroDropGameMode.h"

#include "Character/AstroDropCharacter.h"
#include "GameFramework/PlayerStart.h"
#include "HUD/AstroDropHUD.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerController/AstroDropPlayerController.h"
#include "PlayerState/AstroDropPlayerState.h"


AAstroDropGameMode::AAstroDropGameMode()
{
	PlayerControllerClass = AAstroDropPlayerController::StaticClass();
	PlayerStateClass = AAstroDropPlayerState::StaticClass();
	HUDClass = AAstroDropHUD::StaticClass();
}

void AAstroDropGameMode::PlayerEliminated(class AAstroDropCharacter* ElimmedCharacter,
	class AAstroDropPlayerController* VictimController, AAstroDropPlayerController* AttackerController)
{
	if (AttackerController == nullptr || AttackerController->PlayerState == nullptr) return;
	if (VictimController == nullptr || VictimController->PlayerState == nullptr) return;
	AAstroDropPlayerState* AttackerPlayerState = AttackerController ? Cast<AAstroDropPlayerState>(AttackerController->PlayerState) : nullptr;
	AAstroDropPlayerState* VictimPlayerState = VictimController ? Cast<AAstroDropPlayerState>(VictimController->PlayerState) : nullptr;

	if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState)
	{
		AttackerPlayerState->AddToScore(1.f);
	}
	if (VictimPlayerState)
	{
		VictimPlayerState->AddToDefeats(1);
	}
	
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Elim();
	}
}

void AAstroDropGameMode::RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController)
{
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Reset();
		ElimmedCharacter->Destroy();
	}
	if (ElimmedController)
	{
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);
		RestartPlayerAtPlayerStart(ElimmedController, PlayerStarts[Selection]);
	}
}


