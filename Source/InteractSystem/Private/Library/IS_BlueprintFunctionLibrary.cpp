// Fill out your copyright notice in the Description page of Project Settings.


#include "Library/IS_BlueprintFunctionLibrary.h"
#include <Kismet/KismetMathLibrary.h>

float UIS_BlueprintFunctionLibrary::GetAngleFromMyForwardAndTargetDir(FVector MyForward, FVector TargetDir)
{
	float BaseAngle = UKismetMathLibrary::DegAcos(UKismetMathLibrary::Vector_CosineAngle2D(TargetDir, MyForward));
	FVector Cross = UKismetMathLibrary::Cross_VectorVector(TargetDir, MyForward);//判断是否越过了180度

	if (Cross.Z > 0.0f)
	{
		return 360.0f - BaseAngle;
	}
	else
	{
		return BaseAngle;
	}
}

bool UIS_BlueprintFunctionLibrary::GetBeInteractInfoFromHandle(FIS_BeInteractInfoHandle Handle, FIS_BeInteractInfo& BeInteractInfo)
{
	UDataTable* DT = UIS_Config::GetInstance()->BeInteractInfoDataTable.LoadSynchronous();
	if(DT)
	{
		if (DT && DT->GetRowNames().Contains(Handle.RowName))
		{
			BeInteractInfo = *DT->FindRow<FIS_BeInteractInfo>(Handle.RowName, TEXT(""));
			return true;
		}
	}
	return false;
}

bool UIS_BlueprintFunctionLibrary::GetBeInteractExtendFromHandle(FIS_BeInteractExtendHandle Handle, FIS_BeInteractExtend& BeInteractExtend)
{
	UDataTable* DT = UIS_Config::GetInstance()->BeInteractExtendDataTable.LoadSynchronous();
	if (DT)
	{
		if (DT && DT->GetRowNames().Contains(Handle.RowName))
		{
			BeInteractExtend = *DT->FindRow<FIS_BeInteractExtend>(Handle.RowName, TEXT(""));
			return true;
		}
	}
	return false;
}
