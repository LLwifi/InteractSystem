// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
//#include <Common/IS_BeInteractInterface.h>
#include <IS_StructAndEnum.h>
#include "IS_BlueprintFunctionLibrary.generated.h"

/**
 * ����ϵͳ����ͼ������
 */
UCLASS()
class INTERACTSYSTEM_API UIS_BlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	/*�����ҵ�ǰ����Ŀ�귽��ļн�
	* return �� 0~360
	*/
	UFUNCTION(BlueprintCallable)
	static float GetAngleFromMyForwardAndTargetDir(FVector MyForward, FVector TargetDir);

	//ͨ��FIS_BeInteractInfoHandle��ȡ����������Ϣ���ṹ������
	UFUNCTION(BlueprintCallable)
	static bool GetBeInteractInfoFromHandle(FIS_BeInteractInfoHandle Handle, FIS_BeInteractInfo& BeInteractInfo);

	//ͨ��FIS_BeInteractInfoHandle��ȡ����������չ���ṹ������
	UFUNCTION(BlueprintCallable)
	static bool GetBeInteractExtendFromHandle(FIS_BeInteractExtendHandle Handle, FIS_BeInteractExtend& BeInteractExtend);

};
