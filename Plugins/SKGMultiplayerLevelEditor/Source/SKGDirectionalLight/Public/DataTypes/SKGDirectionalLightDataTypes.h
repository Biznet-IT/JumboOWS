// Copyright 2023, Dakota Dawe, All rights reserved

#pragma once

#include "SKGDirectionalLightDataTypes.generated.h"

USTRUCT(BlueprintType)
struct FSKGDirectionalLightSettings
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, Category = "SKGMapEditor")
	float TimeOfDay = 12.0f;
	UPROPERTY(BlueprintReadWrite, Category = "SKGMapEditor")
	float Brightness = 2.0f;
};