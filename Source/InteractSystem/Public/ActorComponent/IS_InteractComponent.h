// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <GameplayTagContainer.h>
#include "Net/UnrealNetwork.h"
#include "../../../../../CommonCompare/Source/CommonCompare/Public/CC_StructAndEnum.h"
#include "Kismet/KismetSystemLibrary.h"//EDrawDebugTrace需要
#include "IS_InteractComponent.generated.h"

////任务角色
//UENUM(BlueprintType)
//enum class ETS_TaskRole :uint8
//{
//	None = 0 UMETA(DisplayName = "无"),
//	Player UMETA(DisplayName = "Player-做任务的单位"),
//	NPC UMETA(DisplayName = "NPC-发任务的单位"),
//	People UMETA(DisplayName = "People-与任务相关的单位")
//};

class UIS_BeInteractComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FIS_InteractEvent, UIS_InteractComponent*, InteractComponent, UIS_BeInteractComponent*, BeInteractComponent);


/*交互信息
* 是否仅交互一个/可以交互的范围、同时交互的数量
* 暂定
*/
USTRUCT(BlueprintType)
struct FCC_InteractInfo
{
	GENERATED_BODY()
public:
	//是否仅交互一个资源
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bInteractOnce = true;
};

/*交互射线信息
*/
USTRUCT(BlueprintType)
struct FCC_InteractRayInfo
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

/*交互组件：该组件拥有交互其他资源的能力
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
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

	/*相机射线获取可被交互组件
	* return：全部命中的可被交互组件
	* TopPriorityCom：最高交互优先级的交互组件
	*/
	UFUNCTION(BlueprintCallable)
	TArray<UIS_BeInteractComponent*> CameraTraceGetBeInteractCom(FCC_InteractRayInfo InteractRayInfo, UIS_BeInteractComponent*& TopPriorityCom);

	//尝试从摄像机发射射线触发交互
	UFUNCTION(BlueprintCallable)
	bool TryTriggerInteract_CameraTrace(FCC_CompareInfo CompareInfo, FCC_InteractRayInfo InteractRayInfo, UIS_BeInteractComponent*& TopPriorityCom, TArray<UIS_BeInteractComponent*>& AllBeInteract, FText& FailText);
	
	//开始进行交互检测
	UFUNCTION(BlueprintCallable)
	void InteractCheckStateChange(bool IsActive);

	//交互检测——检测周围/准心是否有可交互的资源
	UFUNCTION(BlueprintCallable)
	void InteractCheck();

	//广播交互事件
	UFUNCTION(NetMulticast, Reliable)
	void UpdateInteractTarget(UIS_BeInteractComponent* BeInteractComponent);

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

public:
	//该Actor的角色签名（角色唯一标识-通常是ID/或者玩家ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName RoleSign = "None";

	//射线的距离
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	float TraceDistance = 5000.0f;
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
	float DrawTime;

	//-------------------交互检测信息-------------------
	//在BeginPlay时是否要激活交互检测
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace|InteractCheck")
	bool bBeginPlayIsActiveInteractCheck = false;
	UPROPERTY()
	FTimerHandle InteractCheckTimeHandle;
	//交互检测射线的间隔
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace|InteractCheck")
	float InteractCheckInterval = 0.1f;
	//交互检测射线的信息
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace|InteractCheck")
	FCC_InteractRayInfo InteractCheckRayInfo;
	//当前交互检测的目标
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UIS_BeInteractComponent* InteractCheckComponent;

	UPROPERTY(BlueprintAssignable)
	FIS_InteractEvent UpdateInteractEvent;
	//UPROPERTY(BlueprintAssignable)
	//FTaskComponentDelegate TaskEndEvent;

	//当前正在交互中的目标 通常是交互优先级最大的目标
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated)
	UIS_BeInteractComponent* CurStartInteractComponent;


	//默认对比信息
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCC_CompareInfo DefaultCompareInfo;
};
