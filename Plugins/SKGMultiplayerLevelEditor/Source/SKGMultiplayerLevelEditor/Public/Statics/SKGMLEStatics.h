// Copyright 2023, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DataTypes/SKGLEDataTypes.h"
#include "SKGMLEStatics.generated.h"

/**
 * 
 */
UCLASS()
class SKGMULTIPLAYERLEVELEDITOR_API USKGMLEStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	static FString EncodeString(const FString& StringToEncode);
	static FString DecodeString(const FString& StringToEncode);
	
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Serialization")
	static FString SerializeLevel(UObject* WorldContextObject, bool bSaveAsBase, bool& Success);
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Serialization")
	static FSKGMLESave DeSerializeLevel(const FString& JsonString, bool& Success);
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Map")
	static void LoadSavedMap(UObject* WorldContextObject, FSKGMLESave SaveData);
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Map")
	static void LoadSavedMapFromJson(UObject* WorldContextObject, const FString& JsonString);
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Map")
	static void ClearMap(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | FileHandling")
	static bool LoadMapFromFile(UObject* WorldContextObject, const FString& MapDirectory, const FString& MapName, const FString& Extension, FString& OutString, FString& FullMapName);
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | FileHandling")
	static bool SaveMapToFile(UObject* WorldContextObject, const FString& MapDirectory, const FString& MapName, const FString& StringToSave, FString& FullMapName);
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | FileHandling")
	static bool DeleteFile(const FString& MapDirectory, const FString& MapName);
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | FileHandling")
	static bool DoesMapExist(UObject* WorldContextObject, const FString& MapDirectory, const FString& MapName);
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | FileHandling")
	static FString GetRealMapName(const FString& MapName);
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | FileHandling")
	static TArray<FString> GetMapList(UObject* WorldContextObject, const FString& MapDirectory, bool bCutLevelname = true, bool bShowAllMaps = false);
	// Directory/.filetype like .fbx
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | FileHandling")
	static TArray<FString> GetAllFilesOfType(const FString& Directory, const FString& Extension);
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | FileHandling")
	static FString RemoveExtension(const FString& String);
	
	static void StripInvalidMaps(const FString& WorldName, TArray<FString>& MapList);

	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | GetData")
	static TArray<UMeshComponent*> GetMeshComponentsFromActor(AActor* Actor);
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | GetData")
	static TArray<FSKGMLEMeshComponent> GetMeshComponentData(AActor* Actor);
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | GetData")
	static FSKGMLEItemMaterialData GetItemMaterialData(AActor* Actor);
	
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | SetData")
	static bool ApplyMaterial(const FSKGMLEMeshComponent& MaterialData);
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | SetData")
	static bool ApplyMaterialItem(const FSKGMLEItemMaterialData& MaterialData);
	
	UFUNCTION(BlueprintPure, Category = "SKGMapEditor | Spawn")
	static bool GetMouseWorldTransform(const APlayerController* PlayerController, FTransform& OUTTransform, float MaxTraceDistance, TArray<AActor*> IgnoredActors);

	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Selection")
	static void HideSelection(TArray<AActor*> Actors, bool bHide);

	static TArray<FSKGMLEMaterial> GetMeshComponentMaterialData(UMeshComponent* MeshComponent);

	static void SetBoolProperty(AActor* Actor, FSKGMLEBoolProperty PropertyData, bool Value);
	static void SetFloatProperty(AActor* Actor, FSKGMLEFloatProperty PropertyData, double Value);
	static void SetIntProperty(AActor* Actor, FSKGMLEIntProperty PropertyData, int32 Value);
	static void SetTextProperty(AActor* Actor, FSKGMLETextProperty PropertyData, FText Value);
	static void SetNameProperty(AActor* Actor, FSKGMLENameProperty PropertyData, FName Value);
	static void SetStringProperty(AActor* Actor, FSKGMLEStringProperty PropertyData, FString Value);

	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Selection")
	static TArray<FSKGMLEFloatProperty> GetAllFloatProperties(AActor* Actor, FString Prefix = "SKG");
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Selection")
	static TArray<FSKGMLEIntProperty> GetAllIntProperties(AActor* Actor, FString Prefix = "SKG");
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Selection")
	static TArray<FSKGMLEBoolProperty> GetAllBoolProperties(AActor* Actor, FString Prefix = "SKG");
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Selection")
	static TArray<FSKGMLETextProperty> GetAllTextProperties(AActor* Actor, FString Prefix = "SKG");
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Selection")
	static TArray<FSKGMLENameProperty> GetAllNameProperties(AActor* Actor, FString Prefix = "SKG");
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Selection")
	static TArray<FSKGMLEStringProperty> GetAllStringProperties(AActor* Actor, FString Prefix = "SKG");
};
