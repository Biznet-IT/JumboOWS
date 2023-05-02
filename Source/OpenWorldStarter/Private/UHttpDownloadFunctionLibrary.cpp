// Copyright 2023 Biznet It

#include "UHttpDownloadFunctionLibrary.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
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

void UUHttpDownloadFunctionLibrary::GenerarVentaPostRequest(const TArray<FString>& StringArray, int Total, const FString& UserGUID, const FString& URL)
{
    // Crear un objeto Http
    FHttpModule* Http = &FHttpModule::Get();
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();

    // Configurar la URL y el método HTTP
    Request->SetURL(URL);
    Request->SetVerb(TEXT("POST"));

    // Configurar la cabecera para el tipo de contenido
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

    // Convertir el TArray<FString> y las variables adicionales a un objeto JSON
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
    TArray<TSharedPtr<FJsonValue>> JsonStringArray;

    for (const FString& String : StringArray)
    {
        JsonStringArray.Add(MakeShareable(new FJsonValueString(String)));
    }
    JsonObject->SetArrayField(TEXT("arr_productos"), JsonStringArray);
    JsonObject->SetNumberField(TEXT("total"), Total);
    JsonObject->SetStringField(TEXT("id_usuario"), UserGUID);

    // Convertir el objeto JSON a un FString
    FString JsonString;
    TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&JsonString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter);

    // Establecer el cuerpo de la solicitud
    Request->SetContentAsString(JsonString);

    // Imprimir el objeto JSON serializado en el registro
    UE_LOG(LogTemp, Log, TEXT("JSON enviado: %s"), *JsonString);

    // Configurar la función de delegado para manejar la respuesta usando una expresión lambda
    Request->OnProcessRequestComplete().BindLambda(
        [&](FHttpRequestPtr Req, FHttpResponsePtr Res, bool ConnectedSuccessfully)
        {
            if (ConnectedSuccessfully && Res.IsValid())
            {
                FString ResponseString = Res->GetContentAsString();
                UE_LOG(LogTemp, Log, TEXT("Response: %s"), *ResponseString);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Error al procesar la solicitud"));
            }
        }
    );

    // Enviar la solicitud
    Request->ProcessRequest();
}
