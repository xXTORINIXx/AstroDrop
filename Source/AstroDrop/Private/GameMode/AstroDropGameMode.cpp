// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/AstroDropGameMode.h"

#include "HUD/AstroDropHUD.h"
#include "PlayerController/AstroDropPlayerController.h"


AAstroDropGameMode::AAstroDropGameMode()
{
	PlayerControllerClass = AAstroDropPlayerController::StaticClass();
	HUDClass = AAstroDropHUD::StaticClass();
}

void AAstroDropGameMode::PlayerEliminated(class AAstroDropCharacter* ElimmedCharacter,
	class AAstroDropPlayerController* VictimController, AAstroDropPlayerController* AttackerController)
{
}


