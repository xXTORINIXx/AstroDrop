// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/AstroDropGameMode.h"

#include "Character/AstroDropCharacter.h"
#include "GameFramework/PlayerStart.h"
#include "HUD/AstroDropHUD.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerController/AstroDropPlayerController.h"


AAstroDropGameMode::AAstroDropGameMode()
{
	PlayerControllerClass = AAstroDropPlayerController::StaticClass();
	HUDClass = AAstroDropHUD::StaticClass();
}

void AAstroDropGameMode::PlayerEliminated(class AAstroDropCharacter* ElimmedCharacter,
	class AAstroDropPlayerController* VictimController, AAstroDropPlayerController* AttackerController)
{
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


