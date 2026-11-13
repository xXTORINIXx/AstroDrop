// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerState/AstroDropPlayerState.h"

#include "Character/AstroDropCharacter.h"
#include "Net/UnrealNetwork.h"
#include "PlayerController/AstroDropPlayerController.h"


void AAstroDropPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAstroDropPlayerState, Defeats);
}

void AAstroDropPlayerState::AddToScore(float ScoreAmount)
{
	SetScore(GetScore() + ScoreAmount);
	Character = Cast<AAstroDropCharacter>(GetPawn());
	if (Character && Character->Controller)
	{
		Controller = Controller == nullptr ? Cast<AAstroDropPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDScore(GetScore());
		}
	}
}

void AAstroDropPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	Character = Cast<AAstroDropCharacter>(GetPawn());
	if (Character && Character->Controller)
	{
		Controller = Controller == nullptr ? Cast<AAstroDropPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDScore(GetScore());
		}
	}
}

void AAstroDropPlayerState::AddToDefeats(int32 DefeatsAmount)
{
	Defeats += DefeatsAmount;
	Character = Cast<AAstroDropCharacter>(GetPawn());
	if (Character && Character->Controller)
	{
		Controller = Controller == nullptr ? Cast<AAstroDropPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDDefeats(Defeats);
		}
	}
}

void AAstroDropPlayerState::OnRep_Defeats()
{
	Character = Cast<AAstroDropCharacter>(GetPawn());
	if (Character && Character->Controller)
	{
		Controller = Controller == nullptr ? Cast<AAstroDropPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDDefeats(Defeats);
		}
	}
}

