// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "Common/IS_BeInteractInterface.h"
#include "Net/UnrealNetwork.h"
#include "IS_BeInteractComponent.generated.h"

class UIS_BeInteractExtendBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractDelegate, UIS_InteractComponent*, InteractComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInteractVerifyDelegate, UIS_InteractComponent*, InteractComponent, UObject*, VerifyObject);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInteractEnterOrLeaveDelegate, UIS_InteractComponent*, InteractComponent, EIS_InteractTraceType, TraceType);

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

//交互相关事件的网络复制决策类型
UENUM(BlueprintType)
enum class EIS_InteractEventNetType :uint8
{
	Server UMETA(DisplayName = "在服务器上运行"),
	Client UMETA(DisplayName = "在拥有的客户端上运行"),
	NetMulticast UMETA(DisplayName = "组播")
};

/*被交互组件：该组件描述一个可以被交互的资源的基本信息
* 需要注意如果要进行网络同步，添加该组件的Actor也需要开启网络同步
* 待解决问题：扩展完全由外部管理的开始-结束-完成	交互CD	交互锁定		交互选择/多选		交互速度
*/
UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INTERACTSYSTEM_API UIS_BeInteractComponent : public UStaticMeshComponent, public IIS_BeInteractInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UIS_BeInteractComponent();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//-----------------------------------------------------IIS_Interface
	virtual FIS_BeInteractInfo GetBeInteractInfo_Implementation() override;
	virtual FIS_BeInteractDynamicInfo GetBeInteractDynamicInfo_Implementation() override;
	virtual bool IsDisplayInteractText_Implementation() override;
	virtual FText GetInteractText_Implementation() override;
	virtual void SetInteractText_Implementation(const FText& InteractText) override;
	virtual EIS_InteractType GetInteractType_Implementation() override;
	virtual int32 GetMultiInteractNum_Implementation() override;
	virtual TArray<float> GetInteractTime_Implementation(float& TotalTime) override;
	virtual float GetAlreadyInteractTime_Implementation() override;
	virtual float SetInteractTime_Implementation(const TArray<float>& InteractTime) override;
	virtual int32 GetInteractNum_Implementation() override;
	virtual int32 SetInteractNum_Implementation(int32 NewInteractNum) override;
	virtual EIS_InteractNumSubtractType GetInteractNumSubtractType_Implementation() override;
	virtual FGameplayTagContainer GetInteractTag_Implementation() override;
	virtual int32 GetInteractPriority_Implementation() override;
	virtual bool SetInteractActive_Implementation(bool NewActive) override;
	virtual bool IsInteractActive_Implementation() override;

	virtual bool CanInteract_Implementation(UIS_InteractComponent* InteractComponent, FCC_CompareInfo OuterCompareInfo, FText& FailText) override;
	//开始进行第三方验证了
	UPROPERTY(BlueprintAssignable)
	FInteractVerifyDelegate OnInteractVerify;

	virtual FIS_InteractCompleteVerifyInfo SetInteractCompleteVerifyInfo_Implementation(FIS_InteractCompleteVerifyInfo NewInteractCompleteVerifyInfo) override;

	virtual bool InteractCompleteVerifyCheck_Implementation(UIS_InteractComponent* InteractComponent) override;
	virtual UObject* CreateVerifyObject_Implementation(UIS_InteractComponent* InteractComponent) override;
	virtual UUserWidget* CreateVerifyObject_UI_Implementation(UIS_InteractComponent* InteractComponent, TSubclassOf<UUserWidget> UIClass) override;
	virtual void InteractEnter_Implementation(UIS_InteractComponent* InteractComponent, EIS_InteractTraceType TraceType) override;
	//移入可交互目标——仅自主客户端触发该事件
	UPROPERTY(BlueprintAssignable)
	FInteractEnterOrLeaveDelegate OnInteractEnter;
	virtual void InteractLeave_Implementation(UIS_InteractComponent* InteractComponent, EIS_InteractTraceType TraceType) override;
	//移出可交互目标——仅自主客户端触发该事件
	UPROPERTY(BlueprintAssignable)
	FInteractEnterOrLeaveDelegate OnInteractLeave;
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

	//-----------------------------------------------------------------------------------------Net
	UFUNCTION(Client, Reliable)
	void NetClient_OnInteractVerify(UIS_InteractComponent* InteractComponent, UObject* VerifyObject);
	UFUNCTION(NetMulticast, Reliable)
	void NetMulti_OnInteractVerify(UIS_InteractComponent* InteractComponent, UObject* VerifyObject);

	UFUNCTION(Client, Reliable)
	void NetClient_OnInteractEnter(UIS_InteractComponent* InteractComponent, EIS_InteractTraceType TraceType);
	UFUNCTION(NetMulticast, Reliable)
	void NetMulti_OnInteractEnter(UIS_InteractComponent* InteractComponent, EIS_InteractTraceType TraceType);

	UFUNCTION(Client, Reliable)
	void NetClient_OnInteractLeave(UIS_InteractComponent* InteractComponent, EIS_InteractTraceType TraceType);
	UFUNCTION(NetMulticast, Reliable)
	void NetMulti_OnInteractLeave(UIS_InteractComponent* InteractComponent, EIS_InteractTraceType TraceType);

	UFUNCTION(Client, Reliable)
	void NetClient_OnInteractStart(UIS_InteractComponent* InteractComponent);
	UFUNCTION(NetMulticast, Reliable)
	void NetMulti_OnInteractStart(UIS_InteractComponent* InteractComponent);

	UFUNCTION(Client, Reliable)
	void NetClient_OnInteractEnd(UIS_InteractComponent* InteractComponent);
	UFUNCTION(NetMulticast, Reliable)
	void NetMulti_OnInteractEnd(UIS_InteractComponent* InteractComponent);

	UFUNCTION(Client, Reliable)
	void NetClient_OnInteractComplete(UIS_InteractComponent* InteractComponent);
	UFUNCTION(NetMulticast, Reliable)
	void NetMulti_OnInteractComplete(UIS_InteractComponent* InteractComponent);

	UFUNCTION(Client, Reliable)
	void NetClient_OnInteractComplete_MultiSegment(UIS_InteractComponent* InteractComponent);
	UFUNCTION(NetMulticast, Reliable)
	void NetMulti_OnInteractComplete_MultiSegment(UIS_InteractComponent* InteractComponent);

	UFUNCTION(Client, Reliable)
	void NetClient_OnInteractAttachTo(UIS_InteractComponent* InteractComponent);
	UFUNCTION(NetMulticast, Reliable)
	void NetMulti_OnInteractAttachTo(UIS_InteractComponent* InteractComponent);

	UFUNCTION(Client, Reliable)
	void NetClient_OnInteractAttachDetach(UIS_InteractComponent* InteractComponent);
	UFUNCTION(NetMulticast, Reliable)
	void NetMulti_OnInteractAttachDetach(UIS_InteractComponent* InteractComponent);

	//-----------------------------------------------------------------------------------------Net
	/*通过配置名称获取蒙太奇
	* return : -1表示未成功获取
	*/
	UFUNCTION(BlueprintPure)
	UAnimMontage* GetMontageFromKeyName(FName KeyName = FName("Default"));

	/*通过片段下标获取交互蒙太奇某段动画的时长
	* return : -1表示未成功获取
	*/
	UFUNCTION(BlueprintPure)
	float GetInteractMontageSectionLengthFromIndex(UAnimMontage* Montage, int32 SectionIndex);

	/*通过片段名称获取交互蒙太奇某段动画的时长
	* return : -1表示未成功获取
	*/
	UFUNCTION(BlueprintPure)
	float GetInteractMontageSectionLengthFromName(UAnimMontage* Montage, FName SectionName);

	/*Trace的进入和移出
	* 该函数可能会被频繁调用
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void InteractTraceCheck(EIS_InteractTraceType TraceType, bool IsEnter = true);
	virtual void InteractTraceCheck_Implementation(EIS_InteractTraceType TraceType, bool IsEnter = true);

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
	* 若交互验证通过（InteractCompleteVerifyCheck）则交互完成
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

	/*在多个交互者中寻找完成交互的交互者
	* BackTime：回调的时间
	*/
	UFUNCTION()
	UIS_InteractComponent* FindCompleteInteractComponent(float BackTime);

	/*检测类型是否通过 允许该类型的检测且没有触发过“进入”事件时才会返回true
	* BeInteractInfo.InteractEnterTriggerType 不包含该类型表示不接受该类型的检测
	* IsEnter：这次判断是进入还是离开
	* 该值为true时，表示进入——BeInteractDynamicInfo.AllEnterTraceType 未包含该类型表示该类型可以触发“进入”事件
	* 该值为false时，表示离开——BeInteractDynamicInfo.AllEnterTraceType 包含该类型表示该类型可以触发“离开”事件
	*/
	UFUNCTION(BlueprintPure)
	bool TraceTypeCheck(EIS_InteractTraceType TraceType, bool IsEnter = true);

	//检测全部扩展类是否允许交互
	UFUNCTION(BlueprintPure)
	bool CanInteract_Extend(UIS_InteractComponent* InteractComponent, FCC_CompareInfo OuterCompareInfo, FText& FailText);
public:
	/*交互相关事件的网络复制决策
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EIS_InteractEventNetType InteractEventNetType = EIS_InteractEventNetType::Server;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (PinHiddenByDefault, InlineEditConditionToggle))
	bool bIsUseDataTable;
	//被交互信息是否读取数据表
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bIsUseDataTable"))
	FIS_BeInteractInfoHandle BeInteractInfoHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (PinHiddenByDefault, InlineEditConditionToggle))
	bool bIsOverrideBeInteractExtend;
	/*被交互扩展信息是否需要重载
	* 开始该选项后最终的扩展由该值
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bIsOverrideBeInteractExtend"))
	TArray<FIS_BeInteractExtendHandle> OverrideBeInteractExtendHandles;

	//额外添加的扩展
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FIS_BeInteractExtendHandle> AddBeInteractExtendHandles;

	//被交互的信息
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIS_BeInteractInfo BeInteractInfo;

	/*场景Actor拾取器
	* 该变量仅用来拾取场景中的Actor，无任何其余作用
	拾取后可以把该值复制给BeInteractDynamicInfo中的InteractVerifyObject（交互验证对象）
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* SceneActorPickup;

	//被交互的动态信息
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	FIS_BeInteractDynamicInfo BeInteractDynamicInfo;

	//被交互的其他信息
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIS_BeInteractOtherInfo BeInteractOtherInfo;

	//不同交互者的不同累计时间，由于Map不可复制，该值仅在服务器上存在
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TMap<FName,float> ServerInteracterCumulativeTime;

	//全部扩展
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated)
	TArray<UIS_BeInteractExtendBase*> AllExtend;
	
};
