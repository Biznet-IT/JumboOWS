// Copyright 2023, Dakota Dawe, All rights reserved


#include "Tools/SKGSnappingManager.h"
#include "Tools/SKGToolsContext.h"

bool USKGSnappingManager::ExecuteSceneSnapQueryPosition(const FSceneSnapQueryRequest& Request,
	TArray<FSceneSnapQueryResult>& Results) const
{
	if (!QueriesAPI)
	{
		return false;
	}

	int FoundResultCount = 0;

	if ((Request.TargetTypes & ESceneSnapQueryTargetType::Grid) != ESceneSnapQueryTargetType::None)
	{
		FToolContextSnappingConfiguration SnappingConfig = QueriesAPI->GetCurrentSnappingSettings();

		if (SnappingConfig.bEnablePositionGridSnapping)
		{
			FSceneSnapQueryResult SnapResult;
			SnapResult.TargetType = ESceneSnapQueryTargetType::Grid;

			FVector GridSize = Request.GridSize.Get(SnappingConfig.PositionGridDimensions);

			SnapResult.Position.X = (FVector::FReal)SnapToIncrement(Request.Position.X, GridSize.X);
			SnapResult.Position.Y = (FVector::FReal)SnapToIncrement(Request.Position.Y, GridSize.Y);
			SnapResult.Position.Z = (FVector::FReal)SnapToIncrement(Request.Position.Z, GridSize.Z);

			Results.Add(SnapResult);
			FoundResultCount++;
		}
	}

	return FoundResultCount > 0;
}

bool USKGSnappingManager::ExecuteSceneSnapQueryRotation(const FSceneSnapQueryRequest& Request,
	TArray<FSceneSnapQueryResult>& Results) const
{
	if (!QueriesAPI)
	{
		return false;
	}

	if ((Request.TargetTypes & ESceneSnapQueryTargetType::Grid) != ESceneSnapQueryTargetType::None)
	{
		FToolContextSnappingConfiguration SnappingConfig = QueriesAPI->GetCurrentSnappingSettings();
		if (SnappingConfig.bEnableRotationGridSnapping)
		{
			FRotator Rotator(Request.DeltaRotation);
			FRotator RotGrid = Request.RotGridSize.Get(SnappingConfig.RotationGridAngles);
			Rotator = Rotator.GridSnap(RotGrid);

			FSceneSnapQueryResult SnapResult;
			SnapResult.TargetType = ESceneSnapQueryTargetType::Grid;
			SnapResult.DeltaRotation = Rotator.Quaternion();
			Results.Add(SnapResult);
			return true;
		}
	}
	return false;
}

double USKGSnappingManager::SnapToIncrement(double fValue, double fIncrement, double offset)
{
	{
		if (!FMath::IsFinite(fValue))
		{
			return 0;
		}
		fValue -= offset;
		double sign = FMath::Sign(fValue);
		fValue = FMath::Abs(fValue);
		int64 nInc = (int64)(fValue / fIncrement);
		double fRem = (double)fmod(fValue, fIncrement);
		if (fRem > fIncrement / 2.0)
		{
			++nInc;
		}
		return sign * (double)nInc * fIncrement + offset;
	}
}
