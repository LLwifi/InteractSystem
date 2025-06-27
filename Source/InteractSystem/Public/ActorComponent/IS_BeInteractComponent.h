// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include <Engine/DataTable.h>
#include <GameplayTagContainer.h>
#include "Common/IS_Interface.h"
#include "IS_BeInteractComponent.generated.h"

/*被交互的信息
*/
USTRUCT(BlueprintType)
struct FIS_BeInteractInfo : public FTableRowBase
{
	GENERATED_BODY()
public:
	//交互UI显示文本
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText InteractText;
	//交互失败UI显示文本，在为通过交互判断后显示该文本
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText InteractFailText;

	//交互方式类型
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EIS_InteractType InteractType = EIS_InteractType::Instant;
	//多段交互次数 如果交互方式类型为多段持续交互将开启该参数的配置，该值同时影响交互时长（InteractTime）的最大数组下标
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "InteractType == EIS_InteractType::MultiSegment"))
	int32 MultiInteractNum = 2;
	//交互时长 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "InteractType == EIS_InteractType::HasDuration || InteractType == EIS_InteractType::MultiSegment"))
	TArray<float> InteractTime = {1.0f};

	//交互次数 该值为0时不允许交互
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 InteractNum = 1;
	//交互次数扣除类型
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EIS_InteractNumSubtractType InteractNumSubtractType = EIS_InteractNumSubtractType::NotSubtract;
	//交互tag 用来表示类型或额外的其他信息
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer InteractTag;
	//交互优先级 该值越大越优先交互
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 InteractPriority = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractDelegate, UIS_InteractComponent*, InteractComponent);

/*被交互组件：该组件描述一个可以被交互的资源的基本信息
*/
UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INTERACTSYSTEM_API UIS_BeInteractComponent_Box : public UBoxComponent, public IIS_Interface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UIS_BeInteractComponent_Box();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//-----------------------------------------------------IIS_Interface
	virtual FText GetInteractText();
	virtual FText GetInteractFailText();
	virtual EIS_InteractType GetInteractType();
	virtual int32 GetMultiInteractNum();
	virtual TArray<float> GetInteractTime();
	virtual int32 GetInteractNum();
	virtual EIS_InteractNumSubtractType GetInteractNumSubtractType();
	virtual FGameplayTagContainer GetInteractTag();
	virtual int32 GetInteractPriority();

	virtual void InteractEnter(UIS_InteractComponent* EnterInteractCom);
	//移入可交互目标
	UPROPERTY(BlueprintAssignable)
	FInteractDelegate OnInteractEnter;
	virtual void InteractLeave(UIS_InteractComponent* EnterInteractCom);
	//移出可交互目标
	UPROPERTY(BlueprintAssignable)
	FInteractDelegate OnInteractLeave;
	virtual bool TryInteract(UIS_InteractComponent* EnterInteractCom);
	virtual bool InteractCheck(UIS_InteractComponent* EnterInteractCom);
	virtual void InteractStart(UIS_InteractComponent* EnterInteractCom);
	//开始交互
	UPROPERTY(BlueprintAssignable)
	FInteractDelegate OnInteractStart;
	virtual void InteractEnd(UIS_InteractComponent* EnterInteractCom);
	//结束交互
	UPROPERTY(BlueprintAssignable)
	FInteractDelegate OnInteractEnd;
	//-----------------------------------------------------IIS_Interface
		
public:
	//被交互的信息
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIS_BeInteractInfo BeInteractInfo;


};
