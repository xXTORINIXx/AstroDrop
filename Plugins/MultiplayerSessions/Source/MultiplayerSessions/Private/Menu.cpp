#include "Menu.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/EditableTextBox.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"

#define LOCTEXT_NAMESPACE "Menu"

bool UMenu::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	if (PlayButton)
	{
		PlayButton->OnClicked.AddDynamic(this, &ThisClass::PlayButtonClicked);
	}

	if (HostButton)
	{
		// Compatibilidade: no WBP antigo, Host vira PLAY / Matchmaking BR.
		HostButton->OnClicked.AddDynamic(this, &ThisClass::HostButtonClicked);
	}

	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClicked);
	}

	if (InviteButton)
	{
		InviteButton->OnClicked.AddDynamic(this, &ThisClass::InviteButtonClicked);
	}

	if (ReadyButton)
	{
		ReadyButton->OnClicked.AddDynamic(this, &ThisClass::ReadyButtonClicked);
	}

	if (LeaveButton)
	{
		LeaveButton->OnClicked.AddDynamic(this, &ThisClass::LeaveButtonClicked);
	}

	if (ToggleVoiceButton)
	{
		ToggleVoiceButton->OnClicked.AddDynamic(this, &ThisClass::ToggleVoiceButtonClicked);
	}

	if (SendChatButton)
	{
		SendChatButton->OnClicked.AddDynamic(this, &ThisClass::SendChatButtonClicked);
	}

	if (RefreshLobbyButton)
	{
		RefreshLobbyButton->OnClicked.AddDynamic(this, &ThisClass::RefreshLobbyButtonClicked);
	}

	bMenuInitialized = true;
	return true;
}

void UMenu::MenuSetup(int32 NumberOfPublicConnections, FString TypeOfMatch, FString LobbyPath)
{
	PathToLobby = FString::Printf(TEXT("%s?listen"), *LobbyPath);
	NumPublicConnections = NumberOfPublicConnections;
	MatchType = TypeOfMatch;

	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	SetIsFocusable(true);

	if (UWorld* World = GetWorld())
	{
		if (APlayerController* PC = World->GetFirstPlayerController())
		{
			FInputModeUIOnly Input;
			Input.SetWidgetToFocus(TakeWidget());
			Input.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PC->SetInputMode(Input);
			PC->SetShowMouseCursor(true);
		}
	}

	SetButtonsEnabled(false);
	SetLobbyActionButtonsEnabled(false);
	UpdateStatus(LOCTEXT("Connecting", "Conectando ao EOS..."));

	if (UGameInstance* GI = GetGameInstance())
	{
		MultiplayerSessionsSubsystem = GI->GetSubsystem<UMultiplayerSessionsSubsystem>();
	}

	if (!MultiplayerSessionsSubsystem)
	{
		UpdateStatus(LOCTEXT("SubsystemMissing", "Erro: MultiplayerSessionsSubsystem não encontrado"));
		return;
	}

	BindSubsystemDelegates();

	if (MultiplayerSessionsSubsystem->IsLoggedIn())
	{
		bTriedPortalLogin = false;
		UpdateStatus(LOCTEXT("AlreadyConnected", "Conectado ao EOS"));
		MultiplayerSessionsSubsystem->CreatePersonalParty();
		SetButtonsEnabled(true);
		RefreshHeaderPanel();
		RefreshPartyPanel();
		RefreshLobbyPanel();
		RefreshChatPanel();
		return;
	}

	UpdateStatus(LOCTEXT("AutoLogin", "Autenticando..."));
	MultiplayerSessionsSubsystem->AutoLoginForCurrentRuntime();
}

void UMenu::BindSubsystemDelegates()
{
	if (!MultiplayerSessionsSubsystem)
	{
		return;
	}

	UnbindSubsystemDelegates();

	MultiplayerSessionsSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
	MultiplayerSessionsSubsystem->MultiplayerOnFindSessionsComplete.AddUObject(this, &ThisClass::OnFindSessions);
	MultiplayerSessionsSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSession);
	MultiplayerSessionsSubsystem->MultiplayerOnLoginComplete.AddDynamic(this, &ThisClass::OnLoginComplete);
	MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
	MultiplayerSessionsSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSession);
	MultiplayerSessionsSubsystem->MultiplayerOnLobbyUpdated.AddDynamic(this, &ThisClass::OnLobbyUpdated);
	MultiplayerSessionsSubsystem->MultiplayerOnPartyUpdated.AddDynamic(this, &ThisClass::OnPartyUpdated);
	MultiplayerSessionsSubsystem->MultiplayerOnChatUpdated.AddDynamic(this, &ThisClass::OnChatUpdated);
	MultiplayerSessionsSubsystem->MultiplayerOnVoiceEnabledChanged.AddDynamic(this, &ThisClass::OnVoiceEnabledChanged);
	MultiplayerSessionsSubsystem->MultiplayerOnReadyStateChanged.AddDynamic(this, &ThisClass::OnReadyStateChanged);
	MultiplayerSessionsSubsystem->MultiplayerOnConnectionStateChanged.AddDynamic(this, &ThisClass::OnConnectionStateChanged);
}

void UMenu::UnbindSubsystemDelegates()
{
	if (!MultiplayerSessionsSubsystem)
	{
		return;
	}

	MultiplayerSessionsSubsystem->MultiplayerOnCreateSessionComplete.RemoveDynamic(this, &ThisClass::OnCreateSession);
	MultiplayerSessionsSubsystem->MultiplayerOnFindSessionsComplete.RemoveAll(this);
	MultiplayerSessionsSubsystem->MultiplayerOnJoinSessionComplete.RemoveAll(this);
	MultiplayerSessionsSubsystem->MultiplayerOnLoginComplete.RemoveDynamic(this, &ThisClass::OnLoginComplete);
	MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.RemoveDynamic(this, &ThisClass::OnDestroySession);
	MultiplayerSessionsSubsystem->MultiplayerOnStartSessionComplete.RemoveDynamic(this, &ThisClass::OnStartSession);
	MultiplayerSessionsSubsystem->MultiplayerOnLobbyUpdated.RemoveDynamic(this, &ThisClass::OnLobbyUpdated);
	MultiplayerSessionsSubsystem->MultiplayerOnPartyUpdated.RemoveDynamic(this, &ThisClass::OnPartyUpdated);
	MultiplayerSessionsSubsystem->MultiplayerOnChatUpdated.RemoveDynamic(this, &ThisClass::OnChatUpdated);
	MultiplayerSessionsSubsystem->MultiplayerOnVoiceEnabledChanged.RemoveDynamic(this, &ThisClass::OnVoiceEnabledChanged);
	MultiplayerSessionsSubsystem->MultiplayerOnReadyStateChanged.RemoveDynamic(this, &ThisClass::OnReadyStateChanged);
	MultiplayerSessionsSubsystem->MultiplayerOnConnectionStateChanged.RemoveDynamic(this, &ThisClass::OnConnectionStateChanged);
}

bool UMenu::CanUseSubsystem() const
{
	return MultiplayerSessionsSubsystem != nullptr;
}

bool UMenu::CanUsePartyActions() const
{
	return MultiplayerSessionsSubsystem && MultiplayerSessionsSubsystem->IsLoggedIn();
}

bool UMenu::CanUseLobbyActions() const
{
	return MultiplayerSessionsSubsystem && MultiplayerSessionsSubsystem->IsLoggedIn();
}

void UMenu::SetButtonsEnabled(bool bEnabled)
{
	if (PlayButton)
	{
		PlayButton->SetIsEnabled(bEnabled);
	}

	if (HostButton)
	{
		HostButton->SetIsEnabled(bEnabled);
	}

	if (JoinButton)
	{
		JoinButton->SetIsEnabled(bEnabled);
	}

	if (InviteButton)
	{
		InviteButton->SetIsEnabled(bEnabled);
	}

	SetLobbyActionButtonsEnabled(bEnabled);
}

void UMenu::SetLobbyActionButtonsEnabled(bool bEnabled)
{
	if (ReadyButton)
	{
		ReadyButton->SetIsEnabled(bEnabled);
	}

	if (LeaveButton)
	{
		LeaveButton->SetIsEnabled(bEnabled);
	}

	if (ToggleVoiceButton)
	{
		ToggleVoiceButton->SetIsEnabled(bEnabled);
	}

	if (SendChatButton)
	{
		SendChatButton->SetIsEnabled(bEnabled);
	}

	if (RefreshLobbyButton)
	{
		RefreshLobbyButton->SetIsEnabled(bEnabled);
	}

	if (ChatInputTextBox)
	{
		ChatInputTextBox->SetIsEnabled(bEnabled);
	}
}

void UMenu::UpdateStatus(const FText& Message)
{
	if (StatusText)
	{
		StatusText->SetText(Message);
	}
}

void UMenu::OnLoginComplete(bool bWasSuccessful, const FString& ErrorMessage)
{
	if (bWasSuccessful)
	{
		bTriedPortalLogin = false;
		SetButtonsEnabled(true);
		UpdateStatus(LOCTEXT("Connected", "Conectado ao EOS | Party criada"));
		RefreshHeaderPanel();
		RefreshPartyPanel();
		RefreshLobbyPanel();
		RefreshChatPanel();
		return;
	}

	SetButtonsEnabled(false);

	if (MultiplayerSessionsSubsystem &&
		!bTriedPortalLogin &&
		MultiplayerSessionsSubsystem->GetCurrentLoginType() != EEOSLoginType::AccountPortal)
	{
		bTriedPortalLogin = true;
		UpdateStatus(LOCTEXT("LoginFallback", "Primeiro acesso detectado... abrindo login"));
		MultiplayerSessionsSubsystem->LoginWithPortal();
		return;
	}

	UpdateStatus(FText::FromString(FString::Printf(TEXT("Falha no login: %s"), *ErrorMessage)));
}

void UMenu::PlayButtonClicked()
{
	if (!CanUsePartyActions())
	{
		UpdateStatus(LOCTEXT("NotConnectedPlay", "Não conectado ao EOS"));
		return;
	}

	SetButtonsEnabled(false);
	UpdateStatus(LOCTEXT("Matchmaking", "Buscando partida Battle Royale..."));
	MultiplayerSessionsSubsystem->StartBattleRoyaleMatchmaking(NumPublicConnections, MatchType);
}

void UMenu::HostButtonClicked()
{
	// Compatibilidade com WBP antigo: Host agora é PLAY.
	PlayButtonClicked();
}

void UMenu::JoinButtonClicked()
{
	if (!CanUseLobbyActions())
	{
		UpdateStatus(LOCTEXT("NotConnectedJoin", "Não conectado ao EOS"));
		return;
	}

	SetButtonsEnabled(false);
	UpdateStatus(LOCTEXT("SearchingSession", "Buscando lobbies de staging..."));
	MultiplayerSessionsSubsystem->FindSessions(10000);
}

void UMenu::InviteButtonClicked()
{
	if (!CanUseSubsystem())
	{
		return;
	}

	UpdateStatus(LOCTEXT("InviteOpening", "Abrindo convite EOS..."));
	MultiplayerSessionsSubsystem->OpenSessionInviteUI();
}

void UMenu::ReadyButtonClicked()
{
	if (!CanUsePartyActions())
	{
		return;
	}

	MultiplayerSessionsSubsystem->SetPartyReady(!MultiplayerSessionsSubsystem->IsLocalPlayerReady());
}

void UMenu::LeaveButtonClicked()
{
	if (!CanUseSubsystem())
	{
		return;
	}

	MultiplayerSessionsSubsystem->LeaveLobby();
	UpdateStatus(LOCTEXT("LeftLobby", "Você saiu do lobby"));
	SetButtonsEnabled(MultiplayerSessionsSubsystem->IsLoggedIn());
	RefreshPartyPanel();
	RefreshLobbyPanel();
	RefreshChatPanel();
}

void UMenu::ToggleVoiceButtonClicked()
{
	if (!CanUseLobbyActions())
	{
		return;
	}

	MultiplayerSessionsSubsystem->ToggleLocalVoiceEnabled();
}

void UMenu::SendChatButtonClicked()
{
	if (!CanUseLobbyActions() || !ChatInputTextBox)
	{
		return;
	}

	const FString Message = ChatInputTextBox->GetText().ToString().TrimStartAndEnd();
	if (Message.IsEmpty())
	{
		return;
	}

	MultiplayerSessionsSubsystem->SendLobbyChatMessage(Message);
	ChatInputTextBox->SetText(FText::GetEmpty());
}

void UMenu::RefreshLobbyButtonClicked()
{
	if (!CanUseLobbyActions())
	{
		return;
	}

	MultiplayerSessionsSubsystem->RefreshLobbyData();
}

void UMenu::OnCreateSession(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UpdateStatus(LOCTEXT("SessionCreated", "Staging lobby criado!"));

		if (UWorld* World = GetWorld())
		{
			World->ServerTravel(PathToLobby);
			return;
		}
	}

	UpdateStatus(LOCTEXT("CreateFail", "Falha ao criar staging lobby"));
	SetButtonsEnabled(true);
}

void UMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& Results, bool bWasSuccessful)
{
	if (!CanUseLobbyActions())
	{
		UpdateStatus(LOCTEXT("NotConnectedSearch", "Não conectado ao EOS"));
		SetButtonsEnabled(false);
		return;
	}

	if (!bWasSuccessful || Results.Num() == 0)
	{
		UpdateStatus(LOCTEXT("NoSession", "Nenhum lobby encontrado"));
		SetButtonsEnabled(true);
		return;
	}

	UpdateStatus(LOCTEXT("SessionFound", "Lobby encontrado, conectando..."));

	for (const FOnlineSessionSearchResult& Result : Results)
	{
		FString FoundMatchType;
		Result.Session.SessionSettings.Get(FName("MatchType"), FoundMatchType);

		if (FoundMatchType.IsEmpty() || FoundMatchType == MatchType)
		{
			MultiplayerSessionsSubsystem->JoinSession(Result);
			return;
		}
	}

	UpdateStatus(LOCTEXT("MatchTypeNotFound", "Lobby encontrado, mas com playlist diferente"));
	SetButtonsEnabled(true);
}

void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		UpdateStatus(LOCTEXT("JoinFail", "Falha ao conectar"));
		SetButtonsEnabled(true);
		return;
	}

	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (!Subsystem)
	{
		UpdateStatus(LOCTEXT("JoinFailNoSubsystem", "Falha ao conectar: OnlineSubsystem inválido"));
		SetButtonsEnabled(true);
		return;
	}

	IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
	if (!Session.IsValid())
	{
		UpdateStatus(LOCTEXT("JoinFailNoSession", "Falha ao conectar: SessionInterface inválida"));
		SetButtonsEnabled(true);
		return;
	}

	FString Address;
	if (!Session->GetResolvedConnectString(NAME_GameSession, Address) || Address.IsEmpty())
	{
		UpdateStatus(LOCTEXT("JoinFailNoAddress", "Falha ao conectar: endereço inválido"));
		SetButtonsEnabled(true);
		return;
	}

	if (APlayerController* PC = GetGameInstance() ? GetGameInstance()->GetFirstLocalPlayerController() : nullptr)
	{
		UpdateStatus(LOCTEXT("Joined", "Conectado ao staging lobby"));
		PC->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
		return;
	}

	UpdateStatus(LOCTEXT("JoinFailNoPC", "Falha ao conectar: PlayerController inválido"));
	SetButtonsEnabled(true);
}

void UMenu::OnDestroySession(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UpdateStatus(LOCTEXT("Destroyed", "Sessão destruída"));
		OnConnectionStateChanged(MultiplayerSessionsSubsystem && MultiplayerSessionsSubsystem->IsInParty()
			? EMultiplayerConnectionState::InParty
			: EMultiplayerConnectionState::LoggedIn);
		return;
	}

	UpdateStatus(LOCTEXT("DestroyFail", "Falha ao destruir sessão"));
}

void UMenu::OnStartSession(bool bWasSuccessful)
{
	UpdateStatus(
		bWasSuccessful
			? LOCTEXT("Started", "Partida iniciada")
			: LOCTEXT("StartFail", "Falha ao iniciar sessão")
	);
}

void UMenu::OnLobbyUpdated()
{
	RefreshLobbyPanel();
}

void UMenu::OnPartyUpdated()
{
	RefreshPartyPanel();
}

void UMenu::OnChatUpdated()
{
	RefreshChatPanel();
}

void UMenu::OnVoiceEnabledChanged(bool bVoiceEnabled)
{
	if (VoiceStateText)
	{
		VoiceStateText->SetText(
			bVoiceEnabled
				? LOCTEXT("VoiceOn", "Voz: Ativa")
				: LOCTEXT("VoiceOff", "Voz: Mutada")
		);

		VoiceStateText->SetColorAndOpacity(
			bVoiceEnabled ? FSlateColor(FLinearColor::Green) : FSlateColor(FLinearColor::Red)
		);
	}
}

void UMenu::OnReadyStateChanged(bool bReady)
{
	if (ReadyStateText)
	{
		ReadyStateText->SetText(
			bReady
				? LOCTEXT("ReadyYes", "Pronto: Sim")
				: LOCTEXT("ReadyNo", "Pronto: Não")
		);

		ReadyStateText->SetColorAndOpacity(
			bReady ? FSlateColor(FLinearColor(0.1f, 0.8f, 0.2f, 1.f)) : FSlateColor(FLinearColor::Gray)
		);
	}

	RefreshPartyPanel();
}

void UMenu::OnConnectionStateChanged(EMultiplayerConnectionState NewState)
{
	if (LobbyStateText)
	{
		LobbyStateText->SetText(BuildConnectionStateText(NewState));
	}

	const bool bCanUseMainActions =
		(NewState == EMultiplayerConnectionState::LoggedIn) ||
		(NewState == EMultiplayerConnectionState::InParty) ||
		(NewState == EMultiplayerConnectionState::InStagingLobby) ||
		(NewState == EMultiplayerConnectionState::InLobby);

	SetButtonsEnabled(bCanUseMainActions);
}

FText UMenu::BuildConnectionStateText(EMultiplayerConnectionState State) const
{
	switch (State)
	{
	case EMultiplayerConnectionState::Disconnected:
		return LOCTEXT("ConnDisconnected", "Estado: Desconectado");
	case EMultiplayerConnectionState::Connecting:
		return LOCTEXT("ConnConnecting", "Estado: Conectando");
	case EMultiplayerConnectionState::LoggedIn:
		return LOCTEXT("ConnLogged", "Estado: Logado");
	case EMultiplayerConnectionState::InParty:
		return LOCTEXT("ConnParty", "Estado: Em Party");
	case EMultiplayerConnectionState::Matchmaking:
		return LOCTEXT("ConnMatchmaking", "Estado: Buscando Partida");
	case EMultiplayerConnectionState::InStagingLobby:
		return LOCTEXT("ConnStaging", "Estado: Staging Lobby");
	case EMultiplayerConnectionState::InLobby:
		return LOCTEXT("ConnLobby", "Estado: Em Lobby");
	case EMultiplayerConnectionState::InMatch:
		return LOCTEXT("ConnMatch", "Estado: Em Partida");
	default:
		return LOCTEXT("ConnUnknown", "Estado: Desconhecido");
	}
}

void UMenu::RefreshHeaderPanel()
{
	if (!MultiplayerSessionsSubsystem)
	{
		return;
	}

	if (PlayerNameText)
	{
		const FString NameToShow = MultiplayerSessionsSubsystem->IsLoggedIn()
			? MultiplayerSessionsSubsystem->GetPlayerName()
			: TEXT("Offline");

		PlayerNameText->SetText(FText::FromString(NameToShow));
	}

	OnConnectionStateChanged(MultiplayerSessionsSubsystem->GetConnectionState());

	if (MultiplayerSessionsSubsystem->IsLoggedIn())
	{
		OnReadyStateChanged(MultiplayerSessionsSubsystem->IsLocalPlayerReady());
		OnVoiceEnabledChanged(MultiplayerSessionsSubsystem->IsLocalVoiceEnabled());
	}
	else
	{
		if (ReadyStateText)
		{
			ReadyStateText->SetText(LOCTEXT("ReadyOffline", "Pronto: -"));
			ReadyStateText->SetColorAndOpacity(FSlateColor(FLinearColor::Gray));
		}

		if (VoiceStateText)
		{
			VoiceStateText->SetText(LOCTEXT("VoiceOffline", "Voz: -"));
			VoiceStateText->SetColorAndOpacity(FSlateColor(FLinearColor::Gray));
		}
	}
}

void UMenu::RefreshPartyPanel()
{
	if (!MultiplayerSessionsSubsystem)
	{
		return;
	}

	const TArray<FMultiplayerPartyMemberInfo>& PartyMembers = MultiplayerSessionsSubsystem->GetPartyMembers();

	FString Buffer;
	for (const FMultiplayerPartyMemberInfo& Member : PartyMembers)
	{
		Buffer += FString::Printf(
			TEXT("%s%s%s | %s\n"),
			Member.bIsLocalPlayer ? TEXT("👉 ") : TEXT(""),
			*Member.PlayerName,
			Member.bIsLeader ? TEXT(" 👑") : TEXT(""),
			Member.bIsReady ? TEXT("🟢 Ready") : TEXT("⚪ Not Ready")
		);
	}

	if (Buffer.IsEmpty())
	{
		Buffer = TEXT("Party vazia. Faça login para criar sua party.");
	}

	if (PartyListText)
	{
		PartyListText->SetText(FText::FromString(Buffer));
	}
	else if (PlayerListText && MultiplayerSessionsSubsystem->GetLobbyPlayers().Num() == 0)
	{
		// Compatibilidade com WBP antigo: usa PlayerListText para exibir party no menu principal.
		PlayerListText->SetText(FText::FromString(Buffer));
	}
}

void UMenu::RefreshLobbyPanel()
{
	RefreshHeaderPanel();

	if (!PlayerListText || !MultiplayerSessionsSubsystem)
	{
		return;
	}

	const TArray<FMultiplayerLobbyPlayerInfo>& Players = MultiplayerSessionsSubsystem->GetLobbyPlayers();
	if (Players.Num() == 0)
	{
		RefreshPartyPanel();
		return;
	}

	FString Buffer;
	for (const FMultiplayerLobbyPlayerInfo& Player : Players)
	{
		Buffer += FString::Printf(
			TEXT("%s%s | %s | %s\n"),
			Player.bIsLocalPlayer ? TEXT("👉 ") : TEXT(""),
			*Player.PlayerName,
			Player.bIsReady ? TEXT("🟢 Ready") : TEXT("⚪ Not Ready"),
			Player.bVoiceEnabled ? TEXT("🎤 Voice On") : TEXT("🔇 Voice Off")
		);
	}

	PlayerListText->SetText(FText::FromString(Buffer));
}

void UMenu::RefreshChatPanel()
{
	if (!ChatHistoryText || !MultiplayerSessionsSubsystem)
	{
		return;
	}

	FString Buffer;
	const TArray<FMultiplayerChatMessage>& Messages = MultiplayerSessionsSubsystem->GetChatMessages();

	const int32 MaxMessagesToShow = 50;
	const int32 StartIndex = FMath::Max(0, Messages.Num() - MaxMessagesToShow);

	for (int32 Index = StartIndex; Index < Messages.Num(); ++Index)
	{
		const FMultiplayerChatMessage& Chat = Messages[Index];
		Buffer += FString::Printf(
			TEXT("[%s] %s: %s\n"),
			*Chat.Timestamp,
			*Chat.SenderName,
			*Chat.Message
		);
	}

	if (Buffer.IsEmpty())
	{
		Buffer = TEXT("Chat vazio.");
	}

	ChatHistoryText->SetText(FText::FromString(Buffer));
}

void UMenu::NativeDestruct()
{
	UnbindSubsystemDelegates();
	MenuTearDown();
	Super::NativeDestruct();
}

void UMenu::MenuTearDown()
{
	RemoveFromParent();

	if (UWorld* World = GetWorld())
	{
		if (APlayerController* PC = World->GetFirstPlayerController())
		{
			FInputModeGameOnly Input;
			PC->SetInputMode(Input);
			PC->SetShowMouseCursor(false);
		}
	}
}

#undef LOCTEXT_NAMESPACE
