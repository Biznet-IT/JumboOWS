// Copyright 2023 Biznet It

#pragma once

#include "CoreMinimal.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
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
	static void DownloadAssetFile(const FString& URL, const FString& AssetName);

	UFUNCTION(BlueprintCallable, Category = "File IO")
	static TArray<FAssetData> GetAssetsInFolder(const FString& FolderPath);

	UFUNCTION(BlueprintCallable, Category = "File IO")
	static bool DeleteDirectoryContent(const FString& DirectoryPath);

	UFUNCTION(BlueprintCallable, Category = "Http Request")
	static void GenerarVentaPostRequest(const TArray<FString>& StringArray, int Total, const FString& UserGUID, const FString& URL);
	
};
