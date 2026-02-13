// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorComponent/BeInteractExtend/IS_BeInteractExtendBase.h"
#include "GameFramework/Actor.h"
#include "IS_BIEGetActor.generated.h"

/**被交换组件扩展内容————获取Actor
 * 
 */
UCLASS()
class INTERACTSYSTEM_API UIS_BIEGetActor : public UIS_BeInteractExtendBase
{
	GENERATED_BODY()
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	virtual void Init_Implementation(UIS_BeInteractComponent* BeInteractCom, UIS_BeInteractExtendBase* Data) override;

	//当前的组件数量是否足够
	UFUNCTION(BlueprintPure)
	bool NumIsSufficient(int32 CurNum);

	//获取Actor
	UFUNCTION(BlueprintPure)
	TArray<AActor*> GetActor();

	//通过类型获取Actor
	UFUNCTION(BlueprintPure)
	TArray<AActor*> GetActorFromClass();

	//通过接口类型获取组件
	UFUNCTION(BlueprintPure)
	TArray<AActor*> GetActorFromInterface();

	//判断额外条件
	UFUNCTION(BlueprintCallable)
	void ActorConditionCheck(TArray<AActor*> Actors);

	//添加忽略的对象
	UFUNCTION(BlueprintCallable)
	void AddIgnore(AActor* Actor);
public:
	/*需要获取的Actor数量 <=0 表示获取全部
	* 需要注意的是：获取的顺序是先根据GetClass获取，再根据GetInterface获取
	* 如果GetNum = 1 而GetClass和GetInterface同时配置了，会先在GetClass里面拿去符合条件的组件
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Meta = (ExposeOnSpawn = True))
	int32 GetNum = 0;

	//获取给定的类型
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Meta = (ExposeOnSpawn = True))
	TArray<TSubclassOf<AActor>> GetClass;
	//获取给定的接口
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Meta = (ExposeOnSpawn = True))
	TArray<TSubclassOf<UInterface>> GetInterface;

	/*获取给定Tag的Actor
	* 额外判定项，在使用Interface或ComponentClass获取到Actor后再判断是否拥有该Tag
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Meta = (ExposeOnSpawn = True))
	TArray<FName> GetTag;

	UPROPERTY(BlueprintReadWrite, Replicated)
	TArray<AActor*> All;

	//要忽略的对象
	UPROPERTY(BlueprintReadWrite, Replicated, Meta = (ExposeOnSpawn = True))
	TArray<AActor*> Ignore;
};
