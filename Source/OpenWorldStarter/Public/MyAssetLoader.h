// Copyright 2023 Biznet It

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MyAssetLoader.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDSTARTER_API UMyAssetLoader : public UObject
{
	GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Assets")
    TArray<TSoftObjectPtr<UObject>> Assets;

    UFUNCTION(BlueprintCallable, Category = "Assets")
    UObject* LoadAssetAtIndex(int32 Index);
	
};
