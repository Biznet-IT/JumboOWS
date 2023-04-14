// Copyright 2023 Biznet It


#include "UHttpDownloadFunctionLibrary.h"

/*
*/

void UUHttpDownloadFunctionLibrary::DownloadAssetFile(const FString& URL, const FString& SavePath)
{
    UHttpDownloadAsset* HttpDownloadAsset = NewObject<UHttpDownloadAsset>();

    if (HttpDownloadAsset != nullptr)
    {
        HttpDownloadAsset->DownloadAssetFile(URL, SavePath);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create UHttpDownloadAsset object."));
    }
}