// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Common/IS_BeInteractInterface.h"
#include "Net/UnrealNetwork.h"
#include "IS_BeInteractExtendBase.generated.h"

class UIS_BeInteractComponent;

/*可被交互物的扩展基类
* 该类主要是绑定可被交互组件的各个时机事件
*/
UCLASS(Blueprintable, EditInlineNew, BlueprintType)
class INTERACTSYSTEM_API UIS_BeInteractExtendBase : public UObject , public IIS_BeInteractInterface
{
	GENERATED_BODY()

public:	
	UIS_BeInteractExtendBase();

	virtual UWorld* GetWorld() const override;

	virtual bool IsSupportedForNetworking() const override;

	//初始化
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
