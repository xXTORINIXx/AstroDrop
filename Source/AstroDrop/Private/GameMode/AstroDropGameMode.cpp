// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/AstroDropGameMode.h"
#include "PlayerController/AstroDropPlayerController.h"


AAstroDropGameMode::AAstroDropGameMode()
{
	PlayerControllerClass = AAstroDropPlayerController::StaticClass();
}
