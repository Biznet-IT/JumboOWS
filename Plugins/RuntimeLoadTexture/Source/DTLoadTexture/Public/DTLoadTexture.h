// Copyright 2022-2023 Dexter.Wan. All Rights Reserved. 
// EMail: 45141961@qq.com


#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FDTLoadTextureModule"

class FDTLoadTextureModule : public IModuleInterface
{
public:
	virtual void StartupModule() override {}
	virtual void ShutdownModule() override {}
};

#undef LOCTEXT_NAMESPACE
