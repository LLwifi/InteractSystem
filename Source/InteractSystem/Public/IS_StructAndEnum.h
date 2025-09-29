#pragma once

#include "CoreMinimal.h"
#include <Engine/DataTable.h>
#include "Kismet/KismetMathLibrary.h"
#include "Common/IS_Config.h"
#include "GameplayTagContainer.h"
//#include "../../../../../CommonCompare/Source/CommonCompare/Public/CC_StructAndEnum.h"
#include <CC_StructAndEnum.h>
#include "IS_StructAndEnum.generated.h"


class UIS_InteractComponent;
class UUserWidget;
class UIS_BeInteractExtendBase;

//被交互的类型
UENUM(BlueprintType)
enum class EIS_InteractType :uint8
{
	//拾取、开关某物等
	Instant = 0 UMETA(DisplayName = "瞬间交互"),
	//开宝箱、充电等。长时间交互
	HasDuration UMETA(DisplayName = "持续交互")
};

//交互次数扣除类型
UENUM(BlueprintType)
enum class EIS_InteractNumSubtractType :uint8
{
	NotSubtract = 0 UMETA(DisplayName = "不扣除"),
	Complete UMETA(DisplayName = "交互完成时"),
	//由交互完成触发的结束不会触发
	End UMETA(DisplayName = "仅交互结束时"),
	//可以理解为只要交互了就会触发（同时包含交互完成和交互结束）
	Start UMETA(DisplayName = "交互开始时")
};

//交互时间的累计类型
UENUM(BlueprintType)
enum class EIS_InteractCumulativeTimeType :uint8
{
	NotCumulative = 0 UMETA(DisplayName = "不累计"),
	//按照CumulativeTimeInterval的值不断累计
	Interval UMETA(DisplayName = "按间隔累计"),
	/*根据InteractTime数组的每个下标进行阶段累计
	* 需要注意的是该类型只有当InteractTime的长度 > 2 时生效
	*/
	InteractTimeIndex UMETA(DisplayName = "按分段累计")
};

//被交互资源能被触发“进入”事件的类型
UENUM(BlueprintType)
enum class EIS_BeInteractCheckType :uint8
{
	//当检测到多个可被交互的资源时，这些资源均可触发“进入”事件
	AnyTrigger UMETA(DisplayName = "可触发"),
	//当检测到多个可被交互的资源时，只有交互优先级最高的资源会触发“进入”事件
	TopPriorityTrigger UMETA(DisplayName = "最高优先级可触发")
};

//交互检测的类型
UENUM(BlueprintType)
enum class EIS_InteractTraceType :uint8
{
	None = 0 UMETA(DisplayName = "空"),
	CameraTrace UMETA(DisplayName = "摄像机射线检测"),
	SphereTrace UMETA(DisplayName = "球形检测")
};

//被交互的类型
UENUM(BlueprintType)
enum class EIS_InteractCheckType :uint8
{
	//射线检测的判定
	InteractCheck = 0 UMETA(DisplayName = "交互检测判定"),
	//开启交互的判定
	Interact UMETA(DisplayName = "交互时判定"),
	/*【交互完成时】的判定
	* 这里的【交互完成】指的是即将要调用InteractComplete前（经过了交互时间，马上要触发完成前的最终判定）
	*/
	InteractComplete UMETA(DisplayName = "交互完成时判定")
};

/*交互完成时的验证信息
*/
USTRUCT(BlueprintType)
struct FIS_InteractCompleteVerifyInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (PinHiddenByDefault, InlineEditConditionToggle))
	bool bOverride_CompleteInteractRoleNumVerify = false;
	/*完成时的交互人数验证 >=时通过
	* 注意：该配置会影响交互完成功能
	* 如果bRoleNumVerifyIsNowOrHistory = false时该值不应该大于SameTimeInteractRoleNum（同时交互人数），否则有可能永远无法完成交互
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bOverride_CompleteInteractRoleNumVerify"))
	int32 CompleteInteractRoleNumVerify = 1;
	/*人数验证是交互中的人数还是历史交互人数
	* 该值为true表示交互中的人数
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bOverride_CompleteInteractRoleNumVerify"))
	bool bRoleNumVerifyIsNowOrHistory = true;
	//验证交互人数是否满足的间隔
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bOverride_CompleteInteractRoleNumVerify"))
	float VerifyInteractRoleNumTimeInterval = 0.1;

	/*完成交互额外需要的验证类
	* 该验证类可以是UI或者其他——例如QTEUI、3D场景解密
	* 该UI在其他条件全部满足后的最后一步创建
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftClassPtr<UObject> CompleteVerifyClass;
};

/*交互验证信息
* 专属于交互验证的配置
*/
USTRUCT(BlueprintType)
struct FIS_InteractVerifyInfo
{
	GENERATED_BODY()

public:
	bool Verify(UActorComponent* InteractComponent, UActorComponent* BeInteractComponent, FText& FailText);
	bool DistanceVerify(UActorComponent* InteractComponent, UActorComponent* BeInteractComponent, FText& FailText);
	bool AngleVerify(UActorComponent* InteractComponent, UActorComponent* BeInteractComponent, FText& FailText);
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (PinHiddenByDefault, InlineEditConditionToggle))
	bool bOverride_InteractDistanceVerify = false;
	/*交互时的距离验证
	* 设置后交互者必须与该资源的距离处于该范围才允许交互
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bOverride_InteractDistanceVerify"))
	FFloatRange InteractDistanceVerify;
	//距离验证失败的文本提示
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bOverride_InteractDistanceVerify"))
	FText InteractDistance_FailText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (PinHiddenByDefault, InlineEditConditionToggle))
	bool bOverride_InteractAngleVerify = false;
	/*交互时的角度验证 某侧门不能开启/暗杀等交互需求可以对该值进行设置
	* 设置后交互者必须与该资源X轴的夹角处于该值范围内才允许交互
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bOverride_InteractAngleVerify"))
	FFloatRange InteractAngleVerify;
	//角度验证失败的文本提示
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bOverride_InteractAngleVerify"))
	FText InteractAngle_FailText;

};

/*历史被交互的信息
*
*/
USTRUCT(BlueprintType)
struct FIS_HistoryBeInteractInfo
{
	GENERATED_BODY()

public:
	FIS_HistoryBeInteractInfo() {}
	FIS_HistoryBeInteractInfo(FName RoleSign)
	{
		InteracterRoleSign = RoleSign;
		InteractCount = 1;
		InteractCompleteCount = 0;
		InteractTime = 0.0f;
	}
	FIS_HistoryBeInteractInfo(FName RoleSign, int32 InitInteractCount, float InitInteractTime, int32 InitInteractCompleteCount)
	{
		InteracterRoleSign = RoleSign;
		InteractCount = InitInteractCount;
		InteractTime = InitInteractTime;
		InteractCompleteCount = InitInteractCompleteCount;
	}
	int32 AddInteractCount(int32 AddNum) { InteractCount += AddNum; return InteractCount; }
	int32 AddInteractCompleteCount(int32 AddCompleteNum) { InteractCompleteCount += AddCompleteNum; return InteractCompleteCount; }
	float AddInteractTime(float AddTime) { InteractTime += AddTime; return InteractTime; }
public:
	//交互者的签名
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName InteracterRoleSign;

	//历史交互次数
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 InteractCount = 0;

	//历史交互完成次数 该值对应多段交互
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 InteractCompleteCount = 0;

	//历史交互时间
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InteractTime = 0.0f;
};

/*可被交互物的扩展Handle
* 通常是为了方便寻找DT的行名称
*/
USTRUCT(BlueprintType)
struct FIS_BeInteractExtendHandle
{
	GENERATED_BODY()
public:
	//RowName
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName RowName;
};

/*可被交互物的扩展配置
* 通常是可配置的初始化信息
*/
USTRUCT(BlueprintType, Blueprintable)
struct FIS_BeInteractExtend : public FTableRowBase
{
	GENERATED_BODY()
public:
	virtual void OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UIS_BeInteractExtendBase> BeInteractExtendClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditinLine))
	UIS_BeInteractExtendBase* BeInteractExtend = nullptr;
};

/*【可被交互物】能被交互检测到的验证条件
*/
USTRUCT(BlueprintType)
struct FIS_BeInteractCheckCondition_Verify
{
	GENERATED_BODY()
public:
	//通过验证（Verify）的才会被检测
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsVerifyTraceCheck = false;
};

/*可被交互物能被交互检测到的条件
*/
USTRUCT(BlueprintType)
struct FIS_BeInteractCheckCondition
{
	GENERATED_BODY()
public:
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EIS_BeInteractCheckType InteractCheckType = EIS_BeInteractCheckType::AnyTrigger;

	//激活时才能被检测
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsActiveTraceCheck = false;

	//是否要开启验证（Verify）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (PinHiddenByDefault, InlineEditConditionToggle))
	bool bIsVerifyTraceCheck = false;
	//通过【距离】验证（Verify）的才会被检测
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bIsVerifyTraceCheck"))
	bool bIsDistanceVerifyTraceCheck = true;
	//通过【角度】验证（Verify）的才会被检测
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bIsVerifyTraceCheck"))
	bool bIsAngleVerifyTraceCheck = true;

	//通过比对（Compare）的才会被检测
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsCompareTraceCheck = false;
};

/*可被交互物的Handle
* 通常是为了方便寻找DT的行名称
*/
USTRUCT(BlueprintType)
struct FIS_BeInteractInfoHandle
{
	GENERATED_BODY()
public:
	//RowName
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName RowName;
};

/*被交互的信息
* 通常是可配置的初始化信息
*/
USTRUCT(BlueprintType)
struct FIS_BeInteractInfo : public FTableRowBase
{
	GENERATED_BODY()
public:
	virtual void OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName) override
	{
		FTableRowBase::OnDataTableChanged(InDataTable, InRowName);
	}
public:
	//交互UI显示文本
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText InteractText;

	//交互提示UI类
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftClassPtr<UUserWidget> InteractTipPanelClass = UIS_Config::GetInstance()->DefaultInteractTipPanelClass;
	//交互方式类型
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EIS_InteractType InteractType = EIS_InteractType::Instant;
	/*生成挂载事件——在想要实现类似驾驶、操作某物。结束时间自由控制/不存在交互时间的交互时可以开启
	* 开启该函数后在交互完成时不会触发结束而是触发InteractAttachTo
	* 退出/分离挂载需要主动调用InteractAttachDetach
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bGenerateAttachEvents = false;
	/*交互时长
	* 如果数组长度 > 1 则该交互视为多段交互
	* 多段交互会在每个下标的交互时长满足时触发InteractComplete_MultiSegment
	* 多段交互时 若交互时长累计类型（InteractCumulativeTimeType）为 多段累计（InteractTimeIndex）每当交互到了下标时长时会记录节点
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "InteractType != EIS_InteractType::Instant && InteractType != EIS_InteractType::AttachTo"))
	TArray<float> InteractTime = { 1.0f };
	/*交互时长是否允许累计
	* 通常给长时间交互使用，交互时长从上次累计的时长处开始
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EIS_InteractCumulativeTimeType InteractCumulativeTimeType = EIS_InteractCumulativeTimeType::NotCumulative;

	/*交互蒙太奇
	* 数组可以配置多个动作应对不同的使用情况，例如：同时配置第一人称动作和第三人称的角色动作 / 男性和女性的交互动作
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, TSoftObjectPtr<UAnimMontage>> InteractMontage = UIS_Config::GetInstance()->DefaultBeInteractMontage;

	//交互音效
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, TSoftObjectPtr<USoundBase>> InteractSound = UIS_Config::GetInstance()->DefaultBeInteractSound;

	/*交互验证信息
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIS_InteractVerifyInfo InteractVerifyInfo;
	/*交互完成时的验证信息
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIS_InteractCompleteVerifyInfo InteractCompleteVerifyInfo;

	/*交互时是否需要隐藏角色持有物（武器/道具等）
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bInteractHideHoldItem = true;

	//同时交互人数
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SameTimeInteractRoleNum = 1;
	//交互人数超出时的失败文本提示
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText SameTimeInteractRoleNum_FailText;

	//交互时长是否允许多人累加 可以理解为大家共同推进进度/共用一个进度条
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "InteractType != EIS_InteractType::Instant && SameTimeInteractRoleNum > 1"))
	bool bEveryoneCumulativeTime = false;
	//初始交互次数 该值为0时不允许交互
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 InteractNum = 1;
	//交互次数不足时的失败文本提示
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText InteractNum_FailText;
	//交互次数扣除类型
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EIS_InteractNumSubtractType InteractNumSubtractType = EIS_InteractNumSubtractType::NotSubtract;
	/*交互次数是否影响交互激活状态
	* 该值为true时，交互次数 > 0设置为激活 交互次数 < 0 设置为失活
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bInteractActiveFromInteractNum = true;
	/*不同交互者的交互次数是否分开记录
	* 该值为true时开启功能：此时InteractNum表示被交互物体总共可以被交互的次数
	* EveryoneInteractlNum表示每个人单独可以交互的次数
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bInteractNumIsMultiplepeople = false;
	//每个人的交互次数
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "bInteractNumIsMultiplepeople"))
	int32 EveryoneInteractlNum = 1;
	//每个人的交互次数扣除方式 注意：该值不应该为NotSubtract【不扣除】，不扣除本质上不需要区分每个人的交互次数
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "bInteractNumIsMultiplepeople"))
	EIS_InteractNumSubtractType EveryoneInteractNumSubtractType = EIS_InteractNumSubtractType::End;

	//交互tag 用来表示类型或额外的其他信息
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer InteractTag;
	//交互优先级 该值越大越优先交互
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 InteractPriority = 0;
	//默认是否开启/激活交互
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDefaultInteractActive = true;
	//未开启交互时的的失败文本提示
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText InteractActive_FailText;
	/*未激活时是否显示交互文本
	* 该选项包括移入的交互文本以及未激活的失败文本
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bNotActiveIsDisplayInteractText = false;

	//在移出被交互物时，要不要停止交互
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bInteractLeaveIsEnd = true;
	//累计时间交互时的最小累计时长/间隔
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CumulativeTimeInterval = 0.01;

	//被比对的信息
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCC_BeCompareInfo BeCompareInfo;

	/*相机射线能被触发进入的类型
	* 不添加表示该类型不会触发
	* 该Map为空时进入/检测事件不会触发
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EIS_InteractTraceType, FIS_BeInteractCheckCondition> InteractCheckEnterCondition;

	////“我”能被交互被检测到的条件
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//FIS_BeInteractCheckCondition InteractCheckCondition;

	/*可被交互物的扩展功能
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FIS_BeInteractExtendHandle> BeInteractExtendHandle;
};

/*被交互的动态信息
* 关键信息，需要网络同步
*/
USTRUCT(BlueprintType)
struct FIS_BeInteractDynamicInfo : public FTableRowBase
{
	GENERATED_BODY()
public:
	//记录交互信息
	FIS_HistoryBeInteractInfo RecordInteractInfo(FName RoleSign, int32 AddInteractCount, float AddInteractTime, int32 AddInteractCompleteCount)
	{
		for (FIS_HistoryBeInteractInfo& HistoryInfo : HistoryBeInteracterInfo)
		{
			if (HistoryInfo.InteracterRoleSign == RoleSign)
			{
				HistoryInfo.AddInteractCount(AddInteractCount);
				HistoryInfo.AddInteractTime(AddInteractTime);
				HistoryInfo.AddInteractCompleteCount(AddInteractCompleteCount);
				return HistoryInfo;
			}
		}
		//没找到就添加一个
		FIS_HistoryBeInteractInfo BeInteractInfo = FIS_HistoryBeInteractInfo(RoleSign, AddInteractCount, AddInteractTime, AddInteractCompleteCount);
		HistoryBeInteracterInfo.Add(BeInteractInfo);
		return BeInteractInfo;
	}
	//清除全部历史交互者的交互时长
	void ClearAllHistoryInteractTime()
	{
		for (FIS_HistoryBeInteractInfo& HistoryInfo : HistoryBeInteracterInfo)
		{
			HistoryInfo.InteractTime = 0.0f;
		}
	}
	//通过角色签名清除交互时长
	void ClearInteractTimeFromRoleSign(FName RoleSign)
	{
		for (FIS_HistoryBeInteractInfo& HistoryInfo : HistoryBeInteracterInfo)
		{
			if (HistoryInfo.InteracterRoleSign == RoleSign)
			{
				HistoryInfo.InteractTime = 0.0f;
				return;
			}
		}
	}
	//通过角色签名设置交互时长
	void SetInteractTimeFromRoleSign(FName RoleSign, float NewInteractTime)
	{
		for (FIS_HistoryBeInteractInfo& HistoryInfo : HistoryBeInteracterInfo)
		{
			if (HistoryInfo.InteracterRoleSign == RoleSign)
			{
				HistoryInfo.InteractTime = NewInteractTime;
				return;
			}
		}
	}
	//通过角色签名获取交互时长
	float GetInteractTimeFromRoleSign(FName RoleSign)
	{
		for (FIS_HistoryBeInteractInfo& HistoryInfo : HistoryBeInteracterInfo)
		{
			if (HistoryInfo.InteracterRoleSign == RoleSign)
			{
				return HistoryInfo.InteractTime;
			}
		}
		return 0.0f;
	}
	//通过角色签名获取交互次数
	int32 GetInteractCountFromRoleSign(FName RoleSign)
	{
		for (FIS_HistoryBeInteractInfo& HistoryInfo : HistoryBeInteracterInfo)
		{
			if (HistoryInfo.InteracterRoleSign == RoleSign)
			{
				return HistoryInfo.InteractCount;
			}
		}
		return -1;
	}
	//通过角色签名清除交互完成次数
	void ClearInteractCompleteCountFromRoleSign(FName RoleSign)
	{
		for (FIS_HistoryBeInteractInfo& HistoryInfo : HistoryBeInteracterInfo)
		{
			if (HistoryInfo.InteracterRoleSign == RoleSign)
			{
				HistoryInfo.InteractCompleteCount = 0;
				return;
			}
		}
	}
	//通过角色签名设置交互完成次数
	void SetInteractCompleteCountFromRoleSign(FName RoleSign, int32 NewInteractCompleteCount)
	{
		for (FIS_HistoryBeInteractInfo& HistoryInfo : HistoryBeInteracterInfo)
		{
			if (HistoryInfo.InteracterRoleSign == RoleSign)
			{
				HistoryInfo.InteractCompleteCount = NewInteractCompleteCount;
				return;
			}
		}
	}
	//通过角色签名获取交互完成次数
	int32 GetInteractCompleteCountFromRoleSign(FName RoleSign)
	{
		for (FIS_HistoryBeInteractInfo& HistoryInfo : HistoryBeInteracterInfo)
		{
			if (HistoryInfo.InteracterRoleSign == RoleSign)
			{
				return HistoryInfo.InteractCompleteCount;
			}
		}
		return -1;
	}
	//获取历史上有多少人跟我产生交互过
	int32 GetHistoryInteractRoleNum()
	{
		return HistoryBeInteracterInfo.Num();
	}
public:
	/*交互验证对象
	* 该对象也可以提前链接一个场景中的对象
	* 如果该值有效，则不会创建CompeleteVerifyClass的对象
	* 如果该值无效且CompeleteVerifyClass有效，则会在交互通过的最后一步创建交互对象
	* 只要有交互对象，交互成功与否的决定就转移为该对象决定
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UObject* InteractVerifyObject;

	//当前跟我交互的全部组件
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UIS_InteractComponent*> AllInteractComponent;

	/*当前完成了交互前摇的组件 / 全部正在尝试交互完成的交互组件
	* 所谓的交互前摇是指通过了交互部分验证、比对，以及经过了交互时间等待最终验证的交互者
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UIS_InteractComponent*> AllTryCompleteInteractComponent;

	//当前跟我交互过的历史信息
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FIS_HistoryBeInteractInfo> HistoryBeInteracterInfo;

	//运行中的交互次数
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 InteractNum = 0;

	/*交互总时间
	* 在BeginPlay和修改时间时计算获得
	* -1表示初始值
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InteractTotalTime = -1.0f;

	//是否开启/激活了交互
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bInteractActive = true;
	//是否还在内部未移出
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsInEnter = false;
	//是否在被交互中
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsInInteract = false;
	//是否正在挂载中
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsInAttach = false;
	//是否完成了交互
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsComplete = false;
	//人数验证是否通过
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bVerifyInteractRoleNumPass = false;
	//是否处于验证中.. 验证对象的验证/人数验证
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsVerifying = false;

	//交互完成次数，该值与多段交互对应
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 InteractCompleteCount = 0;
	//交互统一累计时间
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InteractCumulativeTime = 0.0f;

	/*Trace的进入离开管理
	* 当前进入的全部检测类型
	*/
	UPROPERTY(BlueprintReadWrite)
	TArray<EIS_InteractTraceType> AllEnterTraceType;
};