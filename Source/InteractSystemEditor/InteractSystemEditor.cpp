// Copyright Epic Games, Inc. All Rights Reserved.

#include "InteractSystemEditor.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "PropertyTypeCustomization/IS_BeInteractInfoHandleCustomization.h"
#include <Interfaces/IPluginManager.h>


#define LOCTEXT_NAMESPACE "FInteractSystemEditorModule"

void FInteractSystemEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// 加载 PropertyEditor 模块
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	//将结构体FUI_PrefabOperation 和 IUI_PrefabPropertyTypeCustom 关联起来
	PropertyEditorModule.RegisterCustomPropertyTypeLayout(FName("IS_BeInteractInfoHandle"), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&IIS_BeInteractInfoHandleCustomization::MakeInstance));
	PropertyEditorModule.RegisterCustomPropertyTypeLayout(FName("IS_BeInteractExtendHandle"), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&IIS_BeInteractExtendHandleCustomization::MakeInstance));
	PropertyEditorModule.NotifyCustomizationModuleChanged();
}

void FInteractSystemEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.UnregisterCustomPropertyTypeLayout("IS_BeInteractInfoHandle");
		PropertyModule.UnregisterCustomPropertyTypeLayout("IS_BeInteractExtendHandle");
		PropertyModule.NotifyCustomizationModuleChanged();
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FInteractSystemEditorModule, InteractSystemEditor)