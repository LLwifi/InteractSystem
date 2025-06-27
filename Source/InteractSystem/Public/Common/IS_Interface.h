#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "IS_Interface.generated.h"

class UIS_InteractComponent;

//被交互的类型
UENUM(BlueprintType)
enum class EIS_InteractType :uint8
{
	Instant = 0 UMETA(DisplayName = "瞬间交互"),
	HasDuration UMETA(DisplayName = "单次持续交互"),
	MultiSegment UMETA(DisplayName = "多段持续交互")
};

//交互次数扣除类型
UENUM(BlueprintType)
enum class EIS_InteractNumSubtractType :uint8
{
	NotSubtract = 0 UMETA(DisplayName = "不扣除"),
	Complete UMETA(DisplayName = "交互完成时扣除"),
	End UMETA(DisplayName = "交互结束时扣除")
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UIS_Interface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 可被交互单位的接口
 */
class INTERACTSYSTEM_API IIS_Interface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/*
	* 获取交互显示文本
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		FText GetInteractText();
	virtual FText GetInteractText_Implementation(){ return FText(); };

	/*
	* 获取交互失败的显示文本
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		FText GetInteractFailText();
	virtual FText GetInteractFailText_Implementation() { return FText(); };

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
	* 获取交互时长
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	TArray<float> GetInteractTime();
	virtual TArray<float> GetInteractTime_Implementation() { return { 1.0f }; };

	/*
	* 获取交互次数
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 GetInteractNum();
	virtual int32 GetInteractNum_Implementation() { return 1; };

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

	//-------------------------------------以下是事件相关接口

	/*
	* 移入可交互目标
	* EnterInteractCom：哪个交互组件移入了“我”
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InteractEnter(UIS_InteractComponent* EnterInteractCom);
	virtual void InteractEnter_Implementation(UIS_InteractComponent* EnterInteractCom) {};

	/*
	* 移出可交互目标
	* EnterInteractCom：哪个交互组件移出了“我”
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InteractLeave(UIS_InteractComponent* EnterInteractCom);
	virtual void InteractLeave_Implementation(UIS_InteractComponent* EnterInteractCom) {};

	/*
	* 尝试交互：该函数会调用InteractCheck，InteractCheck返回为true时调用InteractStart
	* EnterInteractCom：哪个交互组件向我发起交互请求
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool TryInteract(UIS_InteractComponent* EnterInteractCom);
	virtual bool TryInteract_Implementation(UIS_InteractComponent* EnterInteractCom) { return true; };

	/*
	* 交互检测：是否达到可交互的条件
	* EnterInteractCom：哪个交互组件向我发起交互请求
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool InteractCheck(UIS_InteractComponent* EnterInteractCom);
	virtual bool InteractCheck_Implementation(UIS_InteractComponent* EnterInteractCom) { return true; };

	/*
	* 开始交互：InteractCheck通过后调用该函数
	* EnterInteractCom：哪个交互组件向我发起交互请求
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InteractStart(UIS_InteractComponent* EnterInteractCom);
	virtual void InteractStart_Implementation(UIS_InteractComponent* EnterInteractCom) {};

	/*
	* 结束交互
	* EnterInteractCom：哪个交互组件向我发起交互请求
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InteractEnd(UIS_InteractComponent* EnterInteractCom);
	virtual void InteractEnd_Implementation(UIS_InteractComponent* EnterInteractCom) {};
};
