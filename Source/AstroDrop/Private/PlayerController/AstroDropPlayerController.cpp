// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/AstroDropPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "AstroDrop/AstroDrop.h"
#include "Blueprint/UserWidget.h"
#include "Widgets/Input/SVirtualJoystick.h"


void AAstroDropPlayerController::BeginPlay()
{
	Super::BeginPlay();

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
}

void AAstroDropPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (IsLocalPlayerController())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
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
	return SVirtualJoystick::ShouldDisplayTouchInterface() || bForceTouchControls;
}



