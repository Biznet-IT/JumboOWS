// Copyright 2023, Dakota Dawe, All rights reserved

#pragma once

#include "GameFramework/Actor.h"
#include "Materials/MaterialInterface.h"
#include "Components/MeshComponent.h"
#include "SKGToolsDataTypes.generated.h"

extern const FName SKGGIZMO_API SKGMLE_COMPONENT_NAME;

UENUM(BlueprintType)
enum class ESKGMLECoordinate : uint8
{
	World		UMETA(DisplayName = "World"),
	Relative	UMETA(DisplayName = "Relative")
};

UENUM(BlueprintType)
enum class ESKGMLEGizmoType : uint8
{
	TranslateRotateUniformScale	UMETA(DisplayName = "TranslateRotateUniformScale"),
	StandardTranslateRotate		UMETA(DisplayName = "StandardTranslateRotate"),
	FullTranslateRotateScale	UMETA(DisplayName = "FullTranslateRotateScale"),
	ScaleUniform				UMETA(DisplayName = "ScaleUniform"),
	
	ScaleAllPlanes				UMETA(DisplayName = "ScaleAllPlanes"),
	ScalePlaneXY				UMETA(DisplayName = "ScalePlaneXY"),
	ScalePlaneXZ				UMETA(DisplayName = "ScalePlaneXZ"),
	ScalePlaneYZ				UMETA(DisplayName = "ScalePlaneYZ"),
	
	ScaleAllAxes				UMETA(DisplayName = "ScaleAllAxes"),
	ScaleAxisX					UMETA(DisplayName = "ScaleAxisX"),
	ScaleAxisY					UMETA(DisplayName = "ScaleAxisY"),
	ScaleAxisZ					UMETA(DisplayName = "ScaleAxisZ"),

	RotateAllAxes				UMETA(DisplayName = "RotateAllAxes"),
	RotateAxisX					UMETA(DisplayName = "RotateAxisX"),
	RotateAxisY					UMETA(DisplayName = "RotateAxisY"),
	RotateAxisZ					UMETA(DisplayName = "RotateAxisZ"),

	TranslateAllPlanes			UMETA(DisplayName = "TranslateAllPlanes"),
	TranslatePlaneYZ			UMETA(DisplayName = "TranslatePlaneYZ"),
	TranslatePlaneXZ			UMETA(DisplayName = "TranslatePlaneXZ"),
	TranslatePlaneXY			UMETA(DisplayName = "TranslatePlaneXY"),

	TranslateAllAxes			UMETA(DisplayName = "TranslateAllAxes"),
	TranslateAxisX				UMETA(DisplayName = "TranslateAxisX"),
	TranslateAxisY				UMETA(DisplayName = "TranslateAxisY"),
	TranslateAxisZ				UMETA(DisplayName = "TranslateAxisZ")
};

UENUM(BlueprintType)
enum class ESKGMLEHistoryActionType : uint8
{
	Undo	UMETA(DisplayName = "Undo"),
	Redo	UMETA(DisplayName = "Redo")
};

UENUM(BlueprintType)
enum class ESKGMLEActionType : uint8
{
	Grab					UMETA(DisplayName = "Grab"),
	Release					UMETA(DisplayName = "Release"),
	Delete					UMETA(DisplayName = "Delete"),
	Transform				UMETA(DisplayName = "Transform"),
	ClearSelection			UMETA(DisplayName = "ClearSelection"),
	Refresh					UMETA(DisplayName = "Refresh"),
	MovementMode			UMETA(DisplayName = "MovementMode"),
	CoordinateSystem		UMETA(DisplayName = "CoordinateSystem"),
	SnappingConfiguration	UMETA(DisplayName = "SnappingConfiguration"),
	GizmoType				UMETA(DisplayName = "GizmoType"),
	HighlightingToggle		UMETA(DisplayName = "HighlightingToggle"),
	CanSelectToggle			UMETA(DisplayName = "CanSelectToggle"),
	OwnedActorsChange		UMETA(DisplayName = "OwnedActorsChange"),
	WorldSettings			UMETA(DisplayName = "WorldSettings")
};

USTRUCT(BlueprintType)
struct FSKGMLESnappingConfiguration
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGMapEditor | Tools")
	float Location = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGMapEditor | Tools")
	bool bEnableSnappingLocation = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGMapEditor | Tools")
	float Rotation = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGMapEditor | Tools")
	bool bEnableSnappingRotation = true;
};

USTRUCT(BlueprintType)
struct FSKGMLEItemTransform
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly, Category = "SKGMapEditor | Tools")
	AActor* Actor = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "SKGMapEditor | Tools")
	FTransform ActorTransform = FTransform();
	FSKGMLEItemTransform(){}
	FSKGMLEItemTransform(AActor* INActor)
	{
		Actor = INActor;
		if (IsValid(Actor))
		{
			ActorTransform = Actor->GetActorTransform();
		}
	}
	FSKGMLEItemTransform(AActor* INActor, const FTransform& Transform)
	{
		Actor = INActor;
		ActorTransform = Transform;
	}

	void Update()
	{
		if (IsValid(Actor))
		{
			ActorTransform = Actor->GetActorTransform();
		}
	}
};