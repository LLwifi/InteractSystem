// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorComponent/BeInteractExtend/IS_BIEGetActor.h"
#include "IS_BIEInteractLock.generated.h"

USTRUCT(BlueprintType)
struct FIS_BIELockInfo
{
	GENERATED_BODY()
public:
	FIS_BIELockInfo(){}
	FIS_BIELockInfo(AActor* Target, float Time, FTimerHandle TimerHandle)
	{
		LockTarget = Target;
		LockTime = Time;
		LockTimerHandle = TimerHandle;
	}

	bool operator ==(const FIS_BIELockInfo& LockInfo) const
	{
		return LockTarget == LockInfo.LockTarget && LockTime == LockInfo.LockTime/* && LockTimerHandle == LockInfo.LockTimerHandle*/;
	}
public:
	//被锁定的目标
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	AActor* LockTarget;
	//锁定的时长
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float LockTime;
	//锁定的时间记录
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FTimerHandle LockTimerHandle;
};

/**可被交互物的功能扩展——交互锁
 * 锁定某些对象无法进行交互
 */
UCLASS()
class INTERACTSYSTEM_API UIS_BIEInteractLock : public UIS_BIEGetActor
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
	virtual void Init_Implementation(UIS_BeInteractComponent* BeInteractCom, UIS_BeInteractExtendBase* Data) override;

	bool CanInteract_Implementation(UIS_InteractComponent* InteractComponent, FCC_CompareInfo OuterCompareInfo, FText& FailText) override;

	/*重置锁定
	* IsOverrideLockTime : 是否重载锁定时长
	* 该值为true时才会使用OverrideLockTime参数
	* 该值为false时使用配置的LockTimeConfig
	* OverrideLockTime ：如果IsOverrideLockTime = true 会把该值赋予LockTime
	*/
	UFUNCTION(BlueprintCallable)
	void ReSetLock(bool IsOverrideLockTime = false, float OverrideLockTime = 3.0f);

	//锁定时间结束回调
	UFUNCTION()
	void LockBack();

	//添加一个锁定
	UFUNCTION(BlueprintCallable)
	void AddLock(AActor* LockTarget, float LockTime = 3.0f);

	//判断这个对象是否还处于锁定中
	UFUNCTION(BlueprintCallable)
	bool ActorLockCheck(AActor* Target, FIS_BIELockInfo& LockInfo);

	UFUNCTION()
	void DelayRefreshTimeBack();
public:
	/*锁定的时长配置
	* > 0 时表示锁定多少秒
	* = 0 时表示无/以锁定
	* < 0 时表示永久锁定
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Meta = (ExposeOnSpawn = True))
	float LockTimeConfig = 3.0f;

	//锁定时的交互失败提示文本
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Meta = (ExposeOnSpawn = True))
	FText LockFailText;

	//是否要实时刷新/获取Actor 该值为true时在每次ReSetLock的时候都会刷新/获取描边组件
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Meta = (ExposeOnSpawn = True))
	bool RealTimeRefresh = false;
	//延迟刷新/获取Actor的时间，该值<0时会在init函数中直接获取并刷新
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, meta = (EditCondition = "!RealTimeRefresh", ExposeOnSpawn = True))
	float DelayRefreshTime = 0.0f;
	UPROPERTY()
	FTimerHandle DelayRefreshTimeHandle;

	//不同的Actor，剩余锁定的时间
	UPROPERTY(BlueprintReadWrite, Replicated)
	TArray<FIS_BIELockInfo> AllLockInfo;

	//任务时长
	UPROPERTY(BlueprintReadWrite)
	FTimerHandle TaskTimeHandle;
};
