// Copyright 2023 Biznet It

#include "UHttpDownloadFunctionLibrary.h"
#include "Blueprint/UserWidget.h"
#include "AssetRegistryModule.h"
#include "AssetData.h"

/*
*/

void UUHttpDownloadFunctionLibrary::DownloadAssetFile(const FString& URL, const FString& AssetName)
{
    UHttpDownloadAsset* HttpDownloadAsset = NewObject<UHttpDownloadAsset>();

    if (HttpDownloadAsset != nullptr)
    {
        HttpDownloadAsset->DownloadAssetFile(URL, AssetName);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create UHttpDownloadAsset object."));
    }
}

TArray<FAssetData> UUHttpDownloadFunctionLibrary::GetAssetsInFolder(const FString& FolderPath)
{
    TArray<FAssetData> AssetDataList;

    if (!FModuleManager::Get().ModuleExists(TEXT("AssetRegistry")))
    {
        UE_LOG(LogTemp, Error, TEXT("El módulo AssetRegistry no está disponible."));
        return AssetDataList;
    }

    FString LongPackagePath;
    if (!FPackageName::TryConvertFilenameToLongPackageName(FolderPath, LongPackagePath))
    {
        UE_LOG(LogTemp, Error, TEXT("La ruta proporcionada no es válida: %s"), *FolderPath);
        return AssetDataList;
    }

    if (!FPackageName::DoesPackageExist(LongPackagePath))
    {
        UE_LOG(LogTemp, Error, TEXT("La carpeta no existe!: %s"), *LongPackagePath);
        return AssetDataList;
    }

    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

    AssetRegistryModule.Get().GetAssetsByPath(FName(*FolderPath), AssetDataList, true, false);

    if (AssetDataList.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("La lista de assets está vacía."));
        return AssetDataList;
    }

    return AssetDataList;
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
