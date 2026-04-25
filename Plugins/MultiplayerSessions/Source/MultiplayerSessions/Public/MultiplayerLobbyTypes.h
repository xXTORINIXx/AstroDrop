#pragma once

#include "CoreMinimal.h"
#include "MultiplayerLobbyTypes.generated.h"

UENUM(BlueprintType)
enum class EMultiplayerConnectionState : uint8
{
	Disconnected UMETA(DisplayName = "Disconnected"),
	Connecting UMETA(DisplayName = "Connecting"),
	LoggedIn UMETA(DisplayName = "Logged In"),
	InParty UMETA(DisplayName = "In Party"),
	Matchmaking UMETA(DisplayName = "Matchmaking"),
	InStagingLobby UMETA(DisplayName = "In Staging Lobby"),
	InLobby UMETA(DisplayName = "In Lobby"),
	InMatch UMETA(DisplayName = "In Match")
};

USTRUCT(BlueprintType)
struct FMultiplayerChatMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString SenderId;

	UPROPERTY(BlueprintReadOnly)
	FString SenderName;

	UPROPERTY(BlueprintReadOnly)
	FString Message;

	UPROPERTY(BlueprintReadOnly)
	FString Timestamp;

	bool operator==(const FMultiplayerChatMessage& Other) const
	{
		return SenderId == Other.SenderId
			&& SenderName == Other.SenderName
			&& Message == Other.Message
			&& Timestamp == Other.Timestamp;
	}
};

USTRUCT(BlueprintType)
struct FMultiplayerLobbyPlayerInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString PlayerId;

	UPROPERTY(BlueprintReadOnly)
	FString PlayerName;

	UPROPERTY(BlueprintReadOnly)
	bool bIsReady = false;

	UPROPERTY(BlueprintReadOnly)
	bool bVoiceEnabled = true;

	UPROPERTY(BlueprintReadOnly)
	bool bIsLocalPlayer = false;

	bool operator==(const FMultiplayerLobbyPlayerInfo& Other) const
	{
		return PlayerId == Other.PlayerId
			&& PlayerName == Other.PlayerName
			&& bIsReady == Other.bIsReady
			&& bVoiceEnabled == Other.bVoiceEnabled
			&& bIsLocalPlayer == Other.bIsLocalPlayer;
	}
};

USTRUCT(BlueprintType)
struct FMultiplayerPartyMemberInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString PlayerId;

	UPROPERTY(BlueprintReadOnly)
	FString PlayerName;

	UPROPERTY(BlueprintReadOnly)
	bool bIsReady = false;

	UPROPERTY(BlueprintReadOnly)
	bool bIsLeader = false;

	UPROPERTY(BlueprintReadOnly)
	bool bIsLocalPlayer = false;

	bool operator==(const FMultiplayerPartyMemberInfo& Other) const
	{
		return PlayerId == Other.PlayerId
			&& PlayerName == Other.PlayerName
			&& bIsReady == Other.bIsReady
			&& bIsLeader == Other.bIsLeader
			&& bIsLocalPlayer == Other.bIsLocalPlayer;
	}
};
