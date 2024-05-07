// Copyright Developer Bastian 2024. Contact developer.bastian@gmail.com. All Rights Reserved.

#include "BA_RepArrayActorComp.h"

DEFINE_LOG_CATEGORY(Log_BA_RepArrayActorComponent);
#define LOCTEXT_NAMESPACE "FBA_RepArrayActorCompModule"

void FBA_RepArrayActorCompModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FBA_RepArrayActorCompModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FBA_RepArrayActorCompModule, BA_RepArrayActorComp)