// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorComponent/BeInteractExtend/IS_BIEGetComponent.h"
#include "IS_BIEInteractOutLine.generated.h"

/*描边检测类型信息
* 哪些检测可以描边，在哪些条件下可以描边
*/
USTRUCT(BlueprintType)
struct FIS_OutLineInteractTraceTypeInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EIS_InteractTraceType InteractTraceType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIS_InteractVerifyInfo InteractVerifyInfo;
};

/*描边检测类型待验证信息
*/
USTRUCT(BlueprintType)
struct FIS_OutLineBeVerifyInfo
{
	GENERATED_BODY()
public:
	bool operator==(FIS_OutLineBeVerifyInfo VerifyInfo)
	{
		return InteractTraceType == VerifyInfo.InteractTraceType && InteractComponent == VerifyInfo.InteractComponent;
	}
	FIS_OutLineBeVerifyInfo(){}
	FIS_OutLineBeVerifyInfo(UIS_InteractComponent* InteractCom, EIS_InteractTraceType TraceType)
	{
		InteractComponent = InteractCom;
		InteractTraceType = TraceType;
	}
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EIS_InteractTraceType InteractTraceType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UIS_InteractComponent* InteractComponent;
};

/**可被交互物的功能扩展——描边
 * 该功能的作用受限于后期材质，必现要场景中有描边后期该组件设置的参数才有意义
 */
UCLASS()
class INTERACTSYSTEM_API UIS_BIEInteractOutLine : public UIS_BIEGetComponent
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	virtual void Init_Implementation(UIS_BeInteractComponent* BeInteractCom, UIS_BeInteractExtendBase* Data) override;

	virtual void InteractEnter_Implementation(UIS_InteractComponent* InteractComponent, EIS_InteractTraceType TraceType) override;
	virtual void InteractLeave_Implementation(UIS_InteractComponent* InteractComponent, EIS_InteractTraceType TraceType) override;
	virtual bool InteractLeaveIsEnd_Implementation() override;
	virtual void InteractStart_Implementation(UIS_InteractComponent* InteractComponent) override;
	virtual void InteractEnd_Implementation(UIS_InteractComponent* InteractComponent) override;
	virtual void InteractComplete_Implementation(UIS_InteractComponent* InteractComponent) override;
	virtual void InteractComplete_MultiSegment_Implementation(UIS_InteractComponent* InteractComponent) override;
	virtual void InteractAttachTo_Implementation(UIS_InteractComponent* InteractComponent) override;
	virtual void InteractAttachDetach_Implementation(UIS_InteractComponent* InteractComponent) override;

	//改变描边计数
	UFUNCTION(BlueprintCallable)
	int32 ChangeOutLineCount(int32 AddOutLineNum);

	UFUNCTION()
	void ReplicatedUsing_OutLineInteractTraceTypeInfo();

	//间隔验证检测的回调函数
	UFUNCTION()
	void OutLineCheckIntervalBack();
	UFUNCTION()
	void OutLineBeVerifyInfoChange(bool IsAdd, FIS_OutLineBeVerifyInfo VerifyInfo);
public:
	/*描边计数
	* 开启后在移入该被交互组件时描边计数+1，在移出该被交互组件时描边计数-1
	* 当描边计数 > 0时，显示描边
	*/
	UPROPERTY(BlueprintReadWrite)
	int32 OutLineCount = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	int32 CustomDepthStencilValue_OutLine = 255;
	//验证未通过时的间隔验证时间
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float OutLineCheckInterval = 1.0f;

	//描边网格组件
	UPROPERTY(BlueprintReadWrite, Replicated)
	TArray<UPrimitiveComponent*> OutLineMeshComponnets;

	//哪些检测类型在进入的时候需要开启描边
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = ReplicatedUsing_OutLineInteractTraceTypeInfo)
	TArray<FIS_OutLineInteractTraceTypeInfo> OutLineInteractTraceTypeInfo;
	UPROPERTY(BlueprintReadWrite)
	TMap<EIS_InteractTraceType, FIS_InteractVerifyInfo> OutLineInteractTraceTypeInfoMap;

	//当前已经记过数/进入的检测类型
	UPROPERTY(BlueprintReadWrite)
	TArray<EIS_InteractTraceType> CurEnterInteractTraceType;

	UPROPERTY()
	FTimerHandle OutLineCheckTimeHandle;
	//需要间隔验证的待检测类型
	UPROPERTY(BlueprintReadWrite)
	TArray<FIS_OutLineBeVerifyInfo> AllOutLineBeVerifyInfo;
};
