// Copyright Developer Bastian 2024. Contact developer.bastian@gmail.com. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(Log_BA_RepArrayActorComponent, Log, All);

class FBA_RepArrayActorCompModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
