// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorComponent/BeInteractExtend/IS_BeInteractExtendBase.h"
#include <Common/IS_BeInteractInterface.h>
#include "IS_BIEInteractCD.generated.h"


/*被交互的信息_交互冷却相关
* 通常是可配置的初始化信息
*/
USTRUCT(BlueprintType)
struct FIS_BeInteractInfo_CD
{
	GENERATED_BODY()
public:
	//交互CD
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CDTime = 0.0f;

	//每经过多少次交互进入一次CD
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 EnterCDInteractNum = 1;

	//如何算作一次交互
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EIS_InteractNumSubtractType InteractNumSubtractType = EIS_InteractNumSubtractType::Complete;

	/*不同交互者的交互CD是否分开记录
	* 该值为true时开启功能：此时CDTime表示被交互物体自身的CD
	* EveryoneCDTime表示每个人单独的CD
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bInteractNumIsMultiplepeople = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "bInteractNumIsMultiplepeople"))
	float EveryoneCDTime = 0.0f;

	//每经过多少次交互进入一次CD
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "bInteractNumIsMultiplepeople"))
	int32 EveryoneEnterCDInteractNum = 1;

	//如何算作一次交互
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "bInteractNumIsMultiplepeople"))
	EIS_InteractNumSubtractType EveryoneInteractNumSubtractType = EIS_InteractNumSubtractType::Complete;
};

/**可被交互物的功能扩展——冷却
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class INTERACTSYSTEM_API UIS_BIEInteractCD : public UIS_BeInteractExtendBase
{
	GENERATED_BODY()
	
public:
	//virtual void LinkInteractEnter_Implementation(UIS_InteractComponent* InteractComponent, EIS_InteractTraceType TraceType) override;
	//virtual void LinkInteractEnd_Implementation(UIS_InteractComponent* InteractComponent) override;
	//virtual void LinkInteractComplete_Implementation(UIS_InteractComponent* InteractComponent) override;

	//设置CD

	//进入CD
	UFUNCTION(BlueprintCallable)
	void EnterCD();

	//改变交互次数


	UFUNCTION()
	void CDTimerBack();
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIS_BeInteractInfo_CD Config;

	//交互的次数
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 InteractNum = 0;
	//是否在CD中
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bIsCD = false;
	UPROPERTY(BlueprintReadWrite)
	FTimerHandle CDTimerHandle;
};
