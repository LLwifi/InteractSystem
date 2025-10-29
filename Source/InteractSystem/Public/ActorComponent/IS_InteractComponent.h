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

	////只获取激活的可被交互组件
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//bool bOnlyGetActiveComponent = false;
};

/*交互检测类型数组
*/
USTRUCT(BlueprintType)
struct FIS_InteractTraceTypeArray
{
	GENERATED_BODY()
public:
	EIS_InteractTraceType operator[](int32 Index) { return AllInteractTraceType[Index]; };
	int32 Num() { return AllInteractTraceType.Num(); };
	void Add(EIS_InteractTraceType TraceType) { AllInteractTraceType.Add(TraceType); };
	void Remove(EIS_InteractTraceType TraceType){ AllInteractTraceType.Remove(TraceType); }
	bool IsValidIndex(int32 Index) { return AllInteractTraceType.IsValidIndex(Index); };

public:
	//是否仅交互一个资源
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<EIS_InteractTraceType> AllInteractTraceType;
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

	//处理射线命中的全部交互组件
	UFUNCTION()
	TArray<UIS_BeInteractComponent*> TraceOutHitCheck(EIS_InteractTraceType InteractTraceType, FIS_InteractRayInfo InteractRayInfo, const TArray<FHitResult>& OutHit, UIS_BeInteractComponent*& TopPriorityCom);

	/*通过检测类型获取当前交互检测组件
	* InteractTraceType：检测类型，该值为None时对比当前全部的可被交互组件
	* TopPriorityCom：最高交互优先级的交互组件
	* return：全部命中的可被交互组件
	*/
	UFUNCTION(BlueprintPure)
	TArray<UIS_BeInteractComponent*> GetInteractCheckComponentFromType(EIS_InteractTraceType InteractTraceType, UIS_BeInteractComponent*& TopPriorityCom);

	/*相机射线获取可被交互组件
	* return：全部命中的可被交互组件
	* TopPriorityCom：最高交互优先级的交互组件
	*/
	UFUNCTION(BlueprintCallable)
	TArray<UIS_BeInteractComponent*> CameraTraceGetBeInteractCom(FIS_InteractRayInfo InteractRayInfo, UIS_BeInteractComponent*& TopPriorityCom);

	//尝试从摄像机发射射线触发交互
	UFUNCTION(BlueprintCallable)
	bool TryTriggerInteract_CameraTrace(FCC_CompareInfo CompareInfo, FIS_InteractRayInfo InteractRayInfo, UIS_BeInteractComponent*& TopPriorityCom, TArray<UIS_BeInteractComponent*>& AllBeInteract, FText& FailText);
	
	/*圆形范围获取可被交互组件
	* TopPriorityCom：最高交互优先级的交互组件
	* return：全部命中的可被交互组件
	*/
	UFUNCTION(BlueprintCallable)
	TArray<UIS_BeInteractComponent*> SphereTraceGetBeInteractCom(FVector Start, FVector End, float Radius, FIS_InteractRayInfo InteractRayInfo, UIS_BeInteractComponent*& TopPriorityCom);

	//尝试使用圆形检测触发交互
	UFUNCTION(BlueprintCallable)
	bool TryTriggerInteract_SphereTrace(FCC_CompareInfo CompareInfo, FIS_InteractRayInfo InteractRayInfo, FVector Start, FVector End, float Radius, UIS_BeInteractComponent*& TopPriorityCom, TArray<UIS_BeInteractComponent*>& AllBeInteract, FText& FailText);

	//开始进行交互检测
	UFUNCTION(BlueprintCallable)
	void InteractCheckStateChange(bool IsActive);

	//交互检测——检测周围/准心是否有可交互的资源
	UFUNCTION(BlueprintCallable)
	void InteractCheck();

	/*根据进入类型进行交互检测
	* Return：检测到的全部可交互组件
	*/
	UFUNCTION(BlueprintCallable)
	TArray<UIS_BeInteractComponent*> InteractCheckFromEnterType(EIS_InteractTraceType InteractTraceType);

	//广播交互事件
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

public:
	//该Actor的角色签名（角色唯一标识-通常是ID/或者玩家ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName RoleSign = "None";

	//交互失败音效
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<USoundBase> InteractFailSound;

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
	float DrawTime;

	//-------------------交互检测信息-------------------
	//在BeginPlay时是否要激活交互检测
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace|InteractCheck")
	bool bBeginPlayIsActiveInteractCheck = false;
	/*要进行交互检测的全部类型，同时也是检测顺序
	* 数组越前的类型越早检测
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace|InteractCheck")
	TArray<EIS_InteractTraceType> InteractCheckTypes = { EIS_InteractTraceType::CameraTrace };
	
	UPROPERTY()
	FTimerHandle InteractCheckTimeHandle;
	//交互检测射线的间隔
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace|InteractCheck")
	float InteractCheckInterval = 0.1f;
	//交互检测射线的信息
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace|InteractCheck")
	FIS_InteractRayInfo InteractCheckRayInfo;
	//当前交互检测的目标
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite/*, Replicated*/)
	TArray<UIS_BeInteractComponent*> InteractCheckComponents;

	UPROPERTY(BlueprintAssignable)
	FIS_InteractEvent UpdateInteractEvent;

	//当前正在交互中的目标 通常是交互优先级最大的目标
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated)
	UIS_BeInteractComponent* CurStartInteractComponent;

	//默认对比信息
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCC_CompareInfo DefaultCompareInfo;
};
