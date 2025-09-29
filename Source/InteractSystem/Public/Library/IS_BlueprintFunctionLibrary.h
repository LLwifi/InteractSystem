// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
//#include <Common/IS_BeInteractInterface.h>
#include <IS_StructAndEnum.h>
#include "IS_BlueprintFunctionLibrary.generated.h"

/**
 * 交互系统的蓝图函数库
 */
UCLASS()
class INTERACTSYSTEM_API UIS_BlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	/*计算我的前方和目标方向的夹角
	* return ： 0~360
	*/
	UFUNCTION(BlueprintCallable)
	static float GetAngleFromMyForwardAndTargetDir(FVector MyForward, FVector TargetDir);

	//通过FIS_BeInteractInfoHandle获取【被交互信息】结构体数据
	UFUNCTION(BlueprintCallable)
	static bool GetBeInteractInfoFromHandle(FIS_BeInteractInfoHandle Handle, FIS_BeInteractInfo& BeInteractInfo);

	//通过FIS_BeInteractInfoHandle获取【被交互扩展】结构体数据
	UFUNCTION(BlueprintCallable)
	static bool GetBeInteractExtendFromHandle(FIS_BeInteractExtendHandle Handle, FIS_BeInteractExtend& BeInteractExtend);

};
