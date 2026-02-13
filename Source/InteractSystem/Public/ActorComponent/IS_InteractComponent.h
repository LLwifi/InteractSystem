// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <GameplayTagContainer.h>
#include "Net/UnrealNetwork.h"
#include "../../../../../CommonCompare/Source/CommonCompare/Public/CC_StructAndEnum.h"
#include "Kismet/KismetSystemLibrary.h"//EDrawDebugTrace需要
#include "Common/IS_BeInteractInterface.h"//EIS_InteractTraceType需要
#include "IS_InteractComponent.generated.h"

class UIS_BeInteractComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FIS_InteractEvent, UIS_InteractComponent*, InteractComponent, UIS_BeInteractComponent*, BeInteractComponent, FGameplayTag, TraceTypeTag);

USTRUCT(BlueprintType)
struct FIS_BeInteractComponentArray
{
	GENERATED_BODY()
public:
	FIS_BeInteractComponentArray(){}
	FIS_BeInteractComponentArray(TArray<UIS_BeInteractComponent*> CpmArray) { ComponentArray = CpmArray; }
	void operator=(TArray<UIS_BeInteractComponent*> CpmArray) { ComponentArray = CpmArray; }
	UIS_BeInteractComponent* operator[](const int32& Index){ return ComponentArray[Index]; }
	void Add(UIS_BeInteractComponent* Com){ ComponentArray.Add(Com); }
	void Remove(UIS_BeInteractComponent* Com) { ComponentArray.Remove(Com); }
	void RemoveAt(int32 Index) { ComponentArray.RemoveAt(Index); }
public:
	//当前检测到的全部组件
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<UIS_BeInteractComponent*> ComponentArray;
};

/*交互组件：该组件拥有交互其他资源的能力
*/
UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INTERACTSYSTEM_API UIS_InteractComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UIS_InteractComponent();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	//获取签名
	UFUNCTION(BlueprintPure)
	FName GetRoleSign();

	//开始交互
	UFUNCTION(BlueprintCallable)
	bool StartInteract(UIS_BeInteractComponent* BeInteractComponent, FGameplayTag TraceTypeTag);

	/*结束交互
	* 对当前正在交互的组件进行结束交互（通过函数StartInteract触发交互的组件为当前正在交互的组件）
	*/
	UFUNCTION(BlueprintCallable)
	void EndCurInteract(FGameplayTag TraceTypeTag);

	//主动交互验证
	UFUNCTION(BlueprintPure)
	bool InteractVerify(UIS_BeInteractComponent* BeInteractComponent, FGameplayTag TraceType, FText& FailText);

	//处理射线命中的内容，提取通过检测的交互组件
	UFUNCTION()
	TArray<UIS_BeInteractComponent*> TraceOutHitVerify(FGameplayTag TraceType, const TArray<FHitResult>& OutHit, UIS_BeInteractComponent*& TopPriorityCom, FText& FailText);

	/*通过检测类型Tag获取当前交互检测组件
	* TraceTypeTag：检测类型，该值为None时获取当前全部的可被交互组件
	* TopPriorityCom：最高交互优先级的交互组件
	* return：全部命中的可被交互组件
	*/
	UFUNCTION(BlueprintPure)
	TArray<UIS_BeInteractComponent*> GetCurInteractComponentFromTypeTag(UPARAM(Meta = (Categories = "InteractType")) FGameplayTag TraceTypeTag, UIS_BeInteractComponent*& TopPriorityCom);

	//触发交互
	UFUNCTION(BlueprintCallable)
	bool TriggerInteract(UPARAM(Ref)UIS_BeInteractComponent*& BeInteractCom, FCC_CompareInfo CompareInfo, UPARAM(Meta = (Categories = "InteractType")) FGameplayTag TraceTypeTag, FText& FailText);

	//开始进行交互检测
	UFUNCTION(BlueprintCallable)
	void InteractCheckStateChange(bool IsActive);

	//交互检测——检测周围/准心是否有可交互的资源
	UFUNCTION(BlueprintCallable)
	void InteractEnterCheck();

	//根据交互类型配置信息获取位置
	UFUNCTION()
	FVector GetLocationFromTraceInfo(const FIS_TraceInfo& TraceInfo, bool IsStartLocation);

	/*根据检测ID动态获取位置
	* 当FIS_TraceInfo信息中的EIS_TraceLocationType = DynamicGet时会调用该函数获取位置信息
	* 需要子类根据需求去返回不同ID下的位置信息
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintPure)
	FVector GetLocationFromTraceType(FGameplayTag TraceType, bool IsStartLocation);
	virtual FVector GetLocationFromTraceType_Implementation(FGameplayTag TraceType, bool IsStartLocation);

	//通过检测信息获取原生命中内容
	UFUNCTION(BlueprintCallable)
	bool TraceFromTypeInfo(FIS_TraceInfo TraceInfo, TArray<FHitResult>& OutHit);

	//通过追踪类型信息进行追踪检测
	UFUNCTION(BlueprintCallable)
	TArray<UIS_BeInteractComponent*> TraceCheckFromTypeInfo(FIS_InteractTypeInfo InteractTraceTypeInfo, UIS_BeInteractComponent*& TopPriorityCom);

	//通过追踪类型Tag进行追踪检测
	UFUNCTION(BlueprintCallable)
	TArray<UIS_BeInteractComponent*> TraceCheckFromTypeTag(UPARAM(Meta = (Categories = "InteractType")) FGameplayTag TraceTypeTag, UIS_BeInteractComponent*& TopPriorityCom);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	TArray<UIS_BeInteractComponent*> InteractEnterCheckFromTraceType(UPARAM(Meta = (Categories = "InteractType")) FGameplayTag TraceTypeTag, UIS_BeInteractComponent*& TopPriorityCom);
	virtual TArray<UIS_BeInteractComponent*> InteractEnterCheckFromTraceType_Implementation(FGameplayTag TraceTypeTag, UIS_BeInteractComponent*& TopPriorityCom);

	//更新交互目标
	UFUNCTION(NetMulticast, Reliable)
	void UpdateInteractTarget(UIS_BeInteractComponent* BeInteractComponent, UPARAM(Meta = (Categories = "InteractType")) FGameplayTag TraceTypeTag);

	/*验证当前正在交互目标的完成交互的Server版本
	* 该函数主要是给在客户端存在的验证对象/或其他类似需求的资源使用，例如QTEUI
	*/
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ServerVerifyCurInteractComplete(FGameplayTag TraceTypeTag);

	/*验证当前正在交互目标的结束交互的Server版本
	* 该函数主要是给在客户端存在的验证对象/或其他类似需求的资源使用，例如QTEUI
	*/
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ServerVerifyCurInteractEnd(FGameplayTag TraceTypeTag);

	/*离开当前交互检测的目标
	* 离开需要触发交互结束，因此离开需要在服务器上被调用
	*/
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ServerLeaveInteractCheck(UIS_BeInteractComponent* BeInteractComponent, UPARAM(Meta = (Categories = "InteractType")) FGameplayTag InteractTraceTypeTag);

	/*添加忽略Tag
	*/
	UFUNCTION(BlueprintCallable)
	void AddInteractIgnoreTag(FGameplayTagContainer TagContainer);

	/*移除忽略Tag
	*/
	UFUNCTION(BlueprintCallable)
	void RemoveInteractIgnoreTag(FGameplayTagContainer TagContainer);

	//设置预交互目标组件
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ServerSetPreBeInteractComponent(UIS_BeInteractComponent* BeInteractCom);

	//设置
	UFUNCTION(BlueprintCallable)
	void SetInteractTypeTagMapping(TMap<FGameplayTag, float> Mapping);

	UFUNCTION(BlueprintCallable)
	void AppendInteractTypeTagMapping(TMap<FGameplayTag, float> Mapping);

	UFUNCTION(BlueprintCallable)
	void RemoveInteractTypeTag(UPARAM(Meta = (Categories = "InteractType")) FGameplayTag TypeTag);

public:
	//该Actor的角色签名（角色唯一标识-通常是ID/或者玩家ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName RoleSign = "None";

	//交互失败音效
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<USoundBase> InteractFailSound;

	/*交互忽略tag
	* 会用该值和要交互的目标FIS_BeInteractInfo的InteractTag做比较，如果对方存在则忽略
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	FGameplayTagContainer InteractIgnoreTag;

	//-------------------交互检测信息-------------------
	//在BeginPlay时是否要激活交互检测
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace|InteractCheck")
	bool bBeginPlayIsActiveInteractCheck = false;

	/*要进行交互检测的全部类型<检测类型Tag,检测触发间隔>
	* 数组越前的类型越早检测
	* 检测触发间隔：该值最好是InteractCheckInterval的倍数，否则并不会很精确
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Categories = "InteractType"), Category = "Trace|InteractCheck")
	TMap<FGameplayTag, float> AllInteractTypeTagMapping = { {TAG_InteractType_CameraTrace, 0.1f} };

	//该Map用于计时，判断是否可以触发一次追踪检测
	UPROPERTY(BlueprintReadWrite)
	TMap<FGameplayTag, float> AllInteractTypeTagTime;
	
	UPROPERTY()
	FTimerHandle InteractCheckTimeHandle;
	//交互检测射线的间隔
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace|InteractCheck")
	float InteractCheckInterval = 0.1f;
	//当前检测到的全部组件<检测类型Tag,被交互组件数组>
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TMap<FGameplayTag, FIS_BeInteractComponentArray> AllTraceCheckComponent;

	UPROPERTY(BlueprintAssignable)
	FIS_InteractEvent UpdateInteractEvent;

	/*当前的预交互目标
	* 如果该值有效，交互时优先交互该目标，否则才是通过检测获取一个交互目标
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated)
	UIS_BeInteractComponent* PreBeInteractComponent;
	//当前正在交互中的目标
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated)
	UIS_BeInteractComponent* CurStartInteractComponent;

	//默认对比信息
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCC_CompareInfo DefaultCompareInfo;
};
