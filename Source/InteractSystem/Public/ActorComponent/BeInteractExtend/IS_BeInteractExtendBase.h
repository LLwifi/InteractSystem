// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Common/IS_BeInteractInterface.h"
#include "Net/UnrealNetwork.h"
#include "IS_BeInteractExtendBase.generated.h"

class UIS_BeInteractComponent;

/*可被交互物的扩展基类
* 通常由UIS_BeInteractComponent 组件创建，相关的IIS_BeInteractInterface接口函数也会被UIS_BeInteractComponent同步在同名函数中调用
* 子类需要重载Init函数对需要配置获取的参数进行赋值
*/
UCLASS(Blueprintable, EditInlineNew, BlueprintType)
class INTERACTSYSTEM_API UIS_BeInteractExtendBase : public UObject , public IIS_BeInteractInterface
{
	GENERATED_BODY()

public:	
	UIS_BeInteractExtendBase();

	virtual UWorld* GetWorld() const override;

	virtual bool IsSupportedForNetworking() const override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	//UFUNCTION()
	//void ReplicatedUsing_bIsInit();

	UFUNCTION()
	void ReplicatedUsing_NetType();

	//UFUNCTION()
	//void ReplicatedUsing_BeInteractComponent();

	/*初始化网络检测 通常由UIS_BeInteractComponent 组件创建时在服务器由CreateBeInteractExtendFromHandle函数调用
	* 该函数决定Init在哪个环境下被调用
	*/
	UFUNCTION()
	void Init_RepCheck(UIS_BeInteractComponent* BeInteractCom, UIS_BeInteractExtendBase* Data);

	/*初始化 通常由UIS_BeInteractComponent 组件创建时在服务器由CreateBeInteractExtendFromHandle函数调用
	* 该函数也会受到NetType的影响来决定ini的调用环境，需要注意的是因为同步需要时间，不同环境下的init调用时机可能会有间隔差距
	* BeInteractCom：哪个被交互组件创建的“我”
	* Data：初始化使用的数据表里的配置（与自身是一个类型）需要使用该数据初始化自身
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Init(UIS_BeInteractComponent* BeInteractCom, UIS_BeInteractExtendBase* Data);
	virtual void Init_Implementation(UIS_BeInteractComponent* BeInteractCom, UIS_BeInteractExtendBase* Data);

	virtual bool CanInteract_Implementation(UIS_InteractComponent* InteractComponent, FCC_CompareInfo OuterCompareInfo, FText& FailText) override;

	virtual void InteractEnter_Implementation(UIS_InteractComponent* InteractComponent, FGameplayTag TraceTypeTag) override;
	virtual void InteractLeave_Implementation(UIS_InteractComponent* InteractComponent, FGameplayTag TraceTypeTag) override;
	virtual bool InteractLeaveIsEnd_Implementation() override;
	virtual void InteractStart_Implementation(UIS_InteractComponent* InteractComponent, FGameplayTag TraceTypeTag) override;
	virtual void InteractEnd_Implementation(UIS_InteractComponent* InteractComponent, FGameplayTag TraceTypeTag) override;
	virtual void InteractComplete_Implementation(UIS_InteractComponent* InteractComponent, FGameplayTag TraceTypeTag) override;
	virtual void InteractComplete_MultiSegment_Implementation(UIS_InteractComponent* InteractComponent, FGameplayTag TraceTypeTag) override;
	virtual void InteractAttachTo_Implementation(UIS_InteractComponent* InteractComponent, FGameplayTag TraceTypeTag) override;
	virtual void InteractAttachDetach_Implementation(UIS_InteractComponent* InteractComponent, FGameplayTag TraceTypeTag) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Meta = (ExposeOnSpawn = True))
	FIS_BeInteractExtendHandle ExtendHandle;

	/*该扩展的相关事件在哪个网络下运行
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = ReplicatedUsing_NetType)
	EIS_InteractEventNetType NetType = EIS_InteractEventNetType::Server;

	//正在使用该扩展的【可被交互组件】
	UPROPERTY(BlueprintReadWrite, Replicated)
	UIS_BeInteractExtendBase* DTData;

	//正在使用该扩展的【可被交互组件】
	UPROPERTY(BlueprintReadWrite, Replicated /*,ReplicatedUsing = ReplicatedUsing_BeInteractComponent*/)
	UIS_BeInteractComponent* BeInteractComponent;
};
