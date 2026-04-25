#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MultiplayerLobbyPlayerState.generated.h"

UCLASS()
class MULTIPLAYERSESSIONS_API AMultiplayerLobbyPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AMultiplayerLobbyPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	virtual void SetReadyState(bool bInReady);

	UFUNCTION(BlueprintCallable)
	bool GetReadyState() const { return bIsReady; }
	
	UFUNCTION(BlueprintPure)
	bool IsReady() const { return bIsReady; }

	UFUNCTION(BlueprintCallable)
	void SetVoiceEnabled(bool bInVoiceEnabled);

	UFUNCTION(BlueprintPure)
	bool IsVoiceEnabled() const { return bVoiceEnabled; }

protected:
	UPROPERTY(ReplicatedUsing = OnRep_ReadyState, BlueprintReadOnly)
	bool bIsReady = false;

	UPROPERTY(ReplicatedUsing = OnRep_VoiceEnabled, BlueprintReadOnly)
	bool bVoiceEnabled = true;

	UFUNCTION()
	void OnRep_ReadyState();

	UFUNCTION()
	void OnRep_VoiceEnabled();
};
