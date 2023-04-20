// Copyright 2023 Biznet It


#include "HttpDownloadAsset.h"

#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpModule.h"

DEFINE_LOG_CATEGORY_STATIC(HttpDownloadAsset, Display, Display);

void UHttpDownloadAsset::DownloadAssetFile(const FString& URL, const FString& AssetName)
{
    // FString Path = "D:\\Biznet\\Documents\\GitHub\\JumboOWS\\Biznet\\JumboOWS\\Content\\Actors\\ShapeActor\\";
    FString Path = FPaths::Combine(FPaths::ProjectContentDir(), TEXT("Actors\\ShapeActor\\"));
    
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
    HttpRequest->SetURL(URL);
    HttpRequest->SetVerb("GET");
    HttpRequest->OnProcessRequestComplete().BindUObject(this, &UHttpDownloadAsset::OnDownloadCompleted);
    HttpRequest->SetHeader("Content-Type", "application/octet-stream");
    
    UE_LOG(HttpDownloadAsset, Error, TEXT("AssetName: %s"), *AssetName);
    FileSavePath = Path / AssetName;
    UE_LOG(HttpDownloadAsset, Error, TEXT("File SavePath: %s"), *FileSavePath);
    HttpRequest->ProcessRequest();
}

void UHttpDownloadAsset::OnDownloadCompleted(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful && Response.IsValid())
    {
        int32 ResponseCode = Response->GetResponseCode();
        UE_LOG(HttpDownloadAsset, Error, TEXT("Código de respuesta: %d"), ResponseCode);

        if (Response->GetContentLength() > 0)
        {
            // Comprobar y crear la carpeta de destino si no existe
            FString DirectoryPath = FPaths::GetPath(FileSavePath);

            UE_LOG(HttpDownloadAsset, Error, TEXT("Directorio para guardar: %s"), *DirectoryPath);
            
            if (!FPaths::DirectoryExists(DirectoryPath))
            {
                if (!FPlatformFileManager::Get().GetPlatformFile().CreateDirectoryTree(*DirectoryPath))
                {
                    UE_LOG(HttpDownloadAsset, Error, TEXT("No se pudo crear la carpeta de destino: %s"), *DirectoryPath);
                    return;
                }
            }

            if (FFileHelper::SaveArrayToFile(Response->GetContent(), *FileSavePath))
            {
                UE_LOG(HttpDownloadAsset, Error, TEXT("Archivo descargado en: %s"), *FileSavePath);
            }
            else
            {
                UE_LOG(HttpDownloadAsset, Error, TEXT("Error al guardar el archivo en: %s"), *FileSavePath);
            }
        }
        else
        {
            UE_LOG(HttpDownloadAsset, Warning, TEXT("La respuesta no tiene contenido."));
        }
    }
    else
    {
        int32 ResponseCode = Response.IsValid() ? Response->GetResponseCode() : -1;
        UE_LOG(HttpDownloadAsset, Error, TEXT("Error al descargar el archivo. Código de respuesta: %d"), ResponseCode);
    }
}