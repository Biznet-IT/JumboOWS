// Copyright 2023, Dakota Dawe, All rights reserved


#include "Tools/SKGToolsFunctionsLibrary.h"

ESKGMLECoordinate USKGToolsFunctionsLibrary::GetOtherCoordinate(ESKGMLECoordinate CurrentCoordinate)
{
	if (CurrentCoordinate == ESKGMLECoordinate::World)
	{
		return ESKGMLECoordinate::Relative;
	}
	return ESKGMLECoordinate::World;
}

ETransformGizmoSubElements USKGToolsFunctionsLibrary::ConvertGizmoType(ESKGMLEGizmoType Gizmo, bool& bContainsScale)
{
	bContainsScale = false;
	switch (Gizmo)
	{
	case ESKGMLEGizmoType::TranslateRotateUniformScale : bContainsScale = true; return ETransformGizmoSubElements::TranslateRotateUniformScale;
	case ESKGMLEGizmoType::StandardTranslateRotate : return ETransformGizmoSubElements::StandardTranslateRotate;
	case ESKGMLEGizmoType::FullTranslateRotateScale : bContainsScale = true; return ETransformGizmoSubElements::FullTranslateRotateScale;
	case ESKGMLEGizmoType::ScaleUniform : bContainsScale = true; return ETransformGizmoSubElements::ScaleUniform;

	case ESKGMLEGizmoType::ScaleAllPlanes : bContainsScale = true; return ETransformGizmoSubElements::ScaleAllPlanes;
	case ESKGMLEGizmoType::ScalePlaneXY : bContainsScale = true; return ETransformGizmoSubElements::ScalePlaneXY;
	case ESKGMLEGizmoType::ScalePlaneXZ : bContainsScale = true; return ETransformGizmoSubElements::ScalePlaneXZ;
	case ESKGMLEGizmoType::ScalePlaneYZ : bContainsScale = true; return ETransformGizmoSubElements::ScalePlaneYZ;

	case ESKGMLEGizmoType::ScaleAllAxes : bContainsScale = true; return ETransformGizmoSubElements::ScaleAllAxes;
	case ESKGMLEGizmoType::ScaleAxisX : bContainsScale = true; return ETransformGizmoSubElements::ScaleAxisX;
	case ESKGMLEGizmoType::ScaleAxisY : bContainsScale = true; return ETransformGizmoSubElements::ScaleAxisY;
	case ESKGMLEGizmoType::ScaleAxisZ : bContainsScale = true; return ETransformGizmoSubElements::ScaleAxisZ;

	case ESKGMLEGizmoType::RotateAllAxes : return ETransformGizmoSubElements::RotateAllAxes;
	case ESKGMLEGizmoType::RotateAxisX : return ETransformGizmoSubElements::RotateAxisX;
	case ESKGMLEGizmoType::RotateAxisY : return ETransformGizmoSubElements::RotateAxisY;
	case ESKGMLEGizmoType::RotateAxisZ : return ETransformGizmoSubElements::RotateAxisZ;

	case ESKGMLEGizmoType::TranslateAllPlanes : return ETransformGizmoSubElements::TranslateAllPlanes;
	case ESKGMLEGizmoType::TranslatePlaneYZ : return ETransformGizmoSubElements::TranslatePlaneYZ;
	case ESKGMLEGizmoType::TranslatePlaneXZ : return ETransformGizmoSubElements::TranslatePlaneXZ;
	case ESKGMLEGizmoType::TranslatePlaneXY : return ETransformGizmoSubElements::TranslatePlaneXY;

	case ESKGMLEGizmoType::TranslateAllAxes : return ETransformGizmoSubElements::TranslateAllAxes;
	case ESKGMLEGizmoType::TranslateAxisX : return ETransformGizmoSubElements::TranslateAxisX;
	case ESKGMLEGizmoType::TranslateAxisY : return ETransformGizmoSubElements::TranslateAxisY;
	case ESKGMLEGizmoType::TranslateAxisZ : return ETransformGizmoSubElements::TranslateAxisZ;
	default : return ETransformGizmoSubElements::StandardTranslateRotate;
	}
}

FToolContextSnappingConfiguration USKGToolsFunctionsLibrary::ConvertSnappingConfiguration(const FSKGMLESnappingConfiguration& SnappingConfiguration)
{
	FToolContextSnappingConfiguration Configuration;
	Configuration.PositionGridDimensions = FVector(SnappingConfiguration.Location);
	Configuration.bEnablePositionGridSnapping = SnappingConfiguration.bEnableSnappingLocation;
	Configuration.RotationGridAngles = FRotator(SnappingConfiguration.Rotation);
	Configuration.bEnableRotationGridSnapping = SnappingConfiguration.bEnableSnappingRotation;
	
	return Configuration;
}

FSKGMLESnappingConfiguration USKGToolsFunctionsLibrary::ConvertSnappingConfiguration(const FToolContextSnappingConfiguration& SnappingConfiguration)
{
	FSKGMLESnappingConfiguration Configuration;
	Configuration.Location = SnappingConfiguration.PositionGridDimensions.Z;
	Configuration.bEnableSnappingLocation = SnappingConfiguration.bEnablePositionGridSnapping;
	Configuration.Rotation = SnappingConfiguration.RotationGridAngles.Yaw;
	Configuration.bEnableSnappingRotation = SnappingConfiguration.bEnableRotationGridSnapping;
	
	return Configuration;
}
