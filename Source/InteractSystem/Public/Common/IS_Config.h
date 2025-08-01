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

	//默认的交互提示面板
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly)
	TSoftClassPtr<UUserWidget> DefaultInteractTipPanelClass;

	//交互描边的材质
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UMaterial> DefaultInteractOutLineMaterial;

	////交互次数不足时的默认失败文本提示
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//FText InteractNum_DefaultFailText;

	////未开启交互时的默认失败文本提示
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//FText InteractActive_DefaultFailText;
};
