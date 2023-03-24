// Copyright 2023 Biznet It

#pragma once

#include "VivoxCore.h"
#include "CoreMinimal.h"
#include "OWSGameInstance.h"
#include "JumboVivoxGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDSTARTER_API UJumboVivoxGameInstance : public UOWSGameInstance
{
	GENERATED_BODY()

	virtual void Init() override;

	virtual void Shutdown() override;

	void InitVivox();
	void JoinChannel();

	UFUNCTION(BlueprintCallable)
	void Login();
	UFUNCTION(BlueprintCallable)
	void Logout();

	UFUNCTION(BlueprintCallable, Category = "Vivox")
	void JoinVoiceChannel(const FString& ChannelName);

	UFUNCTION(BlueprintCallable, Category = "Vivox")
	void LeaveVoiceChannel();

	IClient* VivoxVoiceClient;
	AccountId LoggedInUserId;
	ChannelId MyChannelId;
};
