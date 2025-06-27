// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include <Engine/DataTable.h>
#include "IS_Config.generated.h"

/**
 * 编辑器下的通用Task配置
 */
UCLASS(config = IS_Config, defaultconfig)
class INTERACTSYSTEM_API UIS_Config : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	virtual FName GetCategoryName() const final override
	{
		return FName("GameEditorConfig");
	}
	static UIS_Config* GetInstance(){ return GetMutableDefault<UIS_Config>(); }
	UFUNCTION(BlueprintPure, BlueprintCallable)
		static UIS_Config* GetIS_Config() { return GetInstance(); }

public:
};
