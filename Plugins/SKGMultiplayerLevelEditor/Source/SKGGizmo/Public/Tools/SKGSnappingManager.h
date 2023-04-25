// Copyright 2023, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "SceneQueries/SceneSnappingManager.h"
#include "SKGSnappingManager.generated.h"

class FSKGToolsContextQueriesImpl;

UCLASS()
class SKGGIZMO_API USKGSnappingManager : public USceneSnappingManager
{
	GENERATED_BODY()
public:
	void SetToolsContextQuery(FSKGToolsContextQueriesImpl* ToolsContextQueries)
	{
		QueriesAPI = ToolsContextQueries;
	}
	
	virtual bool ExecuteSceneSnapQuery(const FSceneSnapQueryRequest& Request, TArray<FSceneSnapQueryResult>& ResultsOut) const override
	{
		switch (Request.RequestType)
		{
		case ESceneSnapQueryType::Position: return ExecuteSceneSnapQueryPosition(Request, ResultsOut);
		case ESceneSnapQueryType::Rotation: return ExecuteSceneSnapQueryRotation(Request, ResultsOut);
		}
		return false;
	}

protected:
	virtual bool ExecuteSceneSnapQueryPosition(const FSceneSnapQueryRequest& Request, TArray<FSceneSnapQueryResult>& Results) const;
	virtual bool ExecuteSceneSnapQueryRotation(const FSceneSnapQueryRequest& Request, TArray<FSceneSnapQueryResult>& Results) const;
	static double SnapToIncrement(double fValue, double fIncrement, double offset = 0);

	const FSKGToolsContextQueriesImpl* QueriesAPI = nullptr;
};
