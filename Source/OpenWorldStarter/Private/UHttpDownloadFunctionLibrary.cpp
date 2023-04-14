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

bool UUHttpDownloadFunctionLibrary::DeleteDirectoryContent(const FString& DirectoryPath)
{
    FString D_Path = DirectoryPath;
    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

    if (PlatformFile.DirectoryExists(*DirectoryPath))
    {
        FPaths::NormalizeDirectoryName(D_Path);
        TArray<FString> FilesToDelete;
        PlatformFile.FindFiles(FilesToDelete, *DirectoryPath, TEXT("*.*"));

        for (const FString& File : FilesToDelete)
        {
            PlatformFile.DeleteFile(*(DirectoryPath / File));
        }

        return true;
    }
    
    return false;
}
