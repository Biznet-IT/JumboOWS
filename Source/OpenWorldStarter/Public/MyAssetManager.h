// Copyright 2023 Biznet It

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "MyAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDSTARTER_API UMyAssetManager : public UAssetManager
{
	GENERATED_BODY()


public:
	static const FPrimaryAssetType AssetType_Custom;
	
};
