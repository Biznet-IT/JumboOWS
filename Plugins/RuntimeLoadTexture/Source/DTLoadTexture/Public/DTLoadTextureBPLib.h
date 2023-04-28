// Copyright 2022-2023 Dexter.Wan. All Rights Reserved. 
// EMail: 45141961@qq.com


#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DTLoadTextureBPLib.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogDTLoadTexture, Log, All);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FHttpTexture, const FString &, URL, UTexture2D *, Texture);

UCLASS()
class DTLOADTEXTURE_API UDTLoadTextureBPLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	/*
	 * The Runtime Local Load Texture
	 * Which Can Load JPG, PNG, TGA, BMP, PSD, GIF, HDR, PIC Format Image
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Load Texture From Local", Keywords = "load,image,texture,loadimage,loadtexture"), Category = "DT Load Texture")
	static UTexture2D * LoadTexture( const FString & FilePath );

	/*
	 * The Runtime Network Load Texture
	 * Which Can Load JPG, PNG, TGA, BMP, PSD, GIF, HDR, PIC Format Image
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Load Texture From Network", Keywords = "load,image,texture,loadimage,loadtexture"), Category = "DT Load Texture")
	static void HttpTexture( const FString & URL, FHttpTexture OnHttpTexture );
};