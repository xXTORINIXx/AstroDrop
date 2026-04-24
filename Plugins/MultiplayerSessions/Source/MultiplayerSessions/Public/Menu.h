#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSessionSettings.h"
#include "MultiplayerSessionsSubsystem.h"
#include "Menu.generated.h"

class UButton;
class UTextBlock;
class UEditableTextBox;

UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void MenuSetup(
		int32 NumberOfPublicConnections = 4,
		FString TypeOfMatch = FString(TEXT("FreeForAll")),
		FString LobbyPath = FString(TEXT("/Game/Maps/LobbyMap"))
	);

protected:
	virtual bool Initialize() override;
	virtual void NativeDestruct() override;

private:
	void MenuTearDown();
	void SetButtonsEnabled(bool bEnabled);
	void SetLobbyActionButtonsEnabled(bool bEnabled);
	void UpdateStatus(const FText& Message);

	void RefreshHeaderPanel();
	void RefreshLobbyPanel();
	void RefreshChatPanel();
	FText BuildConnectionStateText(EMultiplayerConnectionState State) const;
	bool CanUseSubsystem() const;
	bool CanUseLobbyActions() const;
	void BindSubsystemDelegates();
	void UnbindSubsystemDelegates();

private:
	UPROPERTY()
	UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem = nullptr;

	FString PathToLobby;
	int32 NumPublicConnections = 4;
	FString MatchType = TEXT("FreeForAll");

	bool bTriedPortalLogin = false;
	bool bMenuInitialized = false;

private:
	UPROPERTY(meta = (BindWidget))
	UButton* HostButton = nullptr;

	UPROPERTY(meta = (BindWidget))
	UButton* JoinButton = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* ReadyButton = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* LeaveButton = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* ToggleVoiceButton = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* SendChatButton = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* RefreshLobbyButton = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	UEditableTextBox* ChatInputTextBox = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* StatusText = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* PlayerNameText = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* LobbyStateText = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* ReadyStateText = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* VoiceStateText = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* PlayerListText = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* ChatHistoryText = nullptr;

private:
	UFUNCTION()
	void HostButtonClicked();

	UFUNCTION()
	void JoinButtonClicked();

	UFUNCTION()
	void ReadyButtonClicked();

	UFUNCTION()
	void LeaveButtonClicked();

	UFUNCTION()
	void ToggleVoiceButtonClicked();

	UFUNCTION()
	void SendChatButtonClicked();

	UFUNCTION()
	void RefreshLobbyButtonClicked();

	UFUNCTION()
	void OnCreateSession(bool bWasSuccessful);

	void OnFindSessions(const TArray<FOnlineSessionSearchResult>& Results, bool bWasSuccessful);
	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);

	UFUNCTION()
	void OnDestroySession(bool bWasSuccessful);

	UFUNCTION()
	void OnStartSession(bool bWasSuccessful);

	UFUNCTION()
	void OnLoginComplete(bool bWasSuccessful, const FString& ErrorMessage);

	UFUNCTION()
	void OnLobbyUpdated();

	UFUNCTION()
	void OnChatUpdated();

	UFUNCTION()
	void OnVoiceEnabledChanged(bool bVoiceEnabled);

	UFUNCTION()
	void OnReadyStateChanged(bool bReady);

	UFUNCTION()
	void OnConnectionStateChanged(EMultiplayerConnectionState NewState);
};