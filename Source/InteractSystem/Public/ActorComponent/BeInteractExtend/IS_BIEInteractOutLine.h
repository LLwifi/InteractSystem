// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorComponent/BeInteractExtend/IS_BIEGetComponent.h"
#include "IS_BIEInteractOutLine.generated.h"

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

	UFUNCTION(Client, Reliable)
	void NetClient_OnInteractEnter(UIS_InteractComponent* InteractComponent, EIS_InteractTraceType TraceType);
	UFUNCTION(Client, Reliable)
	void NetClient_OnInteractLeave(UIS_InteractComponent* InteractComponent, EIS_InteractTraceType TraceType);

	//改变描边计数
	UFUNCTION(BlueprintCallable)
	int32 ChangeOutLineCount(int32 AddOutLineNum);
public:
	/*描边计数
	* 开启后在移入该被交互组件时描边计数+1，在移出该被交互组件时描边计数-1
	* 当描边计数 > 0时，显示描边
	*/
	UPROPERTY(BlueprintReadWrite)
	int32 OutLineCount = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	int32 CustomDepthStencilValue_OutLine = 255;
	//描边网格组件
	UPROPERTY(BlueprintReadWrite, Replicated)
	TArray<UPrimitiveComponent*> OutLineMeshComponnets;

	//哪些检测类型在进入的时候需要开启描边
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	TArray<EIS_InteractTraceType> EnterInteractTraceType;

	//当前已经记过数/进入的检测类型
	UPROPERTY(BlueprintReadWrite)
	TArray<EIS_InteractTraceType> CurEnterInteractTraceType;
};
