// Copyright 2023 Biznet It

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpModule.h"
#include "HttpDownloadAsset.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDSTARTER_API UHttpDownloadAsset : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Download")
	void DownloadAssetFile(const FString& URL, const FString& SavePath);

private:
	void OnDownloadCompleted(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	FString FileSavePath; // Agrega esta línea para definir FileSavePath

	
};
