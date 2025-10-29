// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorComponent/BeInteractExtend/IS_BeInteractExtendBase.h"
#include "IS_BIEGetComponent.generated.h"

/**
 * 被交换组件扩展内容————获取组件
 * 该类提供了UE基础获取组件的方法
 */
UCLASS()
class INTERACTSYSTEM_API UIS_BIEGetComponent : public UIS_BeInteractExtendBase
{
	GENERATED_BODY()

public:
	virtual void Init_Implementation(UIS_BeInteractComponent* BeInteractCom, UIS_BeInteractExtendBase* Data) override;

	//当前的组件数量是否足够
	UFUNCTION(BlueprintPure)
	bool NumIsSufficient(int32 CurComponentNum);

	//获取组件
	UFUNCTION(BlueprintPure)
	TArray<UActorComponent*> GetComponent();

	//通过组件类型获取组件
	UFUNCTION(BlueprintPure)
	TArray<UActorComponent*> GetComponentFromComponentClass();

	//通过接口类型获取组件
	UFUNCTION(BlueprintPure)
	TArray<UActorComponent*> GetComponentFromInterface();
	
public:
	/*需要获取的组件数量 <=0 表示获取全部
	* 需要注意的是：获取的顺序是先根据ComponentClass获取，再根据Interface获取
	* 如果GetComponentNum = 1 而ComponentClass和Interface同时配置了，会先在ComponentClass里面拿去符合条件的组件
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 GetComponentNum = 0;

	//获取给定类型的组件
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<UActorComponent>> ComponentClass;
	//获取给定接口的组件
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<UInterface>> Interface;

	/*获取给定Tag的组件
	* 额外判定项，在使用Interface或ComponentClass获取到组件后再判断是否拥有该Tag
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> Tag;

	UPROPERTY(BlueprintReadWrite)
	TArray<UActorComponent*> AllComponents;
};
