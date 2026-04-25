#include "HUD/OverheadWidget.h"

#include "Components/TextBlock.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerState.h"

void UOverheadWidget::SetDisplayText(FString TextToDisplay)
{
	if (DisplayText)
	{
		DisplayText->SetText(
			FText::FromString(TextToDisplay)
		);
	}
}

void UOverheadWidget::ShowPlayerName(APawn* InPawn)
{
	if (!InPawn)
	{
		SetDisplayText(TEXT("Unknown"));
		return;
	}

	FString PlayerName = TEXT("Player");

	APlayerState* PS = InPawn->GetPlayerState();

	if (PS)
	{
		const FString ReplicatedName =
			PS->GetPlayerName();

		if (!ReplicatedName.IsEmpty())
		{
			PlayerName = ReplicatedName;
		}
	}

	SetDisplayText(PlayerName);
}

void UOverheadWidget::ShowPlayerNetRole(APawn* InPawn)
{
	if (!InPawn)
	{
		SetDisplayText(TEXT("No Pawn"));
		return;
	}

	FString Role;

	switch (InPawn->GetRemoteRole())
	{
	case ROLE_Authority:
		Role = TEXT("Authority");
		break;

	case ROLE_AutonomousProxy:
		Role = TEXT("Autonomous Proxy");
		break;

	case ROLE_SimulatedProxy:
		Role = TEXT("Simulated Proxy");
		break;

	case ROLE_None:
	default:
		Role = TEXT("None");
		break;
	}

	FString RoleString =
		FString::Printf(
			TEXT("Remote Role: %s"),
			*Role
		);

	SetDisplayText(RoleString);
}

void UOverheadWidget::NativeDestruct()
{
	Super::NativeDestruct();
}