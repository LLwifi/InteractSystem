#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include <IS_StructAndEnum.h>
#include "IS_BeInteractInterface.generated.h"

//交互相关事件的网络复制决策类型
UENUM(BlueprintType)
enum class EIS_InteractEventNetType :uint8
{
	Server UMETA(DisplayName = "在服务器上运行"),
	Client UMETA(DisplayName = "在拥有的客户端上运行"),
	NetMulticast UMETA(DisplayName = "组播")
};

//被交互接口函数类型枚举
UENUM(BlueprintType)
enum class EIS_BeInteractInterfaceType :uint8
{
	InteractStart UMETA(DisplayName = "InteractStart"),
	InteractEnd UMETA(DisplayName = "InteractEnd"),
	InteractComplete UMETA(DisplayName = "InteractComplete"),
	InteractComplete_MultiSegment UMETA(DisplayName = "InteractComplete_MultiSegment"),
	InteractAttachTo UMETA(DisplayName = "InteractAttachTo"),
	InteractAttachDetach UMETA(DisplayName = "InteractAttachDetach")
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UIS_BeInteractVerifyInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 被交互验证接口
 */
class INTERACTSYSTEM_API IIS_BeInteractVerifyInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/*开始交互验证
	* 调用该函数后需要验证对象自行在后续的内容中处理BeInteractComponent是否完成交互
	* 确定完成时调用BeInteractComponent的InteractComplete接口
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InteractVerifyStart(UIS_InteractComponent* InteractComponent, UIS_BeInteractComponent* BeInteractComponent);
	virtual void InteractVerifyStart_Implementation(UIS_InteractComponent* InteractComponent, UIS_BeInteractComponent* BeInteractComponent) {};
};



// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UIS_BeInteractInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 可被交互单位的接口
 */
class INTERACTSYSTEM_API IIS_BeInteractInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	//获取被交互的信息
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FIS_BeInteractInfo GetBeInteractInfo();
	virtual FIS_BeInteractInfo GetBeInteractInfo_Implementation() { return FIS_BeInteractInfo(); };

	//获取被交互的动态信息
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FIS_BeInteractDynamicInfo GetBeInteractDynamicInfo();
	virtual FIS_BeInteractDynamicInfo GetBeInteractDynamicInfo_Implementation() { return FIS_BeInteractDynamicInfo(); };

	/*
	* 当前是否可以显示交互文本
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsDisplayInteractText();
	virtual bool IsDisplayInteractText_Implementation() { return true; };

	/*
	* 获取交互显示文本
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		FText GetInteractText();
	virtual FText GetInteractText_Implementation(){ return FText(); };
	/*
	* 设置交互显示文本
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetInteractText(const FText& InteractText);
	virtual void SetInteractText_Implementation(const FText& InteractText) {};

	/*
	* 获取交互类型
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	EIS_InteractType GetInteractType();
	virtual EIS_InteractType GetInteractType_Implementation() { return EIS_InteractType::Instant; };

	/*
	* 获取多段交互次数
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 GetMultiInteractNum();
	virtual int32 GetMultiInteractNum_Implementation() { return 2; };

	/*
	* 获取要交互时长
	* TotalTime：总时长
	* return：每一段的交互时长
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	TArray<float> GetInteractTime(float& TotalTime);
	virtual TArray<float> GetInteractTime_Implementation(float& TotalTime) { TotalTime = 1.0f;return { 1.0f }; };

	/*
	* 设置要交互时长
	* return：总时长
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	float SetInteractTime(const TArray<float>& InteractTime);
	virtual float SetInteractTime_Implementation(const TArray<float>& InteractTime) { return 0.0f; };

	/*
	* 获取已经交互过的时长
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	float GetAlreadyInteractTime();
	virtual float GetAlreadyInteractTime_Implementation() { return 0.0f; };

	/*
	* 获取交互次数
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 GetInteractNum();
	virtual int32 GetInteractNum_Implementation() { return 1; };

	/*
	* 设置交互次数
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 SetInteractNum(int32 NewInteractNum);
	virtual int32 SetInteractNum_Implementation(int32 NewInteractNum) { return NewInteractNum; };

	/*
	* 获取交互次数扣除类型
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	EIS_InteractNumSubtractType GetInteractNumSubtractType();
	virtual EIS_InteractNumSubtractType GetInteractNumSubtractType_Implementation() { return EIS_InteractNumSubtractType::NotSubtract; };

	/*
	* 获取交互次数扣除类型
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FGameplayTagContainer GetInteractTag();
	virtual FGameplayTagContainer GetInteractTag_Implementation() { return FGameplayTagContainer(); };

	/*
	* 获取交互优先级
	* 该值越大交互优先级越高
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 GetInteractPriority();
	virtual int32 GetInteractPriority_Implementation() { return 0; };

	//开启/激活交互
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool SetInteractActive(bool NewActive);
	virtual bool SetInteractActive_Implementation(bool NewActive) { return NewActive; };

	//是否激活了交互
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsInteractActive();
	virtual bool IsInteractActive_Implementation() { return true; };

	/*
	* 是否允许交互，只在玩家尝试交互时进行判断，判断可能还会经过交互验证
	* 至少要保证可被交互的资源IsEnableInteract（开启/激活了交互）& GetInteractNum（仍有剩余交互次数）后
	* 如果允许多人交互还需要判断交互人数 后
	* 再进行额外的比对，当比对通过时，允许交互
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool CanInteract(UIS_InteractComponent* InteractComponent, FCC_CompareInfo OuterCompareInfo, FText& FailText);
	virtual bool CanInteract_Implementation(UIS_InteractComponent* InteractComponent, FCC_CompareInfo OuterCompareInfo, FText& FailText) { return true; };

	/*
	* 设置交互完成时的验证信息
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FIS_InteractCompleteVerifyInfo SetInteractCompleteVerifyInfo(FIS_InteractCompleteVerifyInfo NewInteractCompleteVerifyInfo);
	virtual FIS_InteractCompleteVerifyInfo SetInteractCompleteVerifyInfo_Implementation(FIS_InteractCompleteVerifyInfo NewInteractCompleteVerifyInfo) { return NewInteractCompleteVerifyInfo; };

	//-------------------------------------以下是事件相关接口

	/*交互完成时的额外验证
	* 该函数会判断是否需要进行额外的验证，包括
	* 验证对象检测、子类覆写的其他成功验证逻辑
	* return：返回会true时表示验证通过，返回false表示验证不通过或有其他待验证的事项
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool InteractCompleteVerifyCheck(UIS_InteractComponent* InteractComponent);
	virtual bool InteractCompleteVerifyCheck_Implementation(UIS_InteractComponent* InteractComponent) { return true; };

	/*创建交互验证对象
	* 需要判断class是UI还是Object还是Actor
	* 需要注意的是UI只能在客户端创建
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UObject* CreateVerifyObject(UIS_InteractComponent* InteractComponent);
	virtual UObject* CreateVerifyObject_Implementation(UIS_InteractComponent* InteractComponent) { return nullptr; };

	/*创建交互验证对象——UI
	* 需要注意的是UI只能在客户端创建
	* 该函数在BeInteractComponent上会被多播
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UUserWidget* CreateVerifyObject_UI(UIS_InteractComponent* InteractComponent, TSubclassOf<UUserWidget> UIClass);
	virtual UUserWidget* CreateVerifyObject_UI_Implementation(UIS_InteractComponent* InteractComponent, TSubclassOf<UUserWidget> UIClass) { return nullptr; };

	/*
	* 移入可交互目标——仅在客户端触发
	* InteractComponent：哪个交互组件移入了“我”
	* TraceType：进入时是因为被哪个类型检测到了
	* TopPriority：我被触发移入时，“我”是不是交互优先级最高的对象
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InteractEnter(UIS_InteractComponent* InteractComponent, EIS_InteractTraceType TraceType);
	virtual void InteractEnter_Implementation(UIS_InteractComponent* InteractComponent, EIS_InteractTraceType TraceType) {};

	/*
	* 移出可交互目标——仅在客户端触发
	* InteractComponent：哪个交互组件移出了“我”
	* TraceType：离开时是因为被哪个类型触发的
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InteractLeave(UIS_InteractComponent* InteractComponent, EIS_InteractTraceType TraceType);
	virtual void InteractLeave_Implementation(UIS_InteractComponent* InteractComponent, EIS_InteractTraceType TraceType) {};

	/*
	* 在移出被交互物时，要不要停止交互
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool InteractLeaveIsEnd();
	virtual bool InteractLeaveIsEnd_Implementation() { return true; };

	/*
	* 开始交互：InteractCheck通过后调用该函数
	* InteractComponent：哪个交互组件向我发起交互请求
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InteractStart(UIS_InteractComponent* InteractComponent);
	virtual void InteractStart_Implementation(UIS_InteractComponent* InteractComponent) {};

	/*
	* 结束交互
	* InteractComponent：哪个交互组件向我发起交互请求
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InteractEnd(UIS_InteractComponent* InteractComponent);
	virtual void InteractEnd_Implementation(UIS_InteractComponent* InteractComponent) {};

	/*
	* 完成交互——所有的交互条件和验证均通过时，该函数会被调用
	* InteractComponent：哪个交互组件向我发起交互请求
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InteractComplete(UIS_InteractComponent* InteractComponent);
	virtual void InteractComplete_Implementation(UIS_InteractComponent* InteractComponent) {};

	/*
	* 多段完成交互——当交互时长的数组长度 > 1时，在交互过程中达到某个时长后该函数会被调用
	* InteractComponent：哪个交互组件向我发起交互请求
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InteractComplete_MultiSegment(UIS_InteractComponent* InteractComponent);
	virtual void InteractComplete_MultiSegment_Implementation(UIS_InteractComponent* InteractComponent) {};

	/*
	* 交互挂载——交互类型为AttachTo（挂载交互）时，完成交互的时候会调用该接口表示交互进入
	* InteractComponent：哪个交互组件向我发起交互请求
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InteractAttachTo(UIS_InteractComponent* InteractComponent);
	virtual void InteractAttachTo_Implementation(UIS_InteractComponent* InteractComponent) {};

	/*
	* 分离挂载交互——交互类型为AttachTo（挂载交互）时，需要主动调用该接口表示交互退出
	* InteractComponent：哪个交互组件向我发起交互请求
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InteractAttachDetach(UIS_InteractComponent* InteractComponent);
	virtual void InteractAttachDetach_Implementation(UIS_InteractComponent* InteractComponent) {};
};
