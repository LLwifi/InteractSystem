// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Common/IS_BeInteractInterface.h"
#include "Net/UnrealNetwork.h"
#include "IS_BeInteractExtendBase.generated.h"

class UIS_BeInteractComponent;

/*可被交互物的扩展基类
* 通常由UIS_BeInteractComponent 组件创建，相关的IIS_BeInteractInterface接口函数也会被UIS_BeInteractComponent同步在同名函数中调用
*/
UCLASS(Blueprintable, EditInlineNew, BlueprintType)
class INTERACTSYSTEM_API UIS_BeInteractExtendBase : public UObject , public IIS_BeInteractInterface
{
	GENERATED_BODY()

public:	
	UIS_BeInteractExtendBase();

	virtual UWorld* GetWorld() const override;

	virtual bool IsSupportedForNetworking() const override;

	/*初始化 通常由UIS_BeInteractComponent 组件创建时调用
	* BeInteractCom：哪个被交互组件创建的“我”
	* Data：初始化使用的数据表里的配置（与自身是一个类型）需要使用该数据初始化自身
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Init(UIS_BeInteractComponent* BeInteractCom, UIS_BeInteractExtendBase* Data);
	virtual void Init_Implementation(UIS_BeInteractComponent* BeInteractCom, UIS_BeInteractExtendBase* Data);

	virtual bool CanInteract_Implementation(UIS_InteractComponent* InteractComponent, FCC_CompareInfo OuterCompareInfo, FText& FailText) override;

	virtual void InteractEnter_Implementation(UIS_InteractComponent* InteractComponent, EIS_InteractTraceType TraceType) override;
	virtual void InteractLeave_Implementation(UIS_InteractComponent* InteractComponent, EIS_InteractTraceType TraceType) override;
	virtual bool InteractLeaveIsEnd_Implementation() override;
	virtual bool TryInteract_Implementation(UIS_InteractComponent* InteractComponent) override;
	virtual bool InteractCheck_Implementation(UIS_InteractComponent* InteractComponent) override;
	virtual void InteractStart_Implementation(UIS_InteractComponent* InteractComponent) override;
	virtual void InteractEnd_Implementation(UIS_InteractComponent* InteractComponent) override;
	virtual void InteractComplete_Implementation(UIS_InteractComponent* InteractComponent) override;
	virtual void InteractComplete_MultiSegment_Implementation(UIS_InteractComponent* InteractComponent) override;
	virtual void InteractAttachTo_Implementation(UIS_InteractComponent* InteractComponent) override;
	virtual void InteractAttachDetach_Implementation(UIS_InteractComponent* InteractComponent) override;

public:
	//正在使用该扩展的【可被交互组件】
	UPROPERTY(BlueprintReadWrite)
	UIS_BeInteractComponent* BeInteractComponent;
};
