#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "../../../../../CommonCompare/Source/CommonCompare/Public/CC_StructAndEnum.h"
#include "../Common/IS_Config.h"
#include "IS_BeInterface.generated.h"

class UIS_InteractComponent;
class UUserWidget;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UIS_BeInteractVerifyInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 被交互验证接口
 */
class INTERACTSYSTEM_API IIS_BeInteractVerifyInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/*开始交互验证
	* 调用该函数后需要验证对象自行在后续的内容中处理BeInteractComponent是否完成交互
	* 确定完成时调用BeInteractComponent的InteractComplete接口
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InteractVerifyStart(UIS_InteractComponent* InteractComponent, UIS_BeInteractComponent* BeInteractComponent);
	virtual void InteractVerifyStart_Implementation(UIS_InteractComponent* InteractComponent, UIS_BeInteractComponent* BeInteractComponent) {};
};

//被交互的类型
UENUM(BlueprintType)
enum class EIS_InteractType :uint8//要不要把多段拆开到持续和累计交互中去
{
	//拾取、开关某物等
	Instant = 0 UMETA(DisplayName = "瞬间交互"),
	//开宝箱、充电等。长时间交互
	HasDuration UMETA(DisplayName = "持续交互"),
	////根据不同档位交互时间返回不同档位内容的交互。长时间交互，有多段完成回调
	//MultiSegment UMETA(DisplayName = "多段持续交互"),

	////破解调查进度。长时间交互，交互时长从上次累计的时长处开始
	//CumulativeTime = 50 UMETA(DisplayName = "累计时间交互"),
	////根据不同档位交互时间返回不同档位内容的交互。长时间交互，交互时长从上次累计的时长处开始
	//MultiSegmentCumulativeTime UMETA(DisplayName = "多段累计交互")
};

//交互次数扣除类型
UENUM(BlueprintType)
enum class EIS_InteractNumSubtractType :uint8
{
	NotSubtract = 0 UMETA(DisplayName = "不扣除"),
	Complete UMETA(DisplayName = "交互完成时扣除"),
	End UMETA(DisplayName = "交互结束时扣除")
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

/*历史被交互的信息
* 
*/
USTRUCT(BlueprintType)
struct FIS_HistoryBeInteractInfo
{
	GENERATED_BODY()

public:
	FIS_HistoryBeInteractInfo(){}
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
	int32 AddInteractCount(int32 AddNum){ InteractCount += AddNum; return InteractCount; }
	int32 AddInteractCompleteCount(int32 AddCompleteNum) { InteractCompleteCount += AddCompleteNum; return InteractCompleteCount; }
	float AddInteractTime(float AddTime){ InteractTime += AddTime; return InteractTime; }
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

/*被交互的信息
* 通常是可配置的初始化信息
*/
USTRUCT(BlueprintType)
struct FIS_BeInteractInfo : public FTableRowBase
{
	GENERATED_BODY()
public:
	//交互UI显示文本
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText InteractText;

	//交互提示UI类
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftClassPtr<UUserWidget> InteractTipPanelClass = UIS_Config::GetInstance()->DefaultInteractTipPanelClass;
	//交互方式类型
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EIS_InteractType InteractType = EIS_InteractType::Instant;
	/*生成挂载事件——在想要实现类似驾驶、操作某物。结束时间自由控制/不存在交互时间的交互时可以开启
	* 开启该函数后在交互完成时不会触发结束而是触发InteractAttachTo
	* 退出/分离挂载需要主动调用InteractAttachDetach
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bGenerateOverlapEvents = false;
	//交互时长
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "InteractType != EIS_InteractType::Instant && InteractType != EIS_InteractType::AttachTo"))
	TArray<float> InteractTime = { 1.0f };
	/*交互时长是否允许累计
	* 通常给长时间交互使用，交互时长从上次累计的时长处开始
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EIS_InteractCumulativeTimeType InteractCumulativeTimeType = EIS_InteractCumulativeTimeType::NotCumulative;

	/*完成交互额外需要的验证类
	* 该验证类可以是UI或者其他——例如QTEUI、3D场景解密
	* 该UI在其他条件全部满足后的最后一步创建
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftClassPtr<UObject> CompeleteVerifyClass;

	/*交互时的角度验证 某侧门不能开启/暗杀等交互需求可以对该值进行设置
	* 设置后交互者必须与该资源X轴的夹角处于该值范围内才允许交互
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FFloatRange InteractAngleVerify;
	//角度验证失败的文本提示
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText InteractAngle_FailText;

	//同时交互人数
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SameTimeInteractRoleNum = 1;
	//交互人数超出时的失败文本提示
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText SameTimeInteractRoleNum_FailText;
	/*完成时的交互人数验证 >=时通过
	* 注意：该配置会影响交互完成功能
	* 如果bRoleNumVerifyIsNowOrHistory = false时该值不应该大于SameTimeInteractRoleNum（同时交互人数），否则有可能永远无法完成交互
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CompleteInteractRoleNumVerify = 1;
	/*人数验证是交互中的人数还是历史交互人数
	* 该值为true表示交互中的人数
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRoleNumVerifyIsNowOrHistory = true;
	//验证交互人数是否满足的间隔
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float VerifyInteractRoleNumTimeInterval = 0.1;

	//交互时长是否允许多人累加 可以理解为大家共同推进进度/共用一个进度条
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "InteractType != EIS_InteractType::Instant && SameTimeInteractRoleNum > 1"))
	bool bEveryoneCumulativeTime = false;
	//交互次数 该值为0时不允许交互
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 InteractNum = 1;
	//交互次数不足时的失败文本提示
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText InteractNum_FailText;
	//交互次数扣除类型
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EIS_InteractNumSubtractType InteractNumSubtractType = EIS_InteractNumSubtractType::NotSubtract;
	/*不同的交互者的交互次数是否分开记录
	* 该值为true时开启功能：此时InteractNum表示被交互物体总共可以被交互的次数
	* EveryoneInteractlNum表示每个人单独可以交互的次数
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bInteractNumIsSeparate = false;
	//每个人的交互次数
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "bInteractNumIsSeparate"))
	int32 EveryoneInteractlNum = 1;
	//每个人的交互次数扣除方式 注意：该值不应该为NotSubtract【不扣除】，不扣除本质上不需要区分每个人的交互次数
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "bInteractNumIsSeparate"))
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
	//未激活时是否显示交互文本
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsNotActiveDisplayInteractText = false;

	//在移出被交互物时，要不要停止交互
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bInteractLeaveIsEnd = true;
	//累计时间交互时的最小累计时长/间隔
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CumulativeTimeInterval = 0.01;

	//被比对的信息
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCC_BeCompareInfo BeCompareInfo;
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

	//当前跟我交互过的历史信息
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FIS_HistoryBeInteractInfo> HistoryBeInteracterInfo;

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
	//是否处于验证中.. 主要是验证对象的验证
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsVerifying = false;

	//交互完成次数，该值与多段交互对应
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 InteractCompleteCount = 0;
	//交互统一累计时间
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InteractCumulativeTime = 0.0f;
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UIS_BeInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 可被交互单位的接口
 */
class INTERACTSYSTEM_API IIS_BeInterface//修改名称BeInteractInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	//获取被交互的信息
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FIS_BeInteractInfo GetBeInteractInfo();
	virtual FIS_BeInteractInfo GetBeInteractInfo_Implementation() { return FIS_BeInteractInfo(); };

	//获取被交互的动态信息
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FIS_BeInteractDynamicInfo GetBeInteractDynamicInfo();
	virtual FIS_BeInteractDynamicInfo GetBeInteractDynamicInfo_Implementation() { return FIS_BeInteractDynamicInfo(); };

	/*
	* 当前是否可以显示交互文本
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsDisplayInteractText();
	virtual bool IsDisplayInteractText_Implementation() { return true; };

	/*
	* 获取交互显示文本
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		FText GetInteractText();
	virtual FText GetInteractText_Implementation(){ return FText(); };

	/*
	* 获取交互类型
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	EIS_InteractType GetInteractType();
	virtual EIS_InteractType GetInteractType_Implementation() { return EIS_InteractType::Instant; };

	/*
	* 获取多段交互次数
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 GetMultiInteractNum();
	virtual int32 GetMultiInteractNum_Implementation() { return 2; };

	/*
	* 获取要交互时长
	* TotalTime：总时长
	* return：每一段的交互时长
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	TArray<float> GetInteractTime(float& TotalTime);
	virtual TArray<float> GetInteractTime_Implementation(float& TotalTime) { TotalTime = 1.0f;return { 1.0f }; };

	/*
	* 获取已经交互过的时长
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	float GetAlreadyInteractTime();
	virtual float GetAlreadyInteractTime_Implementation() { return { 0.0f }; };

	/*
	* 获取交互次数
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 GetInteractNum();
	virtual int32 GetInteractNum_Implementation() { return 1; };

	/*
	* 获取交互次数扣除类型
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	EIS_InteractNumSubtractType GetInteractNumSubtractType();
	virtual EIS_InteractNumSubtractType GetInteractNumSubtractType_Implementation() { return EIS_InteractNumSubtractType::NotSubtract; };

	/*
	* 获取交互次数扣除类型
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FGameplayTagContainer GetInteractTag();
	virtual FGameplayTagContainer GetInteractTag_Implementation() { return FGameplayTagContainer(); };

	/*
	* 获取交互优先级
	* 该值越大交互优先级越高
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 GetInteractPriority();
	virtual int32 GetInteractPriority_Implementation() { return 0; };

	//开启/激活交互
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool SetInteractActive(bool NewActive);
	virtual bool SetInteractActive_Implementation(bool NewActive) { return NewActive; };

	//是否开启了交互
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsInteractActive();
	virtual bool IsInteractActive_Implementation() { return true; };

	/*
	* 是否允许交互
	* 至少要保证可被交互的资源IsEnableInteract（开启/激活了交互）& GetInteractNum（仍有剩余交互次数）后
	* 如果允许多人交互还需要判断交互人数 后
	* 再进行额外的比对，当比对通过时，允许交互
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool CanInteract(UIS_InteractComponent* InteractComponent, FCC_CompareInfo OuterCompareInfo, FText& FailText);
	virtual bool CanInteract_Implementation(UIS_InteractComponent* InteractComponent, FCC_CompareInfo OuterCompareInfo, FText& FailText) { return true; };


	//-------------------------------------以下是事件相关接口

	/*交互完成时的额外验证
	* 该函数会判断是否需要进行额外的验证，包括
	* 验证对象检测、子类覆写的其他成功验证逻辑
	* return：返回会true时表示验证通过，返回false表示验证不通过或有其他待验证的事项
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool InteractCompleteVerifyCheck(UIS_InteractComponent* InteractComponent);
	virtual bool InteractCompleteVerifyCheck_Implementation(UIS_InteractComponent* InteractComponent) { return true; };

	/*创建交互验证对象
	* 需要判断class是UI还是Object还是Actor
	* 需要注意的是UI只能在客户端创建
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UObject* CreateVerifyObject(UIS_InteractComponent* InteractComponent);
	virtual UObject* CreateVerifyObject_Implementation(UIS_InteractComponent* InteractComponent) { return nullptr; };

	/*创建交互验证对象——UI
	* 需要注意的是UI只能在客户端创建
	* 该函数在BeInteractComponent上会被多播
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UUserWidget* CreateVerifyObject_UI(UIS_InteractComponent* InteractComponent, TSubclassOf<UUserWidget> UIClass);
	virtual UUserWidget* CreateVerifyObject_UI_Implementation(UIS_InteractComponent* InteractComponent, TSubclassOf<UUserWidget> UIClass) { return nullptr; };

	/*
	* 移入可交互目标——仅在客户端触发
	* InteractComponent：哪个交互组件移入了“我”
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InteractEnter(UIS_InteractComponent* InteractComponent);
	virtual void InteractEnter_Implementation(UIS_InteractComponent* InteractComponent) {};

	/*
	* 移出可交互目标——仅在客户端触发
	* InteractComponent：哪个交互组件移出了“我”
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InteractLeave(UIS_InteractComponent* InteractComponent);
	virtual void InteractLeave_Implementation(UIS_InteractComponent* InteractComponent) {};

	/*
	* 在移出被交互物时，要不要停止交互
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool InteractLeaveIsEnd();
	virtual bool InteractLeaveIsEnd_Implementation() { return true; };

	/*
	* 尝试交互：该函数会调用InteractCheck，InteractCheck返回为true时调用InteractStart
	* InteractComponent：哪个交互组件向我发起交互请求
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool TryInteract(UIS_InteractComponent* InteractComponent);
	virtual bool TryInteract_Implementation(UIS_InteractComponent* InteractComponent) { return true; };

	/*
	* 交互检测：是否达到可交互的条件
	* InteractComponent：哪个交互组件向我发起交互请求
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool InteractCheck(UIS_InteractComponent* InteractComponent);
	virtual bool InteractCheck_Implementation(UIS_InteractComponent* InteractComponent) { return true; };

	/*
	* 开始交互：InteractCheck通过后调用该函数
	* InteractComponent：哪个交互组件向我发起交互请求
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InteractStart(UIS_InteractComponent* InteractComponent);
	virtual void InteractStart_Implementation(UIS_InteractComponent* InteractComponent) {};

	/*
	* 结束交互
	* InteractComponent：哪个交互组件向我发起交互请求
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InteractEnd(UIS_InteractComponent* InteractComponent);
	virtual void InteractEnd_Implementation(UIS_InteractComponent* InteractComponent) {};

	/*
	* 完成交互——所有的交互条件和验证均通过时，该函数会被调用
	* InteractComponent：哪个交互组件向我发起交互请求
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InteractComplete(UIS_InteractComponent* InteractComponent);
	virtual void InteractComplete_Implementation(UIS_InteractComponent* InteractComponent) {};

	/*
	* 多段完成交互——当交互时长的数组长度 > 1时，在交互过程中达到某个时长后该函数会被调用
	* InteractComponent：哪个交互组件向我发起交互请求
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InteractComplete_MultiSegment(UIS_InteractComponent* InteractComponent);
	virtual void InteractComplete_MultiSegment_Implementation(UIS_InteractComponent* InteractComponent) {};

	/*
	* 交互挂载——交互类型为AttachTo（挂载交互）时，完成交互的时候会调用该接口表示交互进入
	* InteractComponent：哪个交互组件向我发起交互请求
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InteractAttachTo(UIS_InteractComponent* InteractComponent);
	virtual void InteractAttachTo_Implementation(UIS_InteractComponent* InteractComponent) {};

	/*
	* 分离挂载交互——交互类型为AttachTo（挂载交互）时，需要主动调用该接口表示交互退出
	* InteractComponent：哪个交互组件向我发起交互请求
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InteractAttachDetach(UIS_InteractComponent* InteractComponent);
	virtual void InteractAttachDetach_Implementation(UIS_InteractComponent* InteractComponent) {};
};
