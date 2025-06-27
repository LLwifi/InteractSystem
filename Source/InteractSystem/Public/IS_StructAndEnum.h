#pragma once

#include "CoreMinimal.h"
#include <Engine/DataTable.h>
#include <GameplayTagContainer.h>
#include "IS_StructAndEnum.generated.h"

//class UTS_TaskCompare;
//
///*任务的对比对照结构体
//* 该结构体将常见的比较情况进行了汇总
//*/
//USTRUCT(BlueprintType)
//struct FTaskCompareInfo : public FTableRowBase
//{
//	GENERATED_BODY()
//public:
//	//比对方式是否使用任务比对类
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//	bool IsUseTaskCompare = false;
//
//	//任务对照类
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "IsUseTaskCompare"))
//	TSoftClassPtr<UTS_TaskCompare> TaskCompareClass;
//
//	/*TaskCompareTag_Info，与外部进行比对时的决策
//	* 该值为Flase时，外部满足任意一个Tag即可
//	*/
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "!IsUseTaskCompare"))
//	bool TaskCompareTagIsAllMatch = true;
//
//	/*TaskCompareTag_Info，与外部进行比对时的精准决策
//	* 该值为Flase时，外部Tag包含父类即可
//	*/
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "!IsUseTaskCompare"))
//	bool TaskCompareTagIsExactMatch = true;
//
//	/*任务对照Tag
//	* 可以作为类型使用，例如击杀/收集任务（任务目标类型对照） 等级/职业判断（任务条件类型对照）
//	*/
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Categories = "Task"), meta = (EditConditionHides, EditCondition = "!IsUseTaskCompare"))
//	FGameplayTagContainer TaskCompareTag_Info;
//
//	/*任务对照Class 会判断是否等于该类或该类的子类
//	* 该值用来判断目标的类型，例如判断击杀/交互的单位
//	*/
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "!IsUseTaskCompare"))
//	TSoftClassPtr<UObject> TaskCompareClass_Info;
//
//	/*任务对照自定义信息 自定义字符比对，任务目标检测时额外的比对项目
//	* 某些不至于使用UObject但是Class不足以判断内容时，可以选择使用该内容进行判断
//	* 例如：击杀的目标是否携带某种状态也可以通过定于该值进行判断：例如用Fire代表燃烧；Ice代表冰冻（任务目标类型对照）
//	*/
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "!IsUseTaskCompare"))
//	FString TaskCompareString_Info;
//
//	/*任务对照Obejct信息
//	* 该值用来判断一些更加细致的具体事项（例如复数信息），例如击杀的目标是否携带某种状态，交互的单位身价是否超过某个数值
//	*/
//	UPROPERTY(BlueprintReadWrite)
//	UObject* TaskCompareObject_Info;
//};