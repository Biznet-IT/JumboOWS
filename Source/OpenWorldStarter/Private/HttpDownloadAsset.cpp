// Copyright 2023 Biznet It


#include "HttpDownloadAsset.h"

#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpModule.h"

void UHttpDownloadAsset::DownloadAssetFile(const FString& URL)
{
    FString Path = "D:\\Biznet\\Documents\\GitHub\\JumboOWS\\Biznet\\JumboOWS\\Content\\Actors\\ShapeActor\\";
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
    HttpRequest->SetURL(URL);
    HttpRequest->SetVerb("GET");
    HttpRequest->OnProcessRequestComplete().BindUObject(this, &UHttpDownloadAsset::OnDownloadCompleted);
    HttpRequest->SetHeader("Content-Type", "application/octet-stream");
    FileSavePath = Path;
    HttpRequest->ProcessRequest();
}

void UHttpDownloadAsset::OnDownloadCompleted(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful && Response.IsValid())
    {
        int32 ResponseCode = Response->GetResponseCode();
        UE_LOG(LogTemp, Log, TEXT("C�digo de respuesta: %d"), ResponseCode);

        if (Response->GetContentLength() > 0)
        {
            // Comprobar y crear la carpeta de destino si no existe
            FString DirectoryPath = FPaths::GetPath(FileSavePath);
            if (!FPaths::DirectoryExists(DirectoryPath))
            {
                if (!FPlatformFileManager::Get().GetPlatformFile().CreateDirectoryTree(*DirectoryPath))
                {
                    UE_LOG(LogTemp, Error, TEXT("No se pudo crear la carpeta de destino: %s"), *DirectoryPath);
                    return;
                }
            }

            if (FFileHelper::SaveArrayToFile(Response->GetContent(), *FileSavePath))
            {
                UE_LOG(LogTemp, Log, TEXT("Archivo descargado en: %s"), *FileSavePath);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Error al guardar el archivo en: %s"), *FileSavePath);
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("La respuesta no tiene contenido."));
        }
    }
    else
    {
        int32 ResponseCode = Response.IsValid() ? Response->GetResponseCode() : -1;
        FString ResponseError = Response.IsValid() ? Response->GetErrorSummary().ToString() : TEXT("La respuesta no es v�lida");
        UE_LOG(LogTemp, Error, TEXT("Error al descargar el archivo. C�digo de respuesta: %d, Error: %s"), ResponseCode, *ResponseError);
    }
}