// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "Common/IS_BeInterface.h"
#include "Net/UnrealNetwork.h"
#include "IS_BeInteractComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractDelegate, UIS_InteractComponent*, InteractComponent);

/*被交互的其他信息
* 通常是不需要配置，可能是代码或者逻辑层面需要保存的变量
*/
USTRUCT(BlueprintType)
struct FIS_BeInteractOtherInfo
{
	GENERATED_BODY()
public:
	//<角色签名,触发的TimeHandle>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, FTimerHandle> InteractTimerHandle;

	//验证人数的Timer
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTimerHandle InteractRoleNumVerifyTimerHandle;

};

/*被交互组件：该组件描述一个可以被交互的资源的基本信息
* 需要注意如果要进行网络同步，添加该组件的Actor也需要开启网络同步
*/
UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INTERACTSYSTEM_API UIS_BeInteractComponent : public UStaticMeshComponent, public IIS_BeInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UIS_BeInteractComponent();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//-----------------------------------------------------IIS_Interface
	virtual FIS_BeInteractInfo GetBeInteractInfo_Implementation() override;
	virtual FIS_BeInteractDynamicInfo GetBeInteractDynamicInfo_Implementation() override;
	virtual bool IsDisplayInteractText_Implementation() override;
	virtual FText GetInteractText_Implementation() override;
	virtual EIS_InteractType GetInteractType_Implementation() override;
	virtual int32 GetMultiInteractNum_Implementation() override;
	virtual TArray<float> GetInteractTime_Implementation(float& TotalTime) override;
	virtual float GetAlreadyInteractTime_Implementation() override;
	virtual int32 GetInteractNum_Implementation() override;
	virtual EIS_InteractNumSubtractType GetInteractNumSubtractType_Implementation() override;
	virtual FGameplayTagContainer GetInteractTag_Implementation() override;
	virtual int32 GetInteractPriority_Implementation() override;
	virtual bool SetInteractActive_Implementation(bool NewActive) override;
	virtual bool IsInteractActive_Implementation() override;

	virtual bool CanInteract_Implementation(UIS_InteractComponent* InteractComponent, FCC_CompareInfo OuterCompareInfo, FText& FailText) override;

	virtual bool InteractCompleteVerifyCheck_Implementation(UIS_InteractComponent* InteractComponent) override;
	virtual UObject* CreateVerifyObject_Implementation(UIS_InteractComponent* InteractComponent) override;
	virtual UUserWidget* CreateVerifyObject_UI_Implementation(UIS_InteractComponent* InteractComponent, TSubclassOf<UUserWidget> UIClass) override;
	virtual void InteractEnter_Implementation(UIS_InteractComponent* InteractComponent) override;
	//移入可交互目标——仅自主客户端触发该事件
	UPROPERTY(BlueprintAssignable)
	FInteractDelegate OnInteractEnter;
	virtual void InteractLeave_Implementation(UIS_InteractComponent* InteractComponent) override;
	//移出可交互目标——仅自主客户端触发该事件
	UPROPERTY(BlueprintAssignable)
	FInteractDelegate OnInteractLeave;
	virtual bool InteractLeaveIsEnd_Implementation() override;
	virtual bool TryInteract_Implementation(UIS_InteractComponent* InteractComponent) override;
	virtual bool InteractCheck_Implementation(UIS_InteractComponent* InteractComponent) override;
	virtual void InteractStart_Implementation(UIS_InteractComponent* InteractComponent) override;
	//开始交互
	UPROPERTY(BlueprintAssignable)
	FInteractDelegate OnInteractStart;
	virtual void InteractEnd_Implementation(UIS_InteractComponent* InteractComponent) override;
	//结束交互
	UPROPERTY(BlueprintAssignable)
	FInteractDelegate OnInteractEnd;
	virtual void InteractComplete_Implementation(UIS_InteractComponent* InteractComponent) override;
	//完成交互
	UPROPERTY(BlueprintAssignable)
	FInteractDelegate OnInteractComplete;
	virtual void InteractComplete_MultiSegment_Implementation(UIS_InteractComponent* InteractComponent) override;
	//多段完成交互
	UPROPERTY(BlueprintAssignable)
	FInteractDelegate OnInteractComplete_MultiSegment;

	virtual void InteractAttachTo_Implementation(UIS_InteractComponent* InteractComponent) override;
	//挂载交互
	UPROPERTY(BlueprintAssignable)
	FInteractDelegate OnInteractAttachTo;
	virtual void InteractAttachDetach_Implementation(UIS_InteractComponent* InteractComponent) override;
	//分离挂载交互
	UPROPERTY(BlueprintAssignable)
	FInteractDelegate OnInteractAttachDetach;
	//-----------------------------------------------------IIS_Interface

	/*创建交互验证对象——UI
	* 需要注意的是UI只能在客户端创建
	*/
	UFUNCTION(NetMulticast, Reliable)
	void CreateVerifyUI(UIS_InteractComponent* InteractComponent, TSubclassOf<UUserWidget> UIClass);

	/*通过签名获取某个交互者当前已经交互的时间
	* 若交互方式允许多人累加返回BeInteractDynamicInfo.InteractCumulativeTime
	* 否则返回该签名角色的个人交互时间
	*/
	UFUNCTION(BlueprintPure)
	float GetCurInteractTimeFromRoleSign(FName RoleSign);

	/*交互尝试完成，此函数被调用表示基本的交互过程已经完成
	* 若交互验证通过则交互完成
	* return : 验证是否通过/是否成功调用了交互完成
	*/
	UFUNCTION(BlueprintCallable)
	bool TryInteractComplete(UIS_InteractComponent* InteractComponent);

	//获取“我”的前方与目标向量/方向之间的夹角
	UFUNCTION(BlueprintPure)
	float GetAngleFromTargetDir(FVector TargetDir);

	UFUNCTION()
	void InteractTimerBack();

	/*人数验证间隔检测
	* 当人数验证未通过时，开启该Timer进行间隔验证
	* 验证成功或无人交互时停止验证
	*/
	UFUNCTION()
	void InteractRoleNumVerifyBack();

	//获取多段累计（MSCT/MultiSegmentCumulativeTime）交互下的下一次交互所需时间
	UFUNCTION()
	float Get_MSCT_NextInteractTime();

	/*在多个交互者中寻找完成交互的交互者
	* BackTime：回调的时间
	*/
	UFUNCTION()
	UIS_InteractComponent* FindCompleteInteractComponent(float BackTime);

public:
	//被交互的信息
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIS_BeInteractInfo BeInteractInfo;

	/*场景Actor拾取器
	* 该变量仅用来拾取场景中的Actor，无任何其余作用
	拾取后可以把该值复制给BeInteractDynamicInfo中的InteractVerifyObject（交互验证对象）
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* SceneActorPickup;

	//被交互的信息
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	FIS_BeInteractDynamicInfo BeInteractDynamicInfo;

	//被交互的其他信息
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIS_BeInteractOtherInfo BeInteractOtherInfo;

	//不同交互者的不同累计时间，由于Map不可复制，该值仅在服务器上存在
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TMap<FName,float> ServerInteracterCumulativeTime;
};
