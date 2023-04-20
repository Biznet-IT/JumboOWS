// Copyright 2023 Biznet It


#include "MyAssetLoader.h"

UObject* UMyAssetLoader::LoadAssetAtIndex(int32 Index)
{
    if (Index < 0 || Index >= Assets.Num())
    {
        UE_LOG(LogTemp, Error, TEXT("Índice de asset fuera de rango."));
        return nullptr;
    }

    UObject* Asset = Assets[Index].LoadSynchronous();
    if (!Asset)
    {
        UE_LOG(LogTemp, Error, TEXT("Asset no pudo ser cargado."));
        return nullptr;
    }

    return Asset;
}