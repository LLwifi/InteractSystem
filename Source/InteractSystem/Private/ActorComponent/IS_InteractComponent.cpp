// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent/IS_InteractComponent.h"
#include "ActorComponent/IS_BeInteractComponent.h"
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetMathLibrary.h>
#include "Kismet/KismetSystemLibrary.h"


// Sets default values for this component's properties
UIS_InteractComponent::UIS_InteractComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
	// ...
}

#if WITH_EDITOR
void UIS_InteractComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	auto Property = PropertyChangedEvent.Property;//拿到改变的属性
	//if (Property->GetFName() == GET_MEMBER_NAME_CHECKED(UTS_TaskComponent, TaskRole))
	//{
	//	if (TaskRole == ETS_TaskRole::Player)
	//	{
	//		SetIsReplicated(true);
	//	}
	//	else
	//	{
	//		SetIsReplicated(false);
	//	}
	//}
}
#endif

// Called when the game starts
void UIS_InteractComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	if (GetOwner()->GetLocalRole() == ENetRole::ROLE_AutonomousProxy)
	{
		AllInteractTypeTagTime = AllInteractTypeTagMapping;
		InteractCheckStateChange(bBeginPlayIsActiveInteractCheck);
	}
}

void UIS_InteractComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UIS_InteractComponent, PreBeInteractComponent);
	DOREPLIFETIME(UIS_InteractComponent, CurStartInteractComponent);
}

// Called every frame
void UIS_InteractComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

FName UIS_InteractComponent::GetRoleSign()
{
	if (RoleSign.IsNone())
	{
		return FName(UKismetSystemLibrary::GetDisplayName(this));
	}
	return RoleSign;
}

bool UIS_InteractComponent::StartInteract(UIS_BeInteractComponent* BeInteractComponent, FGameplayTag TraceTypeTag)
{
	if (BeInteractComponent)
	{
		if (CurStartInteractComponent)//当前是否有已经在交互的另一个资源
		{
			EndCurInteract(TraceTypeTag);
		}
		CurStartInteractComponent = BeInteractComponent;
		IIS_BeInteractInterface::Execute_InteractStart(BeInteractComponent, this, TraceTypeTag);//通知目标开始与他交互了
	}
	
	return true;
}

void UIS_InteractComponent::EndCurInteract(FGameplayTag TraceTypeTag)
{
	//有些组件在交互完后被立刻删除了，可能会导致来不及触发结束
	if (CurStartInteractComponent)
	{
		IIS_BeInteractInterface::Execute_InteractEnd(CurStartInteractComponent, this, TraceTypeTag);
		CurStartInteractComponent = nullptr;
	}
}

bool UIS_InteractComponent::InteractVerify(UIS_BeInteractComponent* BeInteractComponent, FGameplayTag TraceType, FText& FailText)
{
	bool IsPass = false;
	//被交互组件有效 && 允许被该类型检测到 && 没有被Tag忽略
	if (BeInteractComponent && BeInteractComponent->BeInteractInfo.InteractTypeVerifyInfo.Contains(TraceType) && !BeInteractComponent->BeInteractInfo.InteractTag.HasAnyExact(InteractIgnoreTag))
	{
		IsPass = true;
		FIS_InteractVerifyInfo VerifyInfo = BeInteractComponent->BeInteractInfo.InteractTypeVerifyInfo[TraceType];
		if (VerifyInfo.bIsActiveTraceCheck)//是否只有被交互组件激活后才能检测
		{
			IsPass = IIS_BeInteractInterface::Execute_IsInteractActive(BeInteractComponent);
		}
		if (IsPass && BeInteractComponent->BeInteractInfo.InteractTypeVerifyInfo[TraceType].bOverride_BeCompareInfoVerify)//比对
		{
			IsPass = BeInteractComponent->BeInteractInfo.InteractTypeVerifyInfo[TraceType].BeCompareInfo.CompareResult(DefaultCompareInfo, FailText);
		}
		if (IsPass)//验证
		{
			IsPass = BeInteractComponent->BeInteractInfo.InteractTypeVerifyInfo[TraceType].Verify(this, BeInteractComponent, FailText);
		}
	}
	return IsPass;
}

TArray<UIS_BeInteractComponent*> UIS_InteractComponent::TraceOutHitVerify(FGameplayTag TraceType, const TArray<FHitResult>& OutHit, UIS_BeInteractComponent*& TopPriorityCom, FText& FailText)
{
	TopPriorityCom = nullptr;
	TArray<UIS_BeInteractComponent*> AllBeInteract;//全部可被交互组件
	for (const FHitResult& HitResult : OutHit)
	{
		UIS_BeInteractComponent* BeInteractComponent = Cast<UIS_BeInteractComponent>(HitResult.GetComponent());
		if (InteractVerify(BeInteractComponent, TraceType, FailText))
		{
			AllBeInteract.Add(BeInteractComponent);
			if (!TopPriorityCom || //为空直接设置
				IIS_BeInteractInterface::Execute_GetInteractPriority(BeInteractComponent) > IIS_BeInteractInterface::Execute_GetInteractPriority(TopPriorityCom))
			{
				TopPriorityCom = BeInteractComponent;
			}
		}
	}
	return AllBeInteract;
}

TArray<UIS_BeInteractComponent*> UIS_InteractComponent::GetCurInteractComponentFromTypeTag(FGameplayTag TraceTypeTag, UIS_BeInteractComponent*& TopPriorityCom)
{
	TArray<UIS_BeInteractComponent*> AllBeInteract;//全部可被交互组件
	TopPriorityCom = nullptr;
	for (TPair<FGameplayTag, FIS_BeInteractComponentArray>& pair : AllTraceCheckComponent)
	{
		for (UIS_BeInteractComponent*& BeInteractCom : pair.Value.ComponentArray)
		{
			if (BeInteractCom)
			{
				//TraceTypeTag为None时获取当前全部的可被交互组件
				if (!TraceTypeTag.IsValid() || BeInteractCom->BeInteractDynamicInfo.AllEnterTraceTypeTag.Contains(TraceTypeTag))
				{
					AllBeInteract.Add(BeInteractCom);
					if (!TopPriorityCom || //为空直接设置
						IIS_BeInteractInterface::Execute_GetInteractPriority(BeInteractCom) > IIS_BeInteractInterface::Execute_GetInteractPriority(TopPriorityCom))
					{
						TopPriorityCom = BeInteractCom;
					}
				}
			}
		}
	}
	return AllBeInteract;
}

bool UIS_InteractComponent::TriggerInteract(UPARAM(Ref)UIS_BeInteractComponent*& BeInteractCom, FCC_CompareInfo CompareInfo, FGameplayTag TraceTypeTag, FText& FailText)
{
	bool ReturnBool = false;
	if (BeInteractCom && IIS_BeInteractInterface::Execute_CanInteract(BeInteractCom, this, CompareInfo, FailText))
	{
		StartInteract(BeInteractCom, TraceTypeTag);
		UpdateInteractTarget(BeInteractCom, TraceTypeTag);
		ReturnBool = true;
	}
	return ReturnBool;
}

void UIS_InteractComponent::InteractCheckStateChange(bool IsActive)
{
	if (IsActive)
	{
		GetWorld()->GetTimerManager().SetTimer(InteractCheckTimeHandle, this, &UIS_InteractComponent::InteractEnterCheck, InteractCheckInterval, true);
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(InteractCheckTimeHandle);

		TArray<UIS_BeInteractComponent*> AllBeInteract;
		TArray<FGameplayTag> AllEnterTypeTag;
		for (TPair<FGameplayTag, FIS_BeInteractComponentArray>& pair : AllTraceCheckComponent)
		{
			for (UIS_BeInteractComponent*& BeInteractCom : pair.Value.ComponentArray)
			{
				ServerLeaveInteractCheck(BeInteractCom, pair.Key);
				IIS_BeInteractInterface::Execute_InteractLeave(BeInteractCom, this, pair.Key);//客户端仍然需要移除该检测类型
			}
		}
		UpdateInteractTarget(nullptr,FGameplayTag());//广播交互事件
		AllTraceCheckComponent.Empty();
		CurStartInteractComponent = nullptr;
	}
}

void UIS_InteractComponent::InteractEnterCheck()
{
	UIS_BeInteractComponent* TopCom = nullptr;
	for (TPair<FGameplayTag, float>& pair : AllInteractTypeTagMapping)
	{
		if (AllInteractTypeTagTime.Contains(pair.Key))
		{
			AllInteractTypeTagTime[pair.Key] += InteractCheckInterval;
		}
		else
		{
			AllInteractTypeTagTime.Add(pair.Key, InteractCheckInterval);
		}

		if (AllInteractTypeTagTime[pair.Key] >= pair.Value)
		{
			AllTraceCheckComponent.Add(pair.Key, FIS_BeInteractComponentArray(InteractEnterCheckFromTraceType(pair.Key, TopCom)));
			AllInteractTypeTagTime[pair.Key] = 0.0f;
		}
	}
}

FVector UIS_InteractComponent::GetLocationFromTraceInfo(const FIS_TraceInfo& TraceInfo, bool IsStartLocation)
{
	FVector ReturnVector;
	EIS_TraceLocationType LocationType = TraceInfo.EndLocationType;
	FVector Location = TraceInfo.End;
	float Angle = TraceInfo.EndAngle;
	float Distance = TraceInfo.EndDistance;
	if (IsStartLocation)
	{
		LocationType = TraceInfo.StartLocationType;
		Location = TraceInfo.Start;
		Angle = TraceInfo.StartAngle;
		Distance = TraceInfo.StartDistance;
	}

	switch (LocationType)
	{
	case EIS_TraceLocationType::OwnerLocation:
	{
		ReturnVector = GetOwner()->GetActorLocation() + Location;
		break;
	}
	case EIS_TraceLocationType::OwnerAngle:
	{
		FRotator AngleRotator = FRotator(0.0f, Angle, 0.0f);
		ReturnVector = GetOwner()->GetActorLocation() + AngleRotator.RotateVector(GetOwner()->GetActorForwardVector()) * Distance + Location;
		break;
	}
	case EIS_TraceLocationType::CameraLocation:
	{
		APawn* Pawn = Cast<APawn>(GetOwner());
		if (Pawn && Pawn->GetController())
		{
			FRotator CameraRotation;
			Pawn->GetController()->GetPlayerViewPoint(ReturnVector, CameraRotation);
			ReturnVector += Location;
		}
		break;
	}
	case EIS_TraceLocationType::CameraAngle:
	{
		APawn* Pawn = Cast<APawn>(GetOwner());
		if (Pawn && Pawn->GetController())
		{
			FVector CameraLocation;
			FRotator CameraRotation;
			Pawn->GetController()->GetPlayerViewPoint(CameraLocation, CameraRotation);
			FRotator AngleRotator = FRotator(0.0f, Angle, 0.0f);
			ReturnVector = CameraLocation + AngleRotator.RotateVector(UKismetMathLibrary::GetForwardVector(CameraRotation)) * Distance + Location;
		}
		break;
	}
	case EIS_TraceLocationType::WorldLocation:
	{
		ReturnVector = Location;
		break;
	}
	case EIS_TraceLocationType::DynamicGet:
	{
		ReturnVector = GetLocationFromTraceType(TraceInfo.InteractTypeTag, IsStartLocation) + Location;
		break;
	}
	default:
		break;
	}

	return ReturnVector;
}


FVector UIS_InteractComponent::GetLocationFromTraceType_Implementation(FGameplayTag TraceType, bool IsStartLocation)
{
	return FVector();
}

bool UIS_InteractComponent::TraceFromTypeInfo(FIS_TraceInfo TraceInfo, TArray<FHitResult>& OutHit)
{
	FText FailText;
	FVector Start = GetLocationFromTraceInfo(TraceInfo, true);
	FVector End = GetLocationFromTraceInfo(TraceInfo, false);

	switch (TraceInfo.InteractTraceType)
	{
	case EIS_TraceType::Line:
	{
		switch (TraceInfo.BeTraceType)
		{
		case EIS_BeTraceType::Channel:
		{
			UKismetSystemLibrary::LineTraceMulti(this, Start, End, TraceInfo.TraceChannel, TraceInfo.bTraceComplex, TraceInfo.ActorsToIgnore, TraceInfo.DrawDebugType, OutHit, TraceInfo.bIgnoreSelf, TraceInfo.TraceColor, TraceInfo.TraceHitColor, TraceInfo.DrawTime);
			break;
		}
		case EIS_BeTraceType::Objects:
		{
			UKismetSystemLibrary::LineTraceMultiForObjects(this, Start, End, TraceInfo.ObjectTypes, TraceInfo.bTraceComplex, TraceInfo.ActorsToIgnore, TraceInfo.DrawDebugType, OutHit, TraceInfo.bIgnoreSelf, TraceInfo.TraceColor, TraceInfo.TraceHitColor);
			break;
		}
		case EIS_BeTraceType::Profile:
		{
			UKismetSystemLibrary::LineTraceMultiByProfile(this, Start, End, TraceInfo.ProfileName, TraceInfo.bTraceComplex, TraceInfo.ActorsToIgnore, TraceInfo.DrawDebugType, OutHit, TraceInfo.bIgnoreSelf, TraceInfo.TraceColor, TraceInfo.TraceHitColor);
			break;
		}
		default:
			break;
		}
		break;
	}
	case EIS_TraceType::Sphere:
	{
		switch (TraceInfo.BeTraceType)
		{
		case EIS_BeTraceType::Channel:
		{
			UKismetSystemLibrary::SphereTraceMulti(this, Start, End, TraceInfo.Radius, TraceInfo.TraceChannel, TraceInfo.bTraceComplex, TraceInfo.ActorsToIgnore, TraceInfo.DrawDebugType, OutHit, TraceInfo.bIgnoreSelf, TraceInfo.TraceColor, TraceInfo.TraceHitColor, TraceInfo.DrawTime);
			break;
		}
		case EIS_BeTraceType::Objects:
		{
			UKismetSystemLibrary::SphereTraceMultiForObjects(this, Start, End, TraceInfo.Radius, TraceInfo.ObjectTypes, TraceInfo.bTraceComplex, TraceInfo.ActorsToIgnore, TraceInfo.DrawDebugType, OutHit, TraceInfo.bIgnoreSelf, TraceInfo.TraceColor, TraceInfo.TraceHitColor);
			break;
		}
		case EIS_BeTraceType::Profile:
		{
			UKismetSystemLibrary::SphereTraceMultiByProfile(this, Start, End, TraceInfo.Radius, TraceInfo.ProfileName, TraceInfo.bTraceComplex, TraceInfo.ActorsToIgnore, TraceInfo.DrawDebugType, OutHit, TraceInfo.bIgnoreSelf, TraceInfo.TraceColor, TraceInfo.TraceHitColor);
			break;
		}
		default:
			break;
		}
		break;
	}
	case EIS_TraceType::Box:
	{
		switch (TraceInfo.BeTraceType)
		{
		case EIS_BeTraceType::Channel:
		{
			UKismetSystemLibrary::BoxTraceMulti(this, Start, End, TraceInfo.HalfSize, TraceInfo.Orientation, TraceInfo.TraceChannel, TraceInfo.bTraceComplex, TraceInfo.ActorsToIgnore, TraceInfo.DrawDebugType, OutHit, TraceInfo.bIgnoreSelf, TraceInfo.TraceColor, TraceInfo.TraceHitColor, TraceInfo.DrawTime);
			break;
		}
		case EIS_BeTraceType::Objects:
		{
			UKismetSystemLibrary::BoxTraceMultiForObjects(this, Start, End, TraceInfo.HalfSize, TraceInfo.Orientation, TraceInfo.ObjectTypes, TraceInfo.bTraceComplex, TraceInfo.ActorsToIgnore, TraceInfo.DrawDebugType, OutHit, TraceInfo.bIgnoreSelf, TraceInfo.TraceColor, TraceInfo.TraceHitColor);
			break;
		}
		case EIS_BeTraceType::Profile:
		{
			UKismetSystemLibrary::BoxTraceMultiByProfile(this, Start, End, TraceInfo.HalfSize, TraceInfo.Orientation, TraceInfo.ProfileName, TraceInfo.bTraceComplex, TraceInfo.ActorsToIgnore, TraceInfo.DrawDebugType, OutHit, TraceInfo.bIgnoreSelf, TraceInfo.TraceColor, TraceInfo.TraceHitColor);
			break;
		}
		default:
			break;
		}
		break;
	}
	case EIS_TraceType::Capsule:
	{
		switch (TraceInfo.BeTraceType)
		{
		case EIS_BeTraceType::Channel:
		{
			UKismetSystemLibrary::CapsuleTraceMulti(this, Start, End, TraceInfo.Radius, TraceInfo.HalfHeight, TraceInfo.TraceChannel, TraceInfo.bTraceComplex, TraceInfo.ActorsToIgnore, TraceInfo.DrawDebugType, OutHit, TraceInfo.bIgnoreSelf, TraceInfo.TraceColor, TraceInfo.TraceHitColor, TraceInfo.DrawTime);
			break;
		}
		case EIS_BeTraceType::Objects:
		{
			UKismetSystemLibrary::CapsuleTraceMultiForObjects(this, Start, End, TraceInfo.Radius, TraceInfo.HalfHeight, TraceInfo.ObjectTypes, TraceInfo.bTraceComplex, TraceInfo.ActorsToIgnore, TraceInfo.DrawDebugType, OutHit, TraceInfo.bIgnoreSelf, TraceInfo.TraceColor, TraceInfo.TraceHitColor);
			break;
		}
		case EIS_BeTraceType::Profile:
		{
			UKismetSystemLibrary::CapsuleTraceMultiByProfile(this, Start, End, TraceInfo.Radius, TraceInfo.HalfHeight, TraceInfo.ProfileName, TraceInfo.bTraceComplex, TraceInfo.ActorsToIgnore, TraceInfo.DrawDebugType, OutHit, TraceInfo.bIgnoreSelf, TraceInfo.TraceColor, TraceInfo.TraceHitColor);
			break;
		}
		default:
			break;
		}
		break;
	}
	default:
		break;
	}

	return OutHit.Num() > 0;
}

TArray<UIS_BeInteractComponent*> UIS_InteractComponent::TraceCheckFromTypeInfo(FIS_InteractTypeInfo InteractTraceInfo, UIS_BeInteractComponent*& TopPriorityCom)
{
	FText FailText;
	TArray<FHitResult> OutHit;
	TArray<UIS_BeInteractComponent*> ReturnValue;
	if (TraceFromTypeInfo(InteractTraceInfo.InteractTraceInfo, OutHit))//是否有命中原生的对象
	{
		ReturnValue = TraceOutHitVerify(InteractTraceInfo.InteractTypeTag, OutHit, TopPriorityCom, FailText);//获取其中的可被交互对象
		if (ReturnValue.Num() > 0 && InteractTraceInfo.bIsBlockCheck)//如果有，根据配置判断是否要检测阻挡
		{
			TraceFromTypeInfo(InteractTraceInfo.BlockTraceInfo, OutHit);
			for (const FHitResult& HitResult : OutHit)
			{
				UIS_BeInteractComponent* BeInteractComponent = Cast<UIS_BeInteractComponent>(HitResult.GetComponent());

				if (BeInteractComponent)//在前面的阻挡是否是个交互碰撞
				{
					if (BeInteractComponent == TopPriorityCom)//如果先找到了TopPriorityCom，那么它们之间无阻挡
					{
						return ReturnValue;//它们之间无阻挡
					}
				}
				else
				{
					TopPriorityCom = nullptr;
					ReturnValue.Empty();
					return ReturnValue;//被什么阻挡了
				}
			}
		}
	}

	return ReturnValue;
}

TArray<UIS_BeInteractComponent*> UIS_InteractComponent::TraceCheckFromTypeTag(FGameplayTag TraceTypeTag, UIS_BeInteractComponent*& TopPriorityCom)
{
	TArray<UIS_BeInteractComponent*> AllBeInteractCom;
	if (UIS_Config::GetInstance()->InteractTraceTypeMapping.Contains(TraceTypeTag))
	{
		AllBeInteractCom = TraceCheckFromTypeInfo(UIS_Config::GetInstance()->InteractTraceTypeMapping[TraceTypeTag], TopPriorityCom);
	}
	return AllBeInteractCom;
}

TArray<UIS_BeInteractComponent*> UIS_InteractComponent::InteractEnterCheckFromTraceType_Implementation(FGameplayTag TraceTypeTag, UIS_BeInteractComponent*& TopPriorityCom)
{
	TArray<UIS_BeInteractComponent*> AllBeInteractCom = TraceCheckFromTypeTag(TraceTypeTag, TopPriorityCom);

	//离开事件判断
	if (AllTraceCheckComponent.Contains(TraceTypeTag))
	{
		for (UIS_BeInteractComponent*& BeInteractCom : AllTraceCheckComponent[TraceTypeTag].ComponentArray)
		{
			//BeInteractCom有效（拾取道具会在交互后删除目标） && 曾经进入的交互组件不在新一批的交互组件中 && 检测类型是否通过
			if (BeInteractCom && !AllBeInteractCom.Contains(BeInteractCom) && BeInteractCom->TraceTypeCheck(TraceTypeTag, false))
			{
				//不在，触发离开事件 离开需要触发交互结束，因此离开需要在服务器上被调用
				ServerLeaveInteractCheck(BeInteractCom, TraceTypeTag);
				IIS_BeInteractInterface::Execute_InteractLeave(BeInteractCom, this, TraceTypeTag);//客户端仍然需要移除该检测类型
				//UpdateInteractTarget(nullptr, TraceTypeTag);//广播交互事件
			}
		}
	}

	//进入事件判断
	for (UIS_BeInteractComponent*& BeInteractCom : AllBeInteractCom)
	{
		if (BeInteractCom && BeInteractCom->TraceTypeCheck(TraceTypeTag))
		{
			switch (BeInteractCom->BeInteractInfo.InteractTypeVerifyInfo[TraceTypeTag].InteractCheckType)//不在，尝试触发进入事件
			{
			case EIS_BeInteractCheckType::AnyTrigger:
			{
				IIS_BeInteractInterface::Execute_InteractEnter(BeInteractCom, this, TraceTypeTag);//移入新交互目标
				UpdateInteractTarget(BeInteractCom, TraceTypeTag);//广播交互事件
				break;
			}
			case EIS_BeInteractCheckType::TopPriorityTrigger:
			{
				if (TopPriorityCom == BeInteractCom)
				{
					IIS_BeInteractInterface::Execute_InteractEnter(BeInteractCom, this, TraceTypeTag);//移入新交互目标
					UpdateInteractTarget(BeInteractCom, TraceTypeTag);//广播交互事件
				}
				break;
			}
			default:
				break;
			}
		}
	}
	return AllBeInteractCom;
}

void UIS_InteractComponent::AddInteractIgnoreTag(FGameplayTagContainer TagContainer)
{
	InteractIgnoreTag.AppendTags(TagContainer);
}

void UIS_InteractComponent::RemoveInteractIgnoreTag(FGameplayTagContainer TagContainer)
{
	InteractIgnoreTag.RemoveTags(TagContainer);
}

void UIS_InteractComponent::ServerSetPreBeInteractComponent_Implementation(UIS_BeInteractComponent* BeInteractCom)
{
	PreBeInteractComponent = BeInteractCom;
}

void UIS_InteractComponent::UpdateInteractTarget_Implementation(UIS_BeInteractComponent* BeInteractComponent, FGameplayTag TraceTypeTag)
{
	UpdateInteractEvent.Broadcast(this, BeInteractComponent, TraceTypeTag);
}

void UIS_InteractComponent::ServerVerifyCurInteractComplete_Implementation(FGameplayTag TraceTypeTag)
{
	if (CurStartInteractComponent)
	{
		IIS_BeInteractInterface::Execute_InteractComplete(CurStartInteractComponent, this, TraceTypeTag);
	}
}

void UIS_InteractComponent::ServerVerifyCurInteractEnd_Implementation(FGameplayTag TraceTypeTag)
{
	if (CurStartInteractComponent)
	{
		CurStartInteractComponent->BeInteractDynamicInfo.bIsVerifying = false;
		IIS_BeInteractInterface::Execute_InteractEnd(CurStartInteractComponent, this, TraceTypeTag);
	}
}

void UIS_InteractComponent::ServerLeaveInteractCheck_Implementation(UIS_BeInteractComponent* BeInteractComponent, FGameplayTag InteractTraceTypeTag)
{
	if (BeInteractComponent)
	{
		IIS_BeInteractInterface::Execute_InteractLeave(BeInteractComponent, this, InteractTraceTypeTag);//移出旧交互目标
	}
}

void UIS_InteractComponent::SetInteractTypeTagMapping(TMap<FGameplayTag, float> Mapping)
{
	for (TPair<FGameplayTag, float>& pair : AllInteractTypeTagMapping)
	{
		RemoveInteractTypeTag(pair.Key);
	}
	AllInteractTypeTagMapping = Mapping;
}

void UIS_InteractComponent::AppendInteractTypeTagMapping(TMap<FGameplayTag, float> Mapping)
{
	AllInteractTypeTagMapping.Append(Mapping);
}

void UIS_InteractComponent::RemoveInteractTypeTag(FGameplayTag TypeTag)
{
	AllInteractTypeTagMapping.Remove(TypeTag);
	if (AllTraceCheckComponent.Contains(TypeTag))
	{
		for (UIS_BeInteractComponent*& BeInteractCom : AllTraceCheckComponent[TypeTag].ComponentArray)
		{
			ServerLeaveInteractCheck(BeInteractCom, TypeTag);
			IIS_BeInteractInterface::Execute_InteractLeave(BeInteractCom, this, TypeTag);//客户端仍然需要移除该检测类型
		}
	}
}
