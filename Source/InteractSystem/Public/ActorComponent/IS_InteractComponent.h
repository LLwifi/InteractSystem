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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FIS_InteractEvent, UIS_InteractComponent*, InteractComponent, UIS_BeInteractComponent*, BeInteractComponent, EIS_InteractTraceType, TraceType);

/*交互射线信息
*/
USTRUCT(BlueprintType)
struct FIS_InteractRayInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bTraceComplex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AActor*> ActorsToIgnore;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIgnoreSelf;
};

/*交互检测组件信息
*/
USTRUCT(BlueprintType)
struct FIS_TraceCheckComponentInfo
{
	GENERATED_BODY()
public:
	FIS_TraceCheckComponentInfo(){}
	FIS_TraceCheckComponentInfo(UIS_BeInteractComponent* BeInteractCom, EIS_InteractTraceType TraceType)
	{ 
		BeInteractComponent = BeInteractCom;
		EnterTraceType = TraceType;
	}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UIS_BeInteractComponent* BeInteractComponent;
	//当前进入的检测类型
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EIS_InteractTraceType EnterTraceType;
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
	bool StartInteract(UIS_BeInteractComponent* BeInteractComponent);

	/*结束交互
	* 对当前正在交互的组件进行结束交互（通过函数StartInteract触发交互的组件为当前正在交互的组件）
	*/
	UFUNCTION(BlueprintCallable)
	void EndCurInteract();

	//交互条件检测
	UFUNCTION(BlueprintPure)
	bool InteractConditionCheck(UIS_BeInteractComponent* BeInteractComponent, EIS_InteractTraceType InteractTraceType, FText& FailText);

	//主动交互验证
	UFUNCTION(BlueprintPure)
	bool InteractVerify(UIS_BeInteractComponent* BeInteractComponent, FGameplayTag TraceType, FText& FailText);

	//处理射线命中的内容，提取通过检测的交互组件
	UFUNCTION()
	TArray<UIS_BeInteractComponent*> TraceOutHitCheck(EIS_InteractTraceType InteractTraceType, const TArray<FHitResult>& OutHit, UIS_BeInteractComponent*& TopPriorityCom, FText& FailText);

	//处理射线命中的内容，提取通过检测的交互组件
	UFUNCTION()
	TArray<UIS_BeInteractComponent*> TraceOutHitVerify(FGameplayTag TraceType, const TArray<FHitResult>& OutHit, UIS_BeInteractComponent*& TopPriorityCom, FText& FailText);

	/*通过检测类型获取当前交互检测组件
	* InteractTraceType：检测类型，该值为None时对比当前全部的可被交互组件
	* TopPriorityCom：最高交互优先级的交互组件
	* return：全部命中的可被交互组件
	*/
	UFUNCTION(BlueprintPure)
	TArray<UIS_BeInteractComponent*> GetInteractCheckComponentFromType(EIS_InteractTraceType InteractTraceType, UIS_BeInteractComponent*& TopPriorityCom);

	//触发交互
	UFUNCTION(BlueprintCallable)
	bool TriggerInteract(UPARAM(Ref)UIS_BeInteractComponent*& BeInteractCom, FCC_CompareInfo CompareInfo, EIS_InteractTraceType TraceType, FText& FailText);

	/*相机射线获取可被交互组件
	* return：全部命中的可被交互组件
	* TopPriorityCom：最高交互优先级的交互组件
	*/
	UFUNCTION(BlueprintCallable)
	TArray<UIS_BeInteractComponent*> CameraTraceGetBeInteractCom(FIS_InteractRayInfo InteractRayInfo, UIS_BeInteractComponent*& TopPriorityCom, FText& FailText);

	//尝试从摄像机发射射线触发交互
	UFUNCTION(BlueprintCallable)
	bool TryTriggerInteract_CameraTrace(FCC_CompareInfo CompareInfo, FIS_InteractRayInfo InteractRayInfo, UIS_BeInteractComponent*& TopPriorityCom, TArray<UIS_BeInteractComponent*>& AllBeInteract, FText& FailText);
	
	/*圆形范围获取可被交互组件
	* TopPriorityCom：最高交互优先级的交互组件
	* return：全部命中的可被交互组件
	*/
	UFUNCTION(BlueprintCallable)
	TArray<UIS_BeInteractComponent*> SphereTraceGetBeInteractCom(FVector Start, FVector End, float Radius, FIS_InteractRayInfo InteractRayInfo, UIS_BeInteractComponent*& TopPriorityCom, FText& FailText);

	//尝试使用圆形检测触发交互
	UFUNCTION(BlueprintCallable)
	bool TryTriggerInteract_SphereTrace(FCC_CompareInfo CompareInfo, FIS_InteractRayInfo InteractRayInfo, FVector Start, FVector End, float Radius, UIS_BeInteractComponent*& TopPriorityCom, TArray<UIS_BeInteractComponent*>& AllBeInteract, FText& FailText);

	//开始进行交互检测
	UFUNCTION(BlueprintCallable)
	void InteractCheckStateChange(bool IsActive);

	//交互检测——检测周围/准心是否有可交互的资源
	UFUNCTION(BlueprintCallable)
	void InteractEnterCheck();

	/*根据进入类型进行交互检测
	* Return：检测到的全部可交互组件
	*/
	UFUNCTION(BlueprintCallable)
	TArray<UIS_BeInteractComponent*> InteractCheckFromEnterType(EIS_InteractTraceType InteractTraceType);

	UFUNCTION()
	FVector GetLocationFromTraceInfo(const FIS_InteractTypeInfo& TraceInfo, bool IsStartLocation);

	/*根据检测ID动态获取位置
	* 需要子类根据需求去返回不同ID下的位置信息
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintPure)
	FVector GetLocationFromTraceType(FGameplayTag TraceType, bool IsStartLocation);
	virtual FVector GetLocationFromTraceType_Implementation(FGameplayTag TraceType, bool IsStartLocation);

	UFUNCTION(BlueprintCallable)
	TArray<UIS_BeInteractComponent*> InteractEnterCheckFromTraceInfo(FIS_InteractTypeInfo InteractTraceInfo);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	TArray<UIS_BeInteractComponent*> InteractEnterCheckFromTraceType(UPARAM(Meta = (Categories = "InteractType")) FGameplayTag TraceType);
	virtual TArray<UIS_BeInteractComponent*> InteractEnterCheckFromTraceType_Implementation(FGameplayTag TraceType);

	//更新交互目标
	UFUNCTION(NetMulticast, Reliable)
	void UpdateInteractTarget(UIS_BeInteractComponent* BeInteractComponent, EIS_InteractTraceType TraceType);

	/*验证当前正在交互目标的完成交互的Server版本
	* 该函数主要是给在客户端存在的验证对象/或其他类似需求的资源使用，例如QTEUI
	*/
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ServerVerifyCurInteractComplete();

	/*验证当前正在交互目标的结束交互的Server版本
	* 该函数主要是给在客户端存在的验证对象/或其他类似需求的资源使用，例如QTEUI
	*/
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ServerVerifyCurInteractEnd();

	/*离开当前交互检测的目标
	* 离开需要触发交互结束，因此离开需要在服务器上被调用
	*/
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ServerLeaveInteractCheck(UIS_BeInteractComponent* BeInteractComponent, EIS_InteractTraceType InteractTraceType);

	/*添加忽略Tag
	*/
	UFUNCTION(BlueprintCallable)
	void AddInteractIgnoreTag(FGameplayTagContainer TagContainer);

	/*移除忽略Tag
	*/
	UFUNCTION(BlueprintCallable)
	void RemoveInteractIgnoreTag(FGameplayTagContainer TagContainer);

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

	//射线的距离
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	float TraceDistance = 1000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	float SphereDistance = 1000.0f;
	//射线的响应类型
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	TEnumAsByte<ETraceTypeQuery> TraceChannel;

	//-------------------射线调试信息-------------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace|DeBug")
	TEnumAsByte<EDrawDebugTrace::Type> DrawDebugType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace|DeBug")
	FLinearColor TraceColor = FLinearColor::Red;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace|DeBug")
	FLinearColor TraceHitColor = FLinearColor::Green;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace|DeBug")
	float DrawTime = 1.0f;

	//-------------------交互检测信息-------------------
	//在BeginPlay时是否要激活交互检测
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace|InteractCheck")
	bool bBeginPlayIsActiveInteractCheck = false;
	/*要进行交互检测的全部类型，同时也是检测顺序
	* 数组越前的类型越早检测
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace|InteractCheck")
	TArray<EIS_InteractTraceType> InteractCheckTypes = { EIS_InteractTraceType::CameraTrace };

	/*要进行交互检测的全部类型，同时也是检测顺序
	* 数组越前的类型越早检测
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace|InteractCheck")
	TArray<FIS_InteractTypeInfo> AllInteractTypeInfo;
	
	UPROPERTY()
	FTimerHandle InteractCheckTimeHandle;
	//交互检测射线的间隔
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace|InteractCheck")
	float InteractCheckInterval = 0.1f;
	//交互检测射线的信息
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace|InteractCheck")
	FIS_InteractRayInfo InteractCheckRayInfo;
	//当前检测到的全部组件
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<FIS_TraceCheckComponentInfo> AllTraceCheckComponent;

	UPROPERTY(BlueprintAssignable)
	FIS_InteractEvent UpdateInteractEvent;

	//当前正在交互中的目标 通常是交互优先级最大的目标
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated)
	UIS_BeInteractComponent* CurStartInteractComponent;

	//默认对比信息
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCC_CompareInfo DefaultCompareInfo;
};
