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
	void Login(int RandomNumber);
	UFUNCTION(BlueprintCallable)
	void Logout();

	IClient* VivoxVoiceClient;
	AccountId LoggedInUserId;
	
};
