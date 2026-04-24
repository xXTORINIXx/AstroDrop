#include "MultiplayerSessionsSubsystem.h"
#include "MultiplayerLobbyGameState.h"
#include "MultiplayerLobbyPlayerController.h"
#include "MultiplayerLobbyPlayerState.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"
#include "Interfaces/VoiceInterface.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Misc/CommandLine.h"
#include "HAL/IConsoleManager.h"

UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem()
	: CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete))
	, FindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsComplete))
	, JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete))
	, DestroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionComplete))
	, StartSessionCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnStartSessionComplete))
	, LoginCompleteDelegate(FOnLoginCompleteDelegate::CreateUObject(this, &ThisClass::OnLoginComplete))
{
}

void UMultiplayerSessionsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	LobbyTickerHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &ThisClass::HandleLobbyTicker), 0.5f);
}

void UMultiplayerSessionsSubsystem::Deinitialize()
{
	if (LobbyTickerHandle.IsValid())
	{
		FTSTicker::GetCoreTicker().RemoveTicker(LobbyTickerHandle);
		LobbyTickerHandle.Reset();
	}

	Super::Deinitialize();
}

bool UMultiplayerSessionsSubsystem::IsUsingEOS() const
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	return Subsystem && Subsystem->GetSubsystemName() == FName("EOS");
}

bool UMultiplayerSessionsSubsystem::IsRunningDedicatedServerInstance() const
{
	return IsRunningDedicatedServer();
}

bool UMultiplayerSessionsSubsystem::IsValidSessionInterface()
{
	if (!SessionInterface.IsValid())
	{
		if (IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get())
		{
			SessionInterface = Subsystem->GetSessionInterface();
		}
	}

	return SessionInterface.IsValid();
}

void UMultiplayerSessionsSubsystem::SetConnectionState(EMultiplayerConnectionState NewState)
{
	if (ConnectionState == NewState)
	{
		return;
	}

	ConnectionState = NewState;
	MultiplayerOnConnectionStateChanged.Broadcast(ConnectionState);
}

void UMultiplayerSessionsSubsystem::LoginWithPortal()
{
	Login(EEOSLoginType::AccountPortal);
}

void UMultiplayerSessionsSubsystem::LoginWithPersistentAuth()
{
	Login(EEOSLoginType::PersistentAuth);
}

void UMultiplayerSessionsSubsystem::LoginDedicatedServer()
{
	Login(EEOSLoginType::DedicatedServer);
}

void UMultiplayerSessionsSubsystem::AutoLoginForCurrentRuntime()
{
	if (IsRunningDedicatedServerInstance())
	{
		LoginDedicatedServer();
		return;
	}

	LoginWithPersistentAuth();
	//LoginWithPortal();
}

FString UMultiplayerSessionsSubsystem::GetPlayerName() const
{
	if (const UWorld* World = GetWorld())
	{
		if (const APlayerController* PC = World->GetFirstPlayerController())
		{
			if (const APlayerState* PS = PC->PlayerState)
			{
				if (!PS->GetPlayerName().IsEmpty())
				{
					return PS->GetPlayerName();
				}
			}
		}
	}

	return LoggedInUserId.IsEmpty() ? TEXT("Unknown") : LoggedInUserId;
}

FString UMultiplayerSessionsSubsystem::BuildDedicatedServerAddress() const
{
	FString Value;
	if (FParse::Value(FCommandLine::Get(), TEXT("EOSDevAuthHost="), Value))
	{
		return Value;
	}

	return TEXT("127.0.0.1:8081");
}

void UMultiplayerSessionsSubsystem::Login(EEOSLoginType LoginType)
{
	SetConnectionState(EMultiplayerConnectionState::Connecting);

	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (!Subsystem)
	{
		bIsLoggedIn = false;
		LoggedInUserId.Empty();
		SetConnectionState(EMultiplayerConnectionState::Disconnected);
		MultiplayerOnLoginComplete.Broadcast(false, TEXT("OnlineSubsystem não encontrado."));
		return;
	}

	IdentityInterface = Subsystem->GetIdentityInterface();
	if (!IdentityInterface.IsValid())
	{
		bIsLoggedIn = false;
		LoggedInUserId.Empty();
		SetConnectionState(EMultiplayerConnectionState::Disconnected);
		MultiplayerOnLoginComplete.Broadcast(false, TEXT("IdentityInterface inválida."));
		return;
	}

	const ELoginStatus::Type CurrentStatus = IdentityInterface->GetLoginStatus(0);
	if (CurrentStatus == ELoginStatus::LoggedIn)
	{
		TSharedPtr<const FUniqueNetId> ExistingUserId = IdentityInterface->GetUniquePlayerId(0);
		bIsLoggedIn = ExistingUserId.IsValid();

		if (ExistingUserId.IsValid())
		{
			LoggedInUserId = ExistingUserId->ToString();
			SetConnectionState(EMultiplayerConnectionState::LoggedIn);
			UE_LOG(LogTemp, Log, TEXT("EOS já autenticado: %s"), *LoggedInUserId);
			MultiplayerOnLoginComplete.Broadcast(true, TEXT(""));
			return;
		}
	}

	CurrentLoginType = LoginType;
	bIsLoggedIn = false;
	LoggedInUserId.Empty();

	if (LoginCompleteDelegateHandle.IsValid())
	{
		IdentityInterface->ClearOnLoginCompleteDelegate_Handle(0, LoginCompleteDelegateHandle);
		LoginCompleteDelegateHandle.Reset();
	}

	LoginCompleteDelegateHandle = IdentityInterface->AddOnLoginCompleteDelegate_Handle(0, LoginCompleteDelegate);

	FOnlineAccountCredentials Credentials;

	switch (LoginType)
	{
	case EEOSLoginType::AccountPortal:
		Credentials.Type = TEXT("accountportal");
		break;

	case EEOSLoginType::PersistentAuth:
		Credentials.Type = TEXT("persistentauth");
		break;

	case EEOSLoginType::DedicatedServer:
		Credentials.Type = TEXT("developer");
		Credentials.Id = BuildDedicatedServerAddress();
		if (!FParse::Value(FCommandLine::Get(), TEXT("EOSDevAuthUser="), Credentials.Token))
		{
			Credentials.Token = TEXT("DedicatedServer");
		}
		break;
	}

	UE_LOG(LogTemp, Log, TEXT("Tentando login EOS. Tipo: %s"), *Credentials.Type);

	if (!IdentityInterface->Login(0, Credentials))
	{
		if (LoginCompleteDelegateHandle.IsValid())
		{
			IdentityInterface->ClearOnLoginCompleteDelegate_Handle(0, LoginCompleteDelegateHandle);
			LoginCompleteDelegateHandle.Reset();
		}

		bIsLoggedIn = false;
		LoggedInUserId.Empty();
		SetConnectionState(EMultiplayerConnectionState::Disconnected);

		const FString ErrorMessage = TEXT("Falha imediata ao iniciar o Login().");
		UE_LOG(LogTemp, Error, TEXT("%s"), *ErrorMessage);
		MultiplayerOnLoginComplete.Broadcast(false, ErrorMessage);
	}
}

void UMultiplayerSessionsSubsystem::OnLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{
	if (IdentityInterface.IsValid() && LoginCompleteDelegateHandle.IsValid())
	{
		IdentityInterface->ClearOnLoginCompleteDelegate_Handle(LocalUserNum, LoginCompleteDelegateHandle);
		LoginCompleteDelegateHandle.Reset();
	}

	if (bWasSuccessful)
	{
		bIsLoggedIn = true;
		LoggedInUserId = UserId.ToString();

		FString PlayerName = TEXT("Player");

		// 🔥 PEGA NICK DO EOS
		if (IdentityInterface.IsValid())
		{
			FString Nick = IdentityInterface->GetPlayerNickname(LocalUserNum);

			if (!Nick.IsEmpty())
			{
				PlayerName = Nick;
			}
			else
			{
				PlayerName = FString::Printf(TEXT("Player_%s"), *LoggedInUserId.Left(4));
			}
		}

		// 🔥 SETA NO PLAYER STATE (CRÍTICO)
		if (UWorld* World = GetWorld())
		{
			if (APlayerController* PC = World->GetFirstPlayerController())
			{
				if (APlayerState* PS = PC->PlayerState)
				{
					PS->SetPlayerName(PlayerName);
				}
			}
		}

		SetConnectionState(EMultiplayerConnectionState::LoggedIn);

		UE_LOG(LogTemp, Log, TEXT("EOS Login SUCCESS: %s | Nick: %s"), *LoggedInUserId, *PlayerName);

		MultiplayerOnLoginComplete.Broadcast(true, TEXT(""));
	}
	else
	{
		bIsLoggedIn = false;
		LoggedInUserId.Empty();
		SetConnectionState(EMultiplayerConnectionState::Disconnected);

		UE_LOG(LogTemp, Error, TEXT("EOS Login FAILED: %s"), *Error);
		MultiplayerOnLoginComplete.Broadcast(false, Error);
	}
}

void UMultiplayerSessionsSubsystem::CreateSession(int32 NumPublicConnections, FString MatchType)
{
	if (!IsValidSessionInterface())
	{
		MultiplayerOnCreateSessionComplete.Broadcast(false);
		return;
	}

	if (IsUsingEOS() && !bIsLoggedIn && !IsRunningDedicatedServerInstance())
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot create session: user is not logged in on EOS."));
		MultiplayerOnCreateSessionComplete.Broadcast(false);
		return;
	}

	if (auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession))
	{
		bCreateSessionOnDestroy = true;
		LastNumPublicConnections = NumPublicConnections;
		LastMatchType = MatchType;
		DestroySession();
		return;
	}

	CreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

	LastSessionSettings = MakeShared<FOnlineSessionSettings>();
	LastSessionSettings->bIsLANMatch = false;
	LastSessionSettings->NumPublicConnections = NumPublicConnections;
	LastSessionSettings->bAllowJoinInProgress = true;
	LastSessionSettings->bAllowJoinViaPresence = true;
	LastSessionSettings->bAllowJoinViaPresenceFriendsOnly = false;
	LastSessionSettings->bShouldAdvertise = true;
	LastSessionSettings->bUsesPresence = !IsRunningDedicatedServerInstance();
	LastSessionSettings->bUseLobbiesIfAvailable = true;
	LastSessionSettings->bUseLobbiesVoiceChatIfAvailable = true;
	LastSessionSettings->bAllowInvites = true;
	LastSessionSettings->bUsesStats = false;
	LastSessionSettings->bIsDedicated = IsRunningDedicatedServerInstance();
	LastSessionSettings->BuildUniqueId = 1;
	LastSessionSettings->Set(FName("MatchType"), MatchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	UE_LOG(LogTemp, Log, TEXT("Creating Session... Dedicated: %s"), LastSessionSettings->bIsDedicated ? TEXT("true") : TEXT("false"));

	if (IsRunningDedicatedServerInstance())
	{
		if (!SessionInterface->CreateSession(0, NAME_GameSession, *LastSessionSettings))
		{
			SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
			MultiplayerOnCreateSessionComplete.Broadcast(false);
		}
		return;
	}

	ULocalPlayer* LocalPlayer = GetGameInstance() ? GetGameInstance()->GetFirstGamePlayer() : nullptr;
	if (!LocalPlayer)
	{
		UE_LOG(LogTemp, Error, TEXT("LocalPlayer is NULL"));
		MultiplayerOnCreateSessionComplete.Broadcast(false);
		return;
	}

	auto UserId = LocalPlayer->GetPreferredUniqueNetId();
	if (!UserId.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("UserId INVALID"));
		MultiplayerOnCreateSessionComplete.Broadcast(false);
		return;
	}

	if (!SessionInterface->CreateSession(*UserId, NAME_GameSession, *LastSessionSettings))
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
		MultiplayerOnCreateSessionComplete.Broadcast(false);
	}
}

void UMultiplayerSessionsSubsystem::FindSessions(int32 MaxSearchResults)
{
	if (!IsValidSessionInterface())
	{
		MultiplayerOnFindSessionsComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
		return;
	}

	if (IsUsingEOS() && !bIsLoggedIn)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot find sessions: user is not logged in on EOS."));
		MultiplayerOnFindSessionsComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
		return;
	}

	FindSessionsCompleteDelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate);
	LastSessionSearch = MakeShared<FOnlineSessionSearch>();
	LastSessionSearch->MaxSearchResults = MaxSearchResults;
	LastSessionSearch->bIsLanQuery = false;
	LastSessionSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);

	const ULocalPlayer* LocalPlayer = GetWorld() ? GetWorld()->GetFirstLocalPlayerFromController() : nullptr;
	if (!LocalPlayer || !LocalPlayer->GetPreferredUniqueNetId().IsValid())
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
		MultiplayerOnFindSessionsComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
		return;
	}

	if (!SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), LastSessionSearch.ToSharedRef()))
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
		MultiplayerOnFindSessionsComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
	}
}

void UMultiplayerSessionsSubsystem::JoinSession(const FOnlineSessionSearchResult& SessionResult)
{
	if (!IsValidSessionInterface())
	{
		MultiplayerOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		return;
	}

	if (IsUsingEOS() && !bIsLoggedIn)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot join session: user is not logged in on EOS."));
		MultiplayerOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		return;
	}

	JoinSessionCompleteDelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);

	const ULocalPlayer* LocalPlayer = GetWorld() ? GetWorld()->GetFirstLocalPlayerFromController() : nullptr;
	if (!LocalPlayer || !LocalPlayer->GetPreferredUniqueNetId().IsValid())
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
		MultiplayerOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		return;
	}

	if (!SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionResult))
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
		MultiplayerOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
	}
}

void UMultiplayerSessionsSubsystem::DestroySession()
{
	if (!IsValidSessionInterface())
	{
		MultiplayerOnDestroySessionComplete.Broadcast(false);
		return;
	}

	DestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegate);

	if (!SessionInterface->DestroySession(NAME_GameSession))
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
		MultiplayerOnDestroySessionComplete.Broadcast(false);
	}
}

void UMultiplayerSessionsSubsystem::LeaveLobby()
{
	DestroySession();
	SetConnectionState(bIsLoggedIn ? EMultiplayerConnectionState::LoggedIn : EMultiplayerConnectionState::Disconnected);
	CachedLobbyPlayers.Empty();
	CachedChatMessages.Empty();
	MultiplayerOnLobbyUpdated.Broadcast();
	MultiplayerOnChatUpdated.Broadcast();
}

void UMultiplayerSessionsSubsystem::StartSession()
{
	if (!IsValidSessionInterface())
	{
		MultiplayerOnStartSessionComplete.Broadcast(false);
		return;
	}

	StartSessionCompleteDelegateHandle = SessionInterface->AddOnStartSessionCompleteDelegate_Handle(StartSessionCompleteDelegate);

	if (!SessionInterface->StartSession(NAME_GameSession))
	{
		SessionInterface->ClearOnStartSessionCompleteDelegate_Handle(StartSessionCompleteDelegateHandle);
		MultiplayerOnStartSessionComplete.Broadcast(false);
	}
}

void UMultiplayerSessionsSubsystem::SendLobbyChatMessage(const FString& Message)
{
	if (Message.TrimStartAndEnd().IsEmpty())
	{
		return;
	}

	if (UWorld* World = GetWorld())
	{
		if (AMultiplayerLobbyPlayerController* PC = Cast<AMultiplayerLobbyPlayerController>(World->GetFirstPlayerController()))
		{
			PC->ServerSendLobbyChatMessage(Message);
		}
	}
}

void UMultiplayerSessionsSubsystem::SetLocalPlayerReady(bool bReady)
{
	bLocalPlayerReady = bReady;
	MultiplayerOnReadyStateChanged.Broadcast(bLocalPlayerReady);

	if (UWorld* World = GetWorld())
	{
		if (AMultiplayerLobbyPlayerController* PC = Cast<AMultiplayerLobbyPlayerController>(World->GetFirstPlayerController()))
		{
			PC->ServerSetLobbyReadyState(bReady);
		}
	}
}

void UMultiplayerSessionsSubsystem::ToggleLocalVoiceEnabled()
{
	SetLocalVoiceEnabled(!bLocalVoiceEnabled);
}

void UMultiplayerSessionsSubsystem::SetLocalVoiceEnabled(bool bEnabled)
{
	bLocalVoiceEnabled = bEnabled;
	ApplyVoiceEnabled(bEnabled);
	MultiplayerOnVoiceEnabledChanged.Broadcast(bLocalVoiceEnabled);

	if (UWorld* World = GetWorld())
	{
		if (AMultiplayerLobbyPlayerController* PC = Cast<AMultiplayerLobbyPlayerController>(World->GetFirstPlayerController()))
		{
			PC->ServerSetLobbyVoiceEnabled(bEnabled);
		}
	}
}

void UMultiplayerSessionsSubsystem::ApplyVoiceEnabled(bool bEnabled)
{
	if (IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get())
	{
		if (IOnlineVoicePtr VoiceInterface = Subsystem->GetVoiceInterface())
		{
			if (bEnabled)
			{
				VoiceInterface->StartNetworkedVoice(0);
			}
			else
			{
				VoiceInterface->StopNetworkedVoice(0);
			}
		}
	}
}

void UMultiplayerSessionsSubsystem::RefreshLobbyData()
{
	SyncLocalPlayerFlagsFromWorld();

	const TArray<FMultiplayerLobbyPlayerInfo> NewPlayers = BuildLobbyPlayerSnapshot();
	if (HasLobbyPlayerSnapshotChanged(NewPlayers))
	{
		CachedLobbyPlayers = NewPlayers;
		MultiplayerOnLobbyUpdated.Broadcast();
	}

	const TArray<FMultiplayerChatMessage> NewMessages = BuildChatSnapshot();
	if (HasChatSnapshotChanged(NewMessages))
	{
		CachedChatMessages = NewMessages;
		MultiplayerOnChatUpdated.Broadcast();
	}
}

bool UMultiplayerSessionsSubsystem::HandleLobbyTicker(float DeltaSeconds)
{
	RefreshLobbyData();
	return true;
}

TArray<FMultiplayerLobbyPlayerInfo> UMultiplayerSessionsSubsystem::BuildLobbyPlayerSnapshot() const
{
	TArray<FMultiplayerLobbyPlayerInfo> Snapshot;

	const UWorld* World = GetWorld();
	const APlayerController* LocalPC = World ? World->GetFirstPlayerController() : nullptr;
	const FString LocalPlayerId = LocalPC && LocalPC->PlayerState && LocalPC->PlayerState->GetUniqueId().IsValid()
		? LocalPC->PlayerState->GetUniqueId()->ToString()
		: LoggedInUserId;

	const AGameStateBase* GS = World ? World->GetGameState() : nullptr;
	if (!GS)
	{
		return Snapshot;
	}

	for (APlayerState* PlayerState : GS->PlayerArray)
	{
		if (!PlayerState)
		{
			continue;
		}

		FMultiplayerLobbyPlayerInfo PlayerInfo;
		PlayerInfo.PlayerId = PlayerState->GetUniqueId().IsValid() ? PlayerState->GetUniqueId()->ToString() : PlayerState->GetPlayerName();
		PlayerInfo.PlayerName = PlayerState->GetPlayerName();
		PlayerInfo.bIsLocalPlayer = (PlayerInfo.PlayerId == LocalPlayerId) || (LocalPC && PlayerState == LocalPC->PlayerState);

		if (const AMultiplayerLobbyPlayerState* LobbyPlayerState = Cast<AMultiplayerLobbyPlayerState>(PlayerState))
		{
			PlayerInfo.bIsReady = LobbyPlayerState->IsReady();
			PlayerInfo.bVoiceEnabled = LobbyPlayerState->IsVoiceEnabled();
		}
		else
		{
			PlayerInfo.bVoiceEnabled = true;
		}

		Snapshot.Add(PlayerInfo);
	}

	return Snapshot;
}

TArray<FMultiplayerChatMessage> UMultiplayerSessionsSubsystem::BuildChatSnapshot() const
{
	if (const UWorld* World = GetWorld())
	{
		if (const AMultiplayerLobbyGameState* LobbyGameState = World->GetGameState<AMultiplayerLobbyGameState>())
		{
			return LobbyGameState->GetRecentMessages();
		}
	}

	return {};
}

bool UMultiplayerSessionsSubsystem::HasLobbyPlayerSnapshotChanged(const TArray<FMultiplayerLobbyPlayerInfo>& NewSnapshot) const
{
	if (NewSnapshot.Num() != CachedLobbyPlayers.Num())
	{
		return true;
	}

	for (int32 Index = 0; Index < NewSnapshot.Num(); ++Index)
	{
		if (!(NewSnapshot[Index] == CachedLobbyPlayers[Index]))
		{
			return true;
		}
	}

	return false;
}

bool UMultiplayerSessionsSubsystem::HasChatSnapshotChanged(const TArray<FMultiplayerChatMessage>& NewSnapshot) const
{
	if (NewSnapshot.Num() != CachedChatMessages.Num())
	{
		return true;
	}

	for (int32 Index = 0; Index < NewSnapshot.Num(); ++Index)
	{
		if (!(NewSnapshot[Index] == CachedChatMessages[Index]))
		{
			return true;
		}
	}

	return false;
}

void UMultiplayerSessionsSubsystem::SyncLocalPlayerFlagsFromWorld()
{
	const UWorld* World = GetWorld();
	const APlayerController* PC = World ? World->GetFirstPlayerController() : nullptr;
	const AMultiplayerLobbyPlayerState* LobbyPlayerState = PC ? PC->GetPlayerState<AMultiplayerLobbyPlayerState>() : nullptr;
	if (!LobbyPlayerState)
	{
		return;
	}

	if (bLocalPlayerReady != LobbyPlayerState->IsReady())
	{
		bLocalPlayerReady = LobbyPlayerState->IsReady();
		MultiplayerOnReadyStateChanged.Broadcast(bLocalPlayerReady);
	}

	if (bLocalVoiceEnabled != LobbyPlayerState->IsVoiceEnabled())
	{
		bLocalVoiceEnabled = LobbyPlayerState->IsVoiceEnabled();
		MultiplayerOnVoiceEnabledChanged.Broadcast(bLocalVoiceEnabled);
	}
}

void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (SessionInterface)
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
	}

	if (bWasSuccessful)
	{
		SetConnectionState(EMultiplayerConnectionState::InLobby);
	}

	MultiplayerOnCreateSessionComplete.Broadcast(bWasSuccessful);
}

void UMultiplayerSessionsSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
	if (SessionInterface)
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
	}

	if (!LastSessionSearch.IsValid() || LastSessionSearch->SearchResults.Num() == 0)
	{
		MultiplayerOnFindSessionsComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
		return;
	}

	MultiplayerOnFindSessionsComplete.Broadcast(LastSessionSearch->SearchResults, bWasSuccessful);
}

void UMultiplayerSessionsSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (SessionInterface)
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
	}

	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		SetConnectionState(EMultiplayerConnectionState::InLobby);
	}

	MultiplayerOnJoinSessionComplete.Broadcast(Result);
}

void UMultiplayerSessionsSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (SessionInterface)
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
	}

	if (bWasSuccessful && bCreateSessionOnDestroy)
	{
		bCreateSessionOnDestroy = false;
		CreateSession(LastNumPublicConnections, LastMatchType);
	}

	MultiplayerOnDestroySessionComplete.Broadcast(bWasSuccessful);
}

void UMultiplayerSessionsSubsystem::OnStartSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (SessionInterface)
	{
		SessionInterface->ClearOnStartSessionCompleteDelegate_Handle(StartSessionCompleteDelegateHandle);
	}

	if (bWasSuccessful)
	{
		SetConnectionState(EMultiplayerConnectionState::InMatch);
	}

	MultiplayerOnStartSessionComplete.Broadcast(bWasSuccessful);
}
