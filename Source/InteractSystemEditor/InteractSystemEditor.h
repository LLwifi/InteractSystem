// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include <Styling/SlateStyle.h>

class IAssetTypeActions;

class FInteractSystemEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

public:
	TSharedPtr<FSlateStyleSet> StyleSet;
	TSharedPtr<IAssetTypeActions> CreateRedPointTreeAction;
};
