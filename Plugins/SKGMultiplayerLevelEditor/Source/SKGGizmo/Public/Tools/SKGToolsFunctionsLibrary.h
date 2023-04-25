// Copyright 2023, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DataTypes/SKGToolsDataTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InteractiveGizmo.h"
#include "SKGToolsFunctionsLibrary.generated.h"

/**
 * 
 */
UCLASS()
class SKGGIZMO_API USKGToolsFunctionsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, Category = "SKGMapEditor | Gizmo")
	static ESKGMLECoordinate GetOtherCoordinate(ESKGMLECoordinate CurrentCoordinate);
	
	static ETransformGizmoSubElements ConvertGizmoType(ESKGMLEGizmoType Gizmo, bool& bContainsScale);
	static FToolContextSnappingConfiguration ConvertSnappingConfiguration(const FSKGMLESnappingConfiguration& SnappingConfiguration);
	static FSKGMLESnappingConfiguration ConvertSnappingConfiguration(const FToolContextSnappingConfiguration& SnappingConfiguration);
};
