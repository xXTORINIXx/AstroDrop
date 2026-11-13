// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/AstroDropPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "AstroDrop/AstroDrop.h"
#include "Blueprint/UserWidget.h"
#include "Character/AstroDropCharacter.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"
#include "HUD/AstroDropHUD.h"
#include "HUD/CharacterOverlay.h"
#include "Widgets/Input/SVirtualJoystick.h"


void AAstroDropPlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	AstroDropHUD = AstroDropHUD == nullptr ? Cast<AAstroDropHUD>(GetHUD()) : AstroDropHUD;

	bool bHUDValid = AstroDropHUD && 
		AstroDropHUD->CharacterOverlay && 
		AstroDropHUD->CharacterOverlay->HealthBar && 
		AstroDropHUD->CharacterOverlay->HealthText;
	if (bHUDValid)
	{
		const float HealthPercent = Health / MaxHealth;
		AstroDropHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
		FString HealthText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
		AstroDropHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
}

void AAstroDropPlayerController::SetHUDScore(float Score)
{
	AstroDropHUD = AstroDropHUD == nullptr ? Cast<AAstroDropHUD>(GetHUD()) : AstroDropHUD;
	bool bHUDValid = AstroDropHUD &&
		AstroDropHUD->CharacterOverlay &&
		AstroDropHUD->CharacterOverlay->ScoreAmount;
	if (bHUDValid)
	{
		FString ScoreText = FString::Printf(TEXT("%d"), FMath::FloorToInt(Score));
		AstroDropHUD->CharacterOverlay->ScoreAmount->SetText(FText::FromString(ScoreText));
	}
}

void AAstroDropPlayerController::SetHUDDefeats(int32 Defeats)
{
	AstroDropHUD = AstroDropHUD == nullptr ? Cast<AAstroDropHUD>(GetHUD()) : AstroDropHUD;
	bool bHUDValid = AstroDropHUD &&
		AstroDropHUD->CharacterOverlay &&
		AstroDropHUD->CharacterOverlay->DefeatsAmount;
	if (bHUDValid)
	{
		FString DefeatsText = FString::Printf(TEXT("%d"), Defeats);
		AstroDropHUD->CharacterOverlay->DefeatsAmount->SetText(FText::FromString(DefeatsText));
	}
}

void AAstroDropPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	AAstroDropCharacter* AstroDropCharacter = Cast<AAstroDropCharacter>(InPawn);
	if (AstroDropCharacter)
	{
		SetHUDHealth(AstroDropCharacter->GetHealth(), AstroDropCharacter->GetMaxHealth());
	}
}

void AAstroDropPlayerController::ClientSetEOSNickname_Implementation(const FString& Nickname)
{
	if (PlayerState)
	{
		PlayerState->SetPlayerName(Nickname);

		PlayerState->OnRep_PlayerName();

		PlayerState->ForceNetUpdate();

		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Nickname synced: %s"),
			*Nickname
		);
	}
}

void AAstroDropPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	if (PlayerState)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("PlayerState Name: %s"),
			*PlayerState->GetPlayerName()
		);
	}

	if (ShouldUseTouchControls() && IsLocalPlayerController())
	{
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

		if (MobileControlsWidget)
		{
			MobileControlsWidget->AddToPlayerScreen(0);
		}
		else
		{
			UE_LOG(LogTeste, Error, TEXT("Could not spawn mobile controls widget."));
		}
	}
	
	AstroDropHUD = Cast<AAstroDropHUD>(GetHUD());
}

void AAstroDropPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (IsLocalPlayerController())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			if (!ShouldUseTouchControls())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}
	}
}

bool AAstroDropPlayerController::ShouldUseTouchControls() const
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		return false;
	}
	return SVirtualJoystick::ShouldDisplayTouchInterface() || bForceTouchControls;
}
