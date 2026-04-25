#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Containers/Ticker.h"
#include "MultiplayerLobbyTypes.h"
#include "MultiplayerSessionsSubsystem.generated.h"

UENUM(BlueprintType)
enum class EEOSLoginType : uint8
{
	AccountPortal UMETA(DisplayName = "Account Portal"),
	PersistentAuth UMETA(DisplayName = "Persistent Auth"),
	DedicatedServer UMETA(DisplayName = "Dedicated Server")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnCreateSessionComplete, bool, bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_TwoParams(FMultiplayerOnFindSessionsComplete, const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FMultiplayerOnJoinSessionComplete, EOnJoinSessionCompleteResult::Type Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnDestroySessionComplete, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnStartSessionComplete, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMultiplayerOnLoginComplete, bool, bWasSuccessful, const FString&, ErrorMessage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMultiplayerOnLobbyUpdated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMultiplayerOnPartyUpdated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnInviteUIClosed, bool, bWasOpened);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnVoiceEnabledChanged, bool, bVoiceEnabled);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnReadyStateChanged, bool, bReady);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnConnectionStateChanged, EMultiplayerConnectionState, NewState);

UCLASS()
class MULTIPLAYERSESSIONS_API UMultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UMultiplayerSessionsSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void CreateSession(int32 NumPublicConnections, FString MatchType);
	void FindSessions(int32 MaxSearchResults);
	void JoinSession(const FOnlineSessionSearchResult& SessionResult);
	void DestroySession();
	void StartSession();

	bool IsValidSessionInterface();

	UFUNCTION(BlueprintCallable)
	void LoginWithPortal();

	UFUNCTION(BlueprintCallable)
	void LoginWithPersistentAuth();

	UFUNCTION(BlueprintCallable)
	void LoginDedicatedServer();

	UFUNCTION(BlueprintCallable)
	void AutoLoginForCurrentRuntime();

	// Battle Royale / Party flow
	UFUNCTION(BlueprintCallable)
	void CreatePersonalParty();

	UFUNCTION(BlueprintCallable)
	void LeaveParty();

	UFUNCTION(BlueprintCallable)
	void SetPartyReady(bool bReady);

	UFUNCTION(BlueprintCallable)
	void StartBattleRoyaleMatchmaking(int32 MaxPlayers, FString Playlist);

	UFUNCTION(BlueprintCallable)
	void CancelMatchmaking();

	// Opens the EOS/Epic session invite overlay for the current GameSession.
	// Use after PLAY connected the player to the staging lobby/dedicated session.
	UFUNCTION(BlueprintCallable)
	void OpenSessionInviteUI();

	UFUNCTION(BlueprintCallable)
	void LeaveLobby();

	UFUNCTION(BlueprintCallable)
	void SendLobbyChatMessage(const FString& Message);

	UFUNCTION(BlueprintCallable)
	void SetLocalPlayerReady(bool bReady);

	UFUNCTION(BlueprintCallable)
	void ToggleLocalVoiceEnabled();

	UFUNCTION(BlueprintCallable)
	void SetLocalVoiceEnabled(bool bEnabled);

	UFUNCTION(BlueprintCallable)
	void RefreshLobbyData();

	UFUNCTION(BlueprintPure)
	bool IsLoggedIn() const { return bIsLoggedIn; }

	UFUNCTION(BlueprintPure)
	FString GetLoggedInUserId() const { return LoggedInUserId; }

	UFUNCTION(BlueprintPure)
	FString GetPlayerName() const;

	UFUNCTION(BlueprintPure)
	bool IsLocalPlayerReady() const { return bLocalPlayerReady; }

	UFUNCTION(BlueprintPure)
	bool IsLocalVoiceEnabled() const { return bLocalVoiceEnabled; }

	UFUNCTION(BlueprintPure)
	EMultiplayerConnectionState GetConnectionState() const { return ConnectionState; }

	UFUNCTION(BlueprintPure)
	EEOSLoginType GetCurrentLoginType() const { return CurrentLoginType; }

	UFUNCTION(BlueprintPure)
	bool IsInParty() const { return bInParty; }

	UFUNCTION(BlueprintPure)
	bool IsPartyLeader() const;

	UFUNCTION(BlueprintPure)
	bool AreAllPartyMembersReady() const;

	UFUNCTION(BlueprintPure)
	bool CanLaunchMatch() const;

	UFUNCTION(BlueprintPure)
	const TArray<FMultiplayerPartyMemberInfo>& GetPartyMembers() const { return CachedPartyMembers; }

	UFUNCTION(BlueprintPure)
	const TArray<FMultiplayerLobbyPlayerInfo>& GetLobbyPlayers() const { return CachedLobbyPlayers; }

	UFUNCTION(BlueprintPure)
	const TArray<FMultiplayerChatMessage>& GetChatMessages() const { return CachedChatMessages; }

	FMultiplayerOnCreateSessionComplete MultiplayerOnCreateSessionComplete;
	FMultiplayerOnFindSessionsComplete MultiplayerOnFindSessionsComplete;
	FMultiplayerOnJoinSessionComplete MultiplayerOnJoinSessionComplete;
	FMultiplayerOnDestroySessionComplete MultiplayerOnDestroySessionComplete;
	FMultiplayerOnStartSessionComplete MultiplayerOnStartSessionComplete;

	UPROPERTY(BlueprintAssignable)
	FMultiplayerOnLoginComplete MultiplayerOnLoginComplete;

	UPROPERTY(BlueprintAssignable)
	FMultiplayerOnLobbyUpdated MultiplayerOnLobbyUpdated;

	UPROPERTY(BlueprintAssignable)
	FMultiplayerOnLobbyUpdated MultiplayerOnChatUpdated;

	UPROPERTY(BlueprintAssignable)
	FMultiplayerOnPartyUpdated MultiplayerOnPartyUpdated;

	UPROPERTY(BlueprintAssignable)
	FMultiplayerOnVoiceEnabledChanged MultiplayerOnVoiceEnabledChanged;

	UPROPERTY(BlueprintAssignable)
	FMultiplayerOnReadyStateChanged MultiplayerOnReadyStateChanged;

	UPROPERTY(BlueprintAssignable)
	FMultiplayerOnConnectionStateChanged MultiplayerOnConnectionStateChanged;
	
	UPROPERTY(BlueprintAssignable)
	FMultiplayerOnInviteUIClosed MultiplayerOnInviteUIClosed;
protected:
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
	void OnStartSessionComplete(FName SessionName, bool bWasSuccessful);

private:
	IOnlineSessionPtr SessionInterface;
	IOnlineIdentityPtr IdentityInterface;

	TSharedPtr<FOnlineSessionSettings> LastSessionSettings;
	TSharedPtr<FOnlineSessionSearch> LastSessionSearch;

	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FDelegateHandle CreateSessionCompleteDelegateHandle;

	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
	FDelegateHandle FindSessionsCompleteDelegateHandle;

	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
	FDelegateHandle JoinSessionCompleteDelegateHandle;

	FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;
	FDelegateHandle DestroySessionCompleteDelegateHandle;

	FOnStartSessionCompleteDelegate StartSessionCompleteDelegate;
	FDelegateHandle StartSessionCompleteDelegateHandle;

	FOnLoginCompleteDelegate LoginCompleteDelegate;
	FDelegateHandle LoginCompleteDelegateHandle;

	FTSTicker::FDelegateHandle LobbyTickerHandle;

	bool bCreateSessionOnDestroy = false;
	int32 LastNumPublicConnections = 100;
	FString LastMatchType = TEXT("BattleRoyale");

	bool bIsLoggedIn = false;
	bool bLocalPlayerReady = false;
	bool bLocalVoiceEnabled = true;
	bool bInParty = false;
	bool bJoinFirstAvailableBattleRoyaleSession = false;
	bool bDedicatedCreateSessionAfterLogin = false;
	bool bDedicatedSessionCreated = false;
	int32 PendingBattleRoyaleMaxPlayers = 100;
	FString PendingBattleRoyalePlaylist = TEXT("BattleRoyale");
	FString LoggedInUserId;
	EEOSLoginType CurrentLoginType = EEOSLoginType::AccountPortal;
	EMultiplayerConnectionState ConnectionState = EMultiplayerConnectionState::Disconnected;

	TArray<FMultiplayerPartyMemberInfo> CachedPartyMembers;
	TArray<FMultiplayerLobbyPlayerInfo> CachedLobbyPlayers;
	TArray<FMultiplayerChatMessage> CachedChatMessages;

	void Login(EEOSLoginType LoginType);
	void SetConnectionState(EMultiplayerConnectionState NewState);
	bool HandleLobbyTicker(float DeltaSeconds);
	bool HasPartySnapshotChanged(const TArray<FMultiplayerPartyMemberInfo>& NewSnapshot) const;
	bool HasLobbyPlayerSnapshotChanged(const TArray<FMultiplayerLobbyPlayerInfo>& NewSnapshot) const;
	bool HasChatSnapshotChanged(const TArray<FMultiplayerChatMessage>& NewSnapshot) const;
	TArray<FMultiplayerPartyMemberInfo> BuildPartySnapshot() const;
	TArray<FMultiplayerLobbyPlayerInfo> BuildLobbyPlayerSnapshot() const;
	TArray<FMultiplayerChatMessage> BuildChatSnapshot() const;
	void RefreshPartyData();
	void SyncLocalPlayerFlagsFromWorld();
	void ApplyVoiceEnabled(bool bEnabled);
	FString BuildDedicatedServerAddress() const;
	bool IsUsingEOS() const;
	bool IsRunningDedicatedServerInstance() const;
	bool TryJoinFirstAvailableBattleRoyaleSession();
	void OnLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);
};
