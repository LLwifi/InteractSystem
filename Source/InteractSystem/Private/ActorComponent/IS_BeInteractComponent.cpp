// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent/IS_BeInteractComponent.h"
#include "Engine/ActorChannel.h"
#include "ActorComponent/IS_InteractComponent.h"
#include "Engine/AssetManager.h"
#include "Blueprint/UserWidget.h"
#include <Kismet/KismetMathLibrary.h>
#include "Components/MeshComponent.h"
#include "ActorComponent/BeInteractExtend/IS_BeInteractExtendBase.h"
#include <Library/IS_BlueprintFunctionLibrary.h>

// Sets default values for this component's properties
UIS_BeInteractComponent::UIS_BeInteractComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	//PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
	// ...
	SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
}

#if WITH_EDITOR

void UIS_BeInteractComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FProperty* Property = PropertyChangedEvent.Property;//拿到改变的属性
}

#endif

void UIS_BeInteractComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UIS_BeInteractComponent, BeInteractDynamicInfo);

	DOREPLIFETIME(UIS_BeInteractComponent, AllExtend);
}

bool UIS_BeInteractComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bWrote = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	//手动同步扩展类
	for (UIS_BeInteractExtendBase*& Extend : AllExtend)
	{
		if (!IsValid(Extend))
		{
			continue;
		}
		bWrote |= Channel->ReplicateSubobject(Extend, *Bunch, *RepFlags);
	}

	return bWrote;
}

// Called when the game starts
void UIS_BeInteractComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	//读取数据表获取数据
	if(bIsUseDataTable)
	{
		UIS_BlueprintFunctionLibrary::GetBeInteractInfoFromHandle(BeInteractInfoHandle, BeInteractInfo);
	}

	if (GetOwner()->GetLocalRole() == ENetRole::ROLE_Authority)
	{
		//生成扩展
		TArray<FIS_BeInteractExtendHandle> UseBeInteractExtend;
		if (bIsOverrideBeInteractExtend)
		{
			UseBeInteractExtend = OverrideBeInteractExtendHandles;
		}
		else
		{
			UseBeInteractExtend = BeInteractInfo.BeInteractExtendHandle;
		}

		UseBeInteractExtend.Append(AddBeInteractExtendHandles);

		for (FIS_BeInteractExtendHandle& BeInteractExtendHandle : UseBeInteractExtend)
		{
			FIS_BeInteractExtend BeInteractExtendInfo;
			UIS_BlueprintFunctionLibrary::GetBeInteractExtendFromHandle(BeInteractExtendHandle, BeInteractExtendInfo);

			UIS_BeInteractExtendBase* BeInteractExtend = NewObject<UIS_BeInteractExtendBase>(this, BeInteractExtendInfo.BeInteractExtendClass);
			if (BeInteractExtend)
			{
				BeInteractExtend->Init(this, BeInteractExtendInfo.BeInteractExtend);
				AllExtend.Add(BeInteractExtend);
			}
		}
	}

	IIS_BeInteractInterface::Execute_SetInteractTime(this, BeInteractInfo.InteractTime);
	IIS_BeInteractInterface::Execute_SetInteractNum(this, BeInteractInfo.InteractNum);
	IIS_BeInteractInterface::Execute_SetInteractActive(this, BeInteractInfo.bDefaultInteractActive);
}

void UIS_BeInteractComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}


// Called every frame
void UIS_BeInteractComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

FIS_BeInteractInfo UIS_BeInteractComponent::GetBeInteractInfo_Implementation()
{
	return BeInteractInfo;
}

FIS_BeInteractDynamicInfo UIS_BeInteractComponent::GetBeInteractDynamicInfo_Implementation()
{
	return BeInteractDynamicInfo;
}

bool UIS_BeInteractComponent::IsDisplayInteractText_Implementation()
{
	if (!BeInteractInfo.InteractText.IsEmpty() && (BeInteractDynamicInfo.bInteractActive ? true : BeInteractInfo.bNotActiveIsDisplayInteractText))
	{
		return true;
	}
	return false;
}

FText UIS_BeInteractComponent::GetInteractText_Implementation()
{
	return BeInteractInfo.InteractText;
}

void UIS_BeInteractComponent::SetInteractText_Implementation(const FText& InteractText)
{
	BeInteractInfo.InteractText = InteractText;
}

EIS_InteractType UIS_BeInteractComponent::GetInteractType_Implementation()
{
	return BeInteractInfo.InteractType;
}

int32 UIS_BeInteractComponent::GetMultiInteractNum_Implementation()
{
	return BeInteractInfo.InteractTime.Num();
}

TArray<float> UIS_BeInteractComponent::GetInteractTime_Implementation(float& TotalTime)
{
	//TotalTime = 0.0f;//不加这一句，多次调用该函数会累计
	TotalTime = BeInteractDynamicInfo.InteractTotalTime;
	return BeInteractInfo.InteractTime;
}

float UIS_BeInteractComponent::GetAlreadyInteractTime_Implementation()
{
	return BeInteractDynamicInfo.InteractCumulativeTime;
}

float UIS_BeInteractComponent::SetInteractTime_Implementation(const TArray<float>& InteractTime)
{
	BeInteractInfo.InteractTime = InteractTime;
	if (BeInteractInfo.InteractType != EIS_InteractType::Instant)//非瞬间交互才存在交互时间
	{
		BeInteractDynamicInfo.InteractTotalTime = 0.0f;
		for (float& f : BeInteractInfo.InteractTime)
		{
			BeInteractDynamicInfo.InteractTotalTime += f;
		}
	}

	/*多段交互 且 累计的特殊处理 这个处理得在BeInteractDynamicInfo.InteractTotalTime计算（上文）之后，否则会使交互时长的计算变多
	* 该处理会使{3.0f,3.0f,3.0f}——》{3.0f,6.0f,9.0f}
	* 原因：多段交互时 累计时长不会清零，需要通过计算（减上一个下标的值）得出下次交互所需的时长
	* 在比对是否完成某个阶段时，处理后才能比对正确，第二次交互了1.5秒的比对应该是4.5 < 6.0,属于未完成
	*/
	if (BeInteractInfo.InteractTime.Num() > 1 && BeInteractInfo.InteractCumulativeTimeType != EIS_InteractCumulativeTimeType::NotCumulative)
	{
		for (int32 i = 0; i < BeInteractInfo.InteractTime.Num(); i++)
		{
			if (i > 0)//从第二个时间开始处理
			{
				BeInteractInfo.InteractTime[i] += BeInteractInfo.InteractTime[i - 1];
			}
		}
	}

	return BeInteractDynamicInfo.InteractTotalTime;
}

int32 UIS_BeInteractComponent::GetInteractNum_Implementation()
{
	return BeInteractDynamicInfo.InteractNum;
}

int32 UIS_BeInteractComponent::SetInteractNum_Implementation(int32 NewInteractNum)
{
	BeInteractDynamicInfo.InteractNum = NewInteractNum;
	if (BeInteractInfo.bInteractActiveFromInteractNum)
	{
		IIS_BeInteractInterface::Execute_SetInteractActive(this, BeInteractDynamicInfo.InteractNum > 0);
	}
	return int32();
}

EIS_InteractNumSubtractType UIS_BeInteractComponent::GetInteractNumSubtractType_Implementation()
{
	return BeInteractInfo.InteractNumSubtractType;
}

FGameplayTagContainer UIS_BeInteractComponent::GetInteractTag_Implementation()
{
	return BeInteractInfo.InteractTag;
}

int32 UIS_BeInteractComponent::GetInteractPriority_Implementation()
{
	return BeInteractInfo.InteractPriority;
}

bool UIS_BeInteractComponent::SetInteractActive_Implementation(bool NewActive)
{
	BeInteractDynamicInfo.bInteractActive = NewActive;
	return BeInteractDynamicInfo.bInteractActive;
}

bool UIS_BeInteractComponent::IsInteractActive_Implementation()
{
	return BeInteractDynamicInfo.bInteractActive;
}

bool UIS_BeInteractComponent::CanInteract_Implementation(UIS_InteractComponent* InteractComponent, FCC_CompareInfo OuterCompareInfo, FText& FailText)
{
	FailText = FText();
	if (IIS_BeInteractInterface::Execute_IsInteractActive(this))//是否激活
	{
		bool IsRoleHaveInteractNum = true;//交互者是否还有交互次数
		//不同的交互者的交互次数是否分开记录
		if (BeInteractInfo.bInteractNumIsMultiplepeople)
		{
			IsRoleHaveInteractNum = BeInteractDynamicInfo.GetInteractCountFromRoleSign(InteractComponent->GetRoleSign()) < BeInteractInfo.EveryoneInteractlNum;
		}
		if (IIS_BeInteractInterface::Execute_GetInteractNum(this) > 0 && IsRoleHaveInteractNum)//交互次数是否足够
		{
			if (BeInteractDynamicInfo.AllInteractComponent.Num() <= BeInteractInfo.SameTimeInteractRoleNum)//同时交互人数
			{
				if (BeInteractInfo.InteractVerifyInfo.Verify(InteractComponent, this, FailText))//交互验证
				{
					if (CanInteract_Extend(InteractComponent, OuterCompareInfo, FailText))//扩展类的额外判断
					{
						return BeInteractInfo.BeCompareInfo.CompareResult(OuterCompareInfo, FailText);
					}
				}
			}
			else
			{
				FailText = BeInteractInfo.SameTimeInteractRoleNum_FailText;
			}
		}
		else
		{
			FailText = BeInteractInfo.InteractNum_FailText;
		}
	}
	else if(BeInteractInfo.bNotActiveIsDisplayInteractText)//没激活是否显示交互文本 包括了报错文本
	{
		FailText = BeInteractInfo.InteractActive_FailText;
	}
	return false;
}

FIS_InteractCompleteVerifyInfo UIS_BeInteractComponent::SetInteractCompleteVerifyInfo_Implementation(FIS_InteractCompleteVerifyInfo NewInteractCompleteVerifyInfo)
{
	BeInteractInfo.InteractCompleteVerifyInfo = NewInteractCompleteVerifyInfo;
	return BeInteractInfo.InteractCompleteVerifyInfo;
}

bool UIS_BeInteractComponent::InteractCompleteVerifyCheck_Implementation(UIS_InteractComponent* InteractComponent)
{
	BeInteractDynamicInfo.bIsVerifying = true;
	//人数验证
	InteractRoleNumVerifyBack();//首次主动调用，判断是否通过
	if (BeInteractDynamicInfo.bVerifyInteractRoleNumPass)
	{
		//其他验证需求
		if (BeInteractDynamicInfo.InteractVerifyObject || !BeInteractInfo.InteractCompleteVerifyInfo.CompleteVerifyClass.IsNull())
		{
			if (!BeInteractDynamicInfo.InteractVerifyObject)//是否需要创建验证对象
			{
				BeInteractDynamicInfo.InteractVerifyObject = IIS_BeInteractInterface::Execute_CreateVerifyObject(this, InteractComponent);
			}

			if (BeInteractDynamicInfo.InteractVerifyObject)//是否有额外的验证对象
			{
				IIS_BeInteractVerifyInterface::Execute_InteractVerifyStart(BeInteractDynamicInfo.InteractVerifyObject, InteractComponent, this);
			}

			switch (InteractEventNetType)
			{
			case EIS_InteractEventNetType::Server:
			{
				OnInteractVerify.Broadcast(InteractComponent, BeInteractDynamicInfo.InteractVerifyObject);
				break;
			}
			case EIS_InteractEventNetType::Client:
			{
				NetClient_OnInteractVerify(InteractComponent, BeInteractDynamicInfo.InteractVerifyObject);
				break;
			}
			case EIS_InteractEventNetType::NetMulticast:
			{
				NetMulti_OnInteractVerify(InteractComponent, BeInteractDynamicInfo.InteractVerifyObject);
				break;
			}
			default:
				break;
			}
			return false;
		}
		return true;
	}
	//未通过开启Timer进行检测
	GetWorld()->GetTimerManager().SetTimer(BeInteractOtherInfo.InteractRoleNumVerifyTimerHandle, this, &UIS_BeInteractComponent::InteractRoleNumVerifyBack,
		BeInteractInfo.InteractCompleteVerifyInfo.VerifyInteractRoleNumTimeInterval,true);
	return false;
}

UObject* UIS_BeInteractComponent::CreateVerifyObject_Implementation(UIS_InteractComponent* InteractComponent)
{
	//需要判断class是UI还是Object还是Actor
	UClass* CreateClass = UAssetManager::GetStreamableManager().LoadSynchronous(BeInteractInfo.InteractCompleteVerifyInfo.CompleteVerifyClass);
	if (CreateClass)
	{
		if (CreateClass->IsChildOf(AActor::StaticClass()))//AActor
		{
			return GetWorld()->SpawnActor<AActor>(CreateClass);
		}
		else if(CreateClass->IsChildOf(UUserWidget::StaticClass()))//UUserWidget
		{
			CreateVerifyUI(InteractComponent, CreateClass);
			return nullptr;//UI不能创建在服务器上
			//UUserWidget* UI = CreateWidget(GetWorld(), CreateClass);
			//if (UI)
			//{
			//	UI->AddToViewport();
			//}
			//return UI;
		}
		else//Object
		{
			return NewObject<UObject>(this, CreateClass);
		}
	}

	return nullptr;
}

UUserWidget* UIS_BeInteractComponent::CreateVerifyObject_UI_Implementation(UIS_InteractComponent* InteractComponent, TSubclassOf<UUserWidget> UIClass)
{
	UUserWidget* UI = nullptr;
	if (InteractComponent && InteractComponent->GetOwner()->GetLocalRole() == ENetRole::ROLE_AutonomousProxy)
	{
		if (!BeInteractDynamicInfo.InteractVerifyObject)//是否需要创建验证对象
		{
			UI = CreateWidget(GetWorld(), UIClass);
			if (UI)
			{
				UI->AddToViewport();
				BeInteractDynamicInfo.InteractVerifyObject = UI;
			}
		}
		if (BeInteractDynamicInfo.InteractVerifyObject)//是否有额外的验证对象
		{
			IIS_BeInteractVerifyInterface::Execute_InteractVerifyStart(BeInteractDynamicInfo.InteractVerifyObject, InteractComponent, this);
		}
	}
	return UI;
}

void UIS_BeInteractComponent::InteractEnter_Implementation(UIS_InteractComponent* InteractComponent, EIS_InteractTraceType TraceType)
{
	InteractTraceCheck(TraceType);
	BeInteractDynamicInfo.bIsInEnter = true;
	switch (InteractEventNetType)
	{
	case EIS_InteractEventNetType::Server:
	{
		OnInteractEnter.Broadcast(InteractComponent, TraceType);
		break;
	}
	case EIS_InteractEventNetType::Client:
	{
		NetClient_OnInteractEnter(InteractComponent, TraceType);
		break;
	}
	case EIS_InteractEventNetType::NetMulticast:
	{
		NetMulti_OnInteractEnter(InteractComponent, TraceType);
		break;
	}
	default:
		break;
	}

	//调用扩展对象的同名函数
	for (UIS_BeInteractExtendBase*& BeInteractExtend : AllExtend)
	{
		IIS_BeInteractInterface::Execute_InteractEnter(BeInteractExtend, InteractComponent, TraceType);
	}
}

void UIS_BeInteractComponent::InteractLeave_Implementation(UIS_InteractComponent* InteractComponent, EIS_InteractTraceType TraceType)
{
	InteractTraceCheck(TraceType,false);
	BeInteractDynamicInfo.bIsInEnter = false;
	if (IIS_BeInteractInterface::Execute_InteractLeaveIsEnd(this) && BeInteractDynamicInfo.bIsInInteract)//在移出被交互物时，要不要停止交互
	{
		IIS_BeInteractInterface::Execute_InteractEnd(this, InteractComponent);
	}
	switch (InteractEventNetType)
	{
	case EIS_InteractEventNetType::Server:
	{
		OnInteractLeave.Broadcast(InteractComponent, TraceType);
		break;
	}
	case EIS_InteractEventNetType::Client:
	{
		NetClient_OnInteractLeave(InteractComponent, TraceType);
		break;
	}
	case EIS_InteractEventNetType::NetMulticast:
	{
		NetMulti_OnInteractLeave(InteractComponent, TraceType);
		break;
	}
	default:
		break;
	}

	//调用扩展对象的同名函数
	for (UIS_BeInteractExtendBase*& BeInteractExtend : AllExtend)
	{
		IIS_BeInteractInterface::Execute_InteractLeave(BeInteractExtend, InteractComponent, TraceType);
	}
}

bool UIS_BeInteractComponent::InteractLeaveIsEnd_Implementation()
{
	return BeInteractInfo.bInteractLeaveIsEnd;
}

void UIS_BeInteractComponent::InteractStart_Implementation(UIS_InteractComponent* InteractComponent)
{
	BeInteractDynamicInfo.bIsInInteract = true;
	BeInteractDynamicInfo.AllInteractComponent.Add(InteractComponent);

	if (BeInteractInfo.InteractNumSubtractType == EIS_InteractNumSubtractType::Start)
	{
		IIS_BeInteractInterface::Execute_SetInteractNum(this, BeInteractDynamicInfo.InteractNum - 1);
	}

	switch (InteractEventNetType)
	{
	case EIS_InteractEventNetType::Server:
	{
		OnInteractStart.Broadcast(InteractComponent);
		break;
	}
	case EIS_InteractEventNetType::Client:
	{
		NetClient_OnInteractStart(InteractComponent);
		break;
	}
	case EIS_InteractEventNetType::NetMulticast:
	{
		NetMulti_OnInteractStart(InteractComponent);
		break;
	}
	default:
		break;
	}

	/*如果不同的交互者的交互次数不分开记录，这里也记录一下当作历史交互单位
	* 如果需要分开记录会在结束或完成时根据配置添加
	*/
	if (!BeInteractInfo.bInteractNumIsMultiplepeople)
	{
		BeInteractDynamicInfo.RecordInteractInfo(InteractComponent->GetRoleSign(), 1, 0.0f, 0);
	}

	FTimerHandle TimeHandle;
	switch (BeInteractInfo.InteractType)
	{
	case EIS_InteractType::Instant://瞬间交互直接完成
	{
		InteractTimerBack();
		break;
	}
	case EIS_InteractType::HasDuration://持续交互
	{
		//交互时长是否允许多人累加 || 交互时长本身是否允许累计
		if (BeInteractInfo.bEveryoneCumulativeTime || BeInteractInfo.InteractCumulativeTimeType != EIS_InteractCumulativeTimeType::NotCumulative)
		{
			GetWorld()->GetTimerManager().SetTimer(TimeHandle, this, &UIS_BeInteractComponent::InteractTimerBack,
				BeInteractInfo.CumulativeTimeInterval, true);
		}
		else
		{
			if (BeInteractInfo.InteractTime.Num() > 1)//交互时长是否拥有多段
			{
				GetWorld()->GetTimerManager().SetTimer(TimeHandle, this, &UIS_BeInteractComponent::InteractTimerBack,
					BeInteractInfo.InteractTime[BeInteractDynamicInfo.InteractCompleteCount]);//从第一段开始交互
			}
			else
			{
				GetWorld()->GetTimerManager().SetTimer(TimeHandle, this, &UIS_BeInteractComponent::InteractTimerBack,
					BeInteractDynamicInfo.InteractTotalTime);
			}
		}
		break;
	}
	default:
		break;
	}
	BeInteractOtherInfo.InteractTimerHandle.Add(InteractComponent->GetRoleSign(), TimeHandle);

	//调用扩展对象的同名函数
	for (UIS_BeInteractExtendBase*& BeInteractExtend : AllExtend)
	{
		IIS_BeInteractInterface::Execute_InteractStart(BeInteractExtend, InteractComponent);
	}
}

void UIS_BeInteractComponent::InteractEnd_Implementation(UIS_InteractComponent* InteractComponent)
{
	//不同的人同时交互，A先完成了，扣除交互次数，此时B应该被打断或无法完成
	//交互结束需要清除历史记录中交互者的交互时间

	//重置参数
	BeInteractDynamicInfo.AllInteractComponent.Remove(InteractComponent);//从当前跟我交互的全部组件移除这个结束的交互者
	BeInteractDynamicInfo.AllTryCompleteInteractComponent.Remove(InteractComponent);
	BeInteractDynamicInfo.bIsInInteract = BeInteractDynamicInfo.AllInteractComponent.Num() > 0 ? true : false;//还有其他人交互吗
	//不是由完成交互触发的结束交互
	if (!BeInteractDynamicInfo.bIsComplete)
	{
		//没完成交互的话，判断还有其他人在交互吗

		if (BeInteractOtherInfo.InteractTimerHandle.Contains(InteractComponent->GetRoleSign()))//我结束时有没有与被交互目标产生过TimerHandle
		{
			GetWorld()->GetTimerManager().ClearTimer(BeInteractOtherInfo.InteractTimerHandle[InteractComponent->GetRoleSign()]);
		}
		BeInteractOtherInfo.InteractTimerHandle.Remove(InteractComponent->GetRoleSign());//移除结束交互者的TimerHandle
		
		switch (BeInteractInfo.InteractCumulativeTimeType)//交互累计类型
		{
		case EIS_InteractCumulativeTimeType::NotCumulative://不累计
		{
			if (!BeInteractDynamicInfo.bIsInInteract)//没有其他人在交互了
			{
				BeInteractDynamicInfo.InteractCumulativeTime = 0.0f;//清除统一累计时长
			}
			BeInteractDynamicInfo.ClearInteractTimeFromRoleSign(InteractComponent->GetRoleSign());//清除结束者的时长
			BeInteractDynamicInfo.ClearInteractCompleteCountFromRoleSign(InteractComponent->GetRoleSign());//清除该交互者的交互完成次数
			break;
		}
		case EIS_InteractCumulativeTimeType::Interval://按间隔累计
		{
			//间隔累计 每个交互者的交互时间和统一交互时间都不管
			break;
		}
		case EIS_InteractCumulativeTimeType::InteractTimeIndex://按分段累计
		{
			//分段累计回退到上一个记录点
			if (BeInteractInfo.bEveryoneCumulativeTime)//是否允许多人统一累加时间
			{
				BeInteractDynamicInfo.InteractCumulativeTime = BeInteractDynamicInfo.InteractCompleteCount == 0 ? 0.0f : BeInteractInfo.InteractTime[BeInteractDynamicInfo.InteractCompleteCount - 1];
			}
			else
			{
				float BackTimePoint = BeInteractDynamicInfo.GetInteractCompleteCountFromRoleSign(InteractComponent->GetRoleSign()) <= 0 ? 0.0f : 
				BeInteractInfo.InteractTime[BeInteractDynamicInfo.GetInteractCompleteCountFromRoleSign(InteractComponent->GetRoleSign()) - 1];
				BeInteractDynamicInfo.SetInteractTimeFromRoleSign(InteractComponent->GetRoleSign(), BackTimePoint);
			}
			break;
		}
		default:
			break;
		}

		//不是完成交互触发的交互结束才减少交互次数，避免次数被多次减少
		if (BeInteractDynamicInfo.bInteractActive)//交互激活的资源才可能去扣除次数
		{
			if (BeInteractInfo.bInteractNumIsMultiplepeople)//交互次数分开记录吗
			{
				if (BeInteractInfo.EveryoneInteractNumSubtractType == EIS_InteractNumSubtractType::End)
				{
					BeInteractDynamicInfo.RecordInteractInfo(InteractComponent->GetRoleSign(), 1, 0.0f, 0);
				}
			}
			else
			{
				if (BeInteractInfo.InteractNumSubtractType == EIS_InteractNumSubtractType::End)
				{
					IIS_BeInteractInterface::Execute_SetInteractNum(this, BeInteractDynamicInfo.InteractNum - 1);
				}
			}
		}
	}

	BeInteractDynamicInfo.bIsComplete = false;
	BeInteractDynamicInfo.bVerifyInteractRoleNumPass = false;
	GetWorld()->GetTimerManager().ClearTimer(BeInteractOtherInfo.InteractRoleNumVerifyTimerHandle);//停止检测交互人数

	switch (InteractEventNetType)
	{
	case EIS_InteractEventNetType::Server:
	{
		OnInteractEnd.Broadcast(InteractComponent);
		break;
	}
	case EIS_InteractEventNetType::Client:
	{
		NetClient_OnInteractEnd(InteractComponent);
		break;
	}
	case EIS_InteractEventNetType::NetMulticast:
	{
		NetMulti_OnInteractEnd(InteractComponent);
		break;
	}
	default:
		break;
	}

	//调用扩展对象的同名函数
	for (UIS_BeInteractExtendBase*& BeInteractExtend : AllExtend)
	{
		IIS_BeInteractInterface::Execute_InteractEnd(BeInteractExtend, InteractComponent);
	}
}

void UIS_BeInteractComponent::InteractComplete_Implementation(UIS_InteractComponent* InteractComponent)
{
	//不同的人同时交互，A先完成了，扣除交互次数，此时B应该被打断或无法完成

	BeInteractDynamicInfo.bIsComplete = true;
	BeInteractDynamicInfo.bIsVerifying = false;
	BeInteractDynamicInfo.InteractCumulativeTime = 0.0f;//清除统一累计时长
	BeInteractDynamicInfo.ClearInteractTimeFromRoleSign(InteractComponent->GetRoleSign());//交互完成需要清除历史记录中该交互者的交互时间
	BeInteractDynamicInfo.ClearInteractCompleteCountFromRoleSign(InteractComponent->GetRoleSign());//清除该交互者的交互完成次数
	BeInteractDynamicInfo.InteractCompleteCount = 0;//清除多段交互完成次数
	GetWorld()->GetTimerManager().ClearTimer(BeInteractOtherInfo.InteractRoleNumVerifyTimerHandle);//停止检测交互人数
	//停掉全部Timer
	for (TPair<FName, FTimerHandle>& pair : BeInteractOtherInfo.InteractTimerHandle)
	{
		GetWorld()->GetTimerManager().ClearTimer(pair.Value);
	}
	BeInteractOtherInfo.InteractTimerHandle.Empty();
	BeInteractDynamicInfo.AllInteractComponent.Empty();//清除当前跟我交互的全部组件
	BeInteractDynamicInfo.AllTryCompleteInteractComponent.Empty();

	if (BeInteractInfo.bInteractNumIsMultiplepeople)//交互次数分开记录吗
	{
		if (BeInteractInfo.EveryoneInteractNumSubtractType == EIS_InteractNumSubtractType::Complete)
		{
			BeInteractDynamicInfo.RecordInteractInfo(InteractComponent->GetRoleSign(), 1, 0.0f, 0);
		}
	}
	else
	{
		if (BeInteractInfo.InteractNumSubtractType == EIS_InteractNumSubtractType::Complete)
		{
			IIS_BeInteractInterface::Execute_SetInteractNum(this, BeInteractDynamicInfo.InteractNum - 1);
		}
	}

	switch (InteractEventNetType)
	{
	case EIS_InteractEventNetType::Server:
	{
		OnInteractComplete.Broadcast(InteractComponent);
		break;
	}
	case EIS_InteractEventNetType::Client:
	{
		NetClient_OnInteractComplete(InteractComponent);
		break;
	}
	case EIS_InteractEventNetType::NetMulticast:
	{
		NetMulti_OnInteractComplete(InteractComponent);
		break;
	}
	default:
		break;
	}

	if (BeInteractInfo.bGenerateAttachEvents)//如果需要生成挂载交互事件，在交互完成时才算开始
	{
		IIS_BeInteractInterface::Execute_InteractAttachTo(this, InteractComponent);
	}
	else
	{
		InteractComponent->EndCurInteract();//完成时使交互组件结束交互
	}

	//调用扩展对象的同名函数
	for (UIS_BeInteractExtendBase*& BeInteractExtend : AllExtend)
	{
		IIS_BeInteractInterface::Execute_InteractComplete(BeInteractExtend, InteractComponent);
	}
}

void UIS_BeInteractComponent::InteractComplete_MultiSegment_Implementation(UIS_InteractComponent* InteractComponent)
{
	switch (InteractEventNetType)
	{
	case EIS_InteractEventNetType::Server:
	{
		OnInteractComplete_MultiSegment.Broadcast(InteractComponent);
		break;
	}
	case EIS_InteractEventNetType::Client:
	{
		NetClient_OnInteractComplete_MultiSegment(InteractComponent);
		break;
	}
	case EIS_InteractEventNetType::NetMulticast:
	{
		NetMulti_OnInteractComplete_MultiSegment(InteractComponent);
		break;
	}
	default:
		break;
	}

	//调用扩展对象的同名函数
	for (UIS_BeInteractExtendBase*& BeInteractExtend : AllExtend)
	{
		IIS_BeInteractInterface::Execute_InteractComplete_MultiSegment(BeInteractExtend, InteractComponent);
	}
}

void UIS_BeInteractComponent::InteractAttachTo_Implementation(UIS_InteractComponent* InteractComponent)
{
	BeInteractDynamicInfo.bIsInAttach = true;

	switch (InteractEventNetType)
	{
	case EIS_InteractEventNetType::Server:
	{
		OnInteractAttachTo.Broadcast(InteractComponent);
		break;
	}
	case EIS_InteractEventNetType::Client:
	{
		NetClient_OnInteractAttachTo(InteractComponent);
		break;
	}
	case EIS_InteractEventNetType::NetMulticast:
	{
		NetMulti_OnInteractAttachTo(InteractComponent);
		break;
	}
	default:
		break;
	}

	//调用扩展对象的同名函数
	for (UIS_BeInteractExtendBase*& BeInteractExtend : AllExtend)
	{
		IIS_BeInteractInterface::Execute_InteractAttachTo(BeInteractExtend, InteractComponent);
	}
}

void UIS_BeInteractComponent::InteractAttachDetach_Implementation(UIS_InteractComponent* InteractComponent)
{
	BeInteractDynamicInfo.bIsInAttach = false;

	switch (InteractEventNetType)
	{
	case EIS_InteractEventNetType::Server:
	{
		OnInteractAttachDetach.Broadcast(InteractComponent);
		break;
	}
	case EIS_InteractEventNetType::Client:
	{
		NetClient_OnInteractAttachDetach(InteractComponent);
		break;
	}
	case EIS_InteractEventNetType::NetMulticast:
	{
		NetMulti_OnInteractAttachDetach(InteractComponent);
		break;
	}
	default:
		break;
	}

	InteractComponent->EndCurInteract();//分离时使交互组件结束交互

	//调用扩展对象的同名函数
	for (UIS_BeInteractExtendBase*& BeInteractExtend : AllExtend)
	{
		IIS_BeInteractInterface::Execute_InteractAttachDetach(BeInteractExtend, InteractComponent);
	}
}

void UIS_BeInteractComponent::NetMulti_OnInteractVerify_Implementation(UIS_InteractComponent* InteractComponent, UObject* VerifyObject)
{
	OnInteractVerify.Broadcast(InteractComponent, VerifyObject);
}

void UIS_BeInteractComponent::NetClient_OnInteractVerify_Implementation(UIS_InteractComponent* InteractComponent, UObject* VerifyObject)
{
	OnInteractVerify.Broadcast(InteractComponent, VerifyObject);
}

void UIS_BeInteractComponent::NetClient_OnInteractEnter_Implementation(UIS_InteractComponent* InteractComponent, EIS_InteractTraceType TraceType)
{
	OnInteractEnter.Broadcast(InteractComponent, TraceType);
}

void UIS_BeInteractComponent::NetMulti_OnInteractEnter_Implementation(UIS_InteractComponent* InteractComponent, EIS_InteractTraceType TraceType)
{
	OnInteractEnter.Broadcast(InteractComponent, TraceType);
}

void UIS_BeInteractComponent::NetClient_OnInteractLeave_Implementation(UIS_InteractComponent* InteractComponent, EIS_InteractTraceType TraceType)
{
	OnInteractLeave.Broadcast(InteractComponent, TraceType);
}

void UIS_BeInteractComponent::NetMulti_OnInteractLeave_Implementation(UIS_InteractComponent* InteractComponent, EIS_InteractTraceType TraceType)
{
	OnInteractLeave.Broadcast(InteractComponent, TraceType);
}

void UIS_BeInteractComponent::NetClient_OnInteractStart_Implementation(UIS_InteractComponent* InteractComponent)
{
	OnInteractStart.Broadcast(InteractComponent);
}

void UIS_BeInteractComponent::NetMulti_OnInteractStart_Implementation(UIS_InteractComponent* InteractComponent)
{
	OnInteractStart.Broadcast(InteractComponent);
}

void UIS_BeInteractComponent::NetClient_OnInteractEnd_Implementation(UIS_InteractComponent* InteractComponent)
{
	OnInteractEnd.Broadcast(InteractComponent);
}

void UIS_BeInteractComponent::NetMulti_OnInteractEnd_Implementation(UIS_InteractComponent* InteractComponent)
{
	OnInteractEnd.Broadcast(InteractComponent);
}

void UIS_BeInteractComponent::NetClient_OnInteractComplete_Implementation(UIS_InteractComponent* InteractComponent)
{
	OnInteractComplete.Broadcast(InteractComponent);
}

void UIS_BeInteractComponent::NetMulti_OnInteractComplete_Implementation(UIS_InteractComponent* InteractComponent)
{
	OnInteractComplete.Broadcast(InteractComponent);
}

void UIS_BeInteractComponent::NetClient_OnInteractComplete_MultiSegment_Implementation(UIS_InteractComponent* InteractComponent)
{
	OnInteractComplete_MultiSegment.Broadcast(InteractComponent);
}

void UIS_BeInteractComponent::NetMulti_OnInteractComplete_MultiSegment_Implementation(UIS_InteractComponent* InteractComponent)
{
	OnInteractComplete_MultiSegment.Broadcast(InteractComponent);
}

void UIS_BeInteractComponent::NetClient_OnInteractAttachTo_Implementation(UIS_InteractComponent* InteractComponent)
{
	OnInteractAttachTo.Broadcast(InteractComponent);
}

void UIS_BeInteractComponent::NetMulti_OnInteractAttachTo_Implementation(UIS_InteractComponent* InteractComponent)
{
	OnInteractAttachTo.Broadcast(InteractComponent);
}

void UIS_BeInteractComponent::NetClient_OnInteractAttachDetach_Implementation(UIS_InteractComponent* InteractComponent)
{
	OnInteractAttachDetach.Broadcast(InteractComponent);
}

void UIS_BeInteractComponent::NetMulti_OnInteractAttachDetach_Implementation(UIS_InteractComponent* InteractComponent)
{
	OnInteractAttachDetach.Broadcast(InteractComponent);
}

UAnimMontage* UIS_BeInteractComponent::GetMontageFromKeyName(FName KeyName)
{
	if (BeInteractInfo.InteractMontage.Contains(KeyName))
	{
		return BeInteractInfo.InteractMontage[KeyName].LoadSynchronous();
	}
	return nullptr;
}

float UIS_BeInteractComponent::GetInteractMontageSectionLengthFromIndex(UAnimMontage* Montage, int32 SectionIndex)
{
	if (Montage && Montage->IsValidSectionIndex(SectionIndex))
	{
		return Montage->GetSectionLength(SectionIndex);
	}
	return -1.0f;
}

float UIS_BeInteractComponent::GetInteractMontageSectionLengthFromName(UAnimMontage* Montage, FName SectionName)
{
	if (Montage && Montage->IsValidSectionName(SectionName))
	{
		return Montage->GetSectionLength(Montage->GetSectionIndex(SectionName));
	}
	return -1.0f;
}

void UIS_BeInteractComponent::InteractTraceCheck_Implementation(EIS_InteractTraceType TraceType, bool IsEnter)
{
	if (IsEnter)
	{
		if (!BeInteractDynamicInfo.AllEnterTraceType.Contains(TraceType))
		{
			BeInteractDynamicInfo.AllEnterTraceType.Add(TraceType);
		}
	}
	else
	{
		BeInteractDynamicInfo.AllEnterTraceType.Remove(TraceType);
	}
}

void UIS_BeInteractComponent::CreateVerifyUI_Implementation(UIS_InteractComponent* InteractComponent, TSubclassOf<UUserWidget> UIClass)
{
	IIS_BeInteractInterface::Execute_CreateVerifyObject_UI(this, InteractComponent, UIClass);
}

float UIS_BeInteractComponent::GetCurInteractTimeFromRoleSign(FName RoleSign)
{
	if (BeInteractInfo.bEveryoneCumulativeTime)//是否允许多人累加交互时长
	{
		return BeInteractDynamicInfo.InteractCumulativeTime;
	}
	return BeInteractDynamicInfo.GetInteractTimeFromRoleSign(RoleSign);
}

bool UIS_BeInteractComponent::TryInteractComplete(UIS_InteractComponent* InteractComponent)
{
	BeInteractDynamicInfo.AllTryCompleteInteractComponent.Add(InteractComponent);
	if (IIS_BeInteractInterface::Execute_InteractCompleteVerifyCheck(this, InteractComponent))//交互验证
	{
		IIS_BeInteractInterface::Execute_InteractComplete(this, InteractComponent);
		return true;
	}
	return false;
}

float UIS_BeInteractComponent::GetAngleFromTargetDir(FVector TargetDir)
{
	float BaseAngle = UKismetMathLibrary::DegAcos(UKismetMathLibrary::Vector_CosineAngle2D(TargetDir, GetOwner()->GetActorForwardVector()));
	FVector Cross = UKismetMathLibrary::Cross_VectorVector(TargetDir, GetOwner()->GetActorForwardVector());//判断是否越过了180度

	if (Cross.Z > 0.0f)
	{
		return 360.0f - BaseAngle;
	}
	else
	{
		return BaseAngle;
	}
}

void UIS_BeInteractComponent::InteractTimerBack()
{
	UIS_InteractComponent* InteractComponent;
	switch (BeInteractInfo.InteractType)
	{
	case EIS_InteractType::Instant://瞬间交互直接完成
	{
		InteractComponent = BeInteractDynamicInfo.AllInteractComponent.Last();
		TryInteractComplete(InteractComponent);
		break;
	}
	case EIS_InteractType::HasDuration://持续交互
	{
		if (BeInteractInfo.bEveryoneCumulativeTime)//如果允许多人统一累加时间，判断这次累加有没有导致完成
		{
			//累加时的time是循环调用的
			BeInteractDynamicInfo.InteractCumulativeTime += BeInteractInfo.CumulativeTimeInterval;
			if (BeInteractInfo.InteractTime.Num() > 1)//是否是多段
			{
				//是多段判断这次交互有没有超过某段时长
				if (BeInteractDynamicInfo.InteractCumulativeTime >= BeInteractInfo.InteractTime[BeInteractDynamicInfo.InteractCompleteCount])
				{
					//BeInteractDynamicInfo.InteractCumulativeTime = 0.0f;多段累计不归零，在Beginplay时会特殊处理
					InteractComponent = FindCompleteInteractComponent(BeInteractInfo.CumulativeTimeInterval);
					BeInteractDynamicInfo.InteractCompleteCount++;//增加累计成功次数
					IIS_BeInteractInterface::Execute_InteractComplete_MultiSegment(this, InteractComponent);
					if (BeInteractDynamicInfo.InteractCompleteCount >= BeInteractInfo.InteractTime.Num())//是否全部完成了
					{
						TryInteractComplete(InteractComponent);
					}
					//没有完成timer仍在不断调用（loop）中，会继续累计交互时长
				}
			}
			else//允许多人统一累加时间，不是多段交互
			{
				//不是多段直接判断这次累计有没有超过总时长BeInteractDynamicInfo.InteractTotalTime
				if (BeInteractDynamicInfo.InteractCumulativeTime >= BeInteractDynamicInfo.InteractTotalTime)
				{
					InteractComponent = FindCompleteInteractComponent(BeInteractInfo.CumulativeTimeInterval);
					TryInteractComplete(InteractComponent);
				}
			}
		}
		else//不允许多人同时累加交互时长
		{
			if (BeInteractInfo.InteractTime.Num() > 1)//是否是多段
			{
				if (BeInteractInfo.InteractCumulativeTimeType == EIS_InteractCumulativeTimeType::NotCumulative)//不累计,多段交互
				{
					//不累计的多段交互是一段一段的timer，每个timer的时长是对应下标的值
					InteractComponent = FindCompleteInteractComponent(BeInteractInfo.InteractTime[BeInteractDynamicInfo.InteractCompleteCount]);
					BeInteractDynamicInfo.RecordInteractInfo(InteractComponent->GetRoleSign(),0,0.0f,1);//增加该交互者的累计成功次数
					IIS_BeInteractInterface::Execute_InteractComplete_MultiSegment(this, InteractComponent);
					if (BeInteractDynamicInfo.GetInteractCompleteCountFromRoleSign(InteractComponent->GetRoleSign()) >= BeInteractInfo.InteractTime.Num())//是否全部完成了
					{
						TryInteractComplete(InteractComponent);
					}
					else
					{
						FTimerHandle TimeHandle;
						GetWorld()->GetTimerManager().SetTimer(TimeHandle, this, &UIS_BeInteractComponent::InteractTimerBack,
							BeInteractInfo.InteractTime[BeInteractDynamicInfo.InteractCompleteCount]);
						BeInteractOtherInfo.InteractTimerHandle.Add(InteractComponent->GetRoleSign(), TimeHandle);
					}
				}
				else//累计时长,多段交互
				{
					//累计时的time是循环调用的
					InteractComponent = FindCompleteInteractComponent(BeInteractInfo.CumulativeTimeInterval);
					//给对应的交互者增加交互时长
					BeInteractDynamicInfo.RecordInteractInfo(InteractComponent->GetRoleSign(), 0, BeInteractInfo.CumulativeTimeInterval, 0);
					if (BeInteractDynamicInfo.GetInteractTimeFromRoleSign(InteractComponent->GetRoleSign()) >= 
					BeInteractInfo.InteractTime[BeInteractDynamicInfo.GetInteractCompleteCountFromRoleSign(InteractComponent->GetRoleSign())])
					{
						BeInteractDynamicInfo.RecordInteractInfo(InteractComponent->GetRoleSign(), 0, 0.0f, 1);//增加该交互者的累计成功次数
						IIS_BeInteractInterface::Execute_InteractComplete_MultiSegment(this, InteractComponent);
						if (BeInteractDynamicInfo.GetInteractCompleteCountFromRoleSign(InteractComponent->GetRoleSign()) >= BeInteractInfo.InteractTime.Num())//是否全部完成了
						{
							TryInteractComplete(InteractComponent);
						}
					}
				}
			}
			else
			{
				if (BeInteractInfo.InteractCumulativeTimeType == EIS_InteractCumulativeTimeType::NotCumulative)//不累计,非多段交互
				{
					//不累计,非多段交互模式下的time调用时长 = 交互时长 且是非循环调用的
					InteractComponent = FindCompleteInteractComponent(BeInteractDynamicInfo.InteractTotalTime);
					TryInteractComplete(InteractComponent);
				}
				else//累计时长,非多段交互 按间隔累计和按分段累计在这里没有区别，区别是交互结束时对累计时长的处理
				{
					//累计时的time是循环调用的
					InteractComponent = FindCompleteInteractComponent(BeInteractInfo.CumulativeTimeInterval);
					//给对应的交互者增加交互时长
					BeInteractDynamicInfo.RecordInteractInfo(InteractComponent->GetRoleSign(), 0, BeInteractInfo.CumulativeTimeInterval, 0);
					//判断这个交互者有没有达到交互时长的需求
					if (BeInteractDynamicInfo.GetInteractTimeFromRoleSign(InteractComponent->GetRoleSign()) >= BeInteractDynamicInfo.InteractTotalTime)
					{
						TryInteractComplete(InteractComponent);
					}
				}
			}
		}
		break;
	}
	default:
		break;
	}
}

void UIS_BeInteractComponent::InteractRoleNumVerifyBack()
{
	if (BeInteractDynamicInfo.bVerifyInteractRoleNumPass)//如果外部还没完成交互，但此时人数已经通过由该函数触发完成
	{
		IIS_BeInteractInterface::Execute_InteractComplete(this, BeInteractDynamicInfo.AllInteractComponent.Last());//理应是因为最后一个人的交互导致的人数满足
	}

	if (BeInteractDynamicInfo.bIsInInteract)//在交互中才需要判断
	{
		if (BeInteractInfo.InteractCompleteVerifyInfo.bRoleNumVerifyIsNowOrHistory)//人数验证是交互中的人数还是历史交互人数
		{
			if (BeInteractDynamicInfo.AllTryCompleteInteractComponent.Num() >= BeInteractInfo.InteractCompleteVerifyInfo.CompleteInteractRoleNumVerify)
			{
				BeInteractDynamicInfo.bVerifyInteractRoleNumPass = true;
			}

		}
		else//历史交互人数
		{
			if (BeInteractDynamicInfo.GetHistoryInteractRoleNum() >= BeInteractInfo.InteractCompleteVerifyInfo.CompleteInteractRoleNumVerify)
			{
				BeInteractDynamicInfo.bVerifyInteractRoleNumPass = true;
			}
		}
	}
}

UIS_InteractComponent* UIS_BeInteractComponent::FindCompleteInteractComponent(float BackTime)
{
	for (TPair<FName, FTimerHandle>& pair : BeInteractOtherInfo.InteractTimerHandle)//哪个Handle完成了
	{
		float TimerHandleTime = UKismetSystemLibrary::K2_GetTimerElapsedTimeHandle(this, pair.Value);
		if (TimerHandleTime >= BackTime || TimerHandleTime <= 0.0f)//<0表示已经结束了
		{
			for (UIS_InteractComponent*& Com : BeInteractDynamicInfo.AllInteractComponent)//这个Handle属于哪个交互组件
			{
				if (Com->GetRoleSign() == pair.Key)
				{
					return Com;
				}
			}
		}
	}
	return nullptr;
}

bool UIS_BeInteractComponent::TraceTypeCheck(EIS_InteractTraceType TraceType, bool IsEnter)
{
	//允许该类型的检测
	if (BeInteractInfo.InteractCheckEnterCondition.Contains(TraceType))
	{
		//如果是进入 && 该类型没有触发过“进入”事件
		if (IsEnter && !BeInteractDynamicInfo.AllEnterTraceType.Contains(TraceType))
		{
			return true;
		}
		else if(!IsEnter && BeInteractDynamicInfo.AllEnterTraceType.Contains(TraceType))//如果是离开，该类型必须先触发过“进入”事件
		{
			return true;
		}
	}
	return false;
}

bool UIS_BeInteractComponent::CanInteract_Extend(UIS_InteractComponent* InteractComponent, FCC_CompareInfo OuterCompareInfo, FText& FailText)
{
	FailText = FText();
	for (UIS_BeInteractExtendBase*& BeInteractExtend : AllExtend)
	{
		if (!IIS_BeInteractInterface::Execute_CanInteract(BeInteractExtend, InteractComponent, OuterCompareInfo, FailText))
		{
			return false;
		}
	}
	return true;
}


