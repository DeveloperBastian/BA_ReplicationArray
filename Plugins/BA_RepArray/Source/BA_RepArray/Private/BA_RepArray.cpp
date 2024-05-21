// Copyright Developer Bastian 2024. Contact: developer.bastian@gmail.com or https://discord.gg/8JStx9XZGP. License Creative Commons 4.0 DEED (https://creativecommons.org/licenses/by/4.0/deed.en).

#include "BA_RepArray.h"

DEFINE_LOG_CATEGORY(Log_BA_IM_RepArray);
#define LOCTEXT_NAMESPACE "FBA_RepArrayModule"

void FBA_RepArrayModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FBA_RepArrayModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FBA_RepArrayModule, BA_RepArray)