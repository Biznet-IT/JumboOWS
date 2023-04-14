// Copyright 2023 Biznet It

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HttpDownloadAsset.h"
#include "UHttpDownloadFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class UUHttpDownloadFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Download")
	static void DownloadAssetFile(const FString& URL, const FString& SavePath);

	UFUNCTION(BlueprintCallable, Category = "File IO")
	static bool DeleteDirectoryContent(const FString& DirectoryPath);
};
