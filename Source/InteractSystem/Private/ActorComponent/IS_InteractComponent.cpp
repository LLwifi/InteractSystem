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
	//if (UKismetSystemLibrary::IsServer(this))
	//{
	//	InteractCheckStateChange(bBeginPlayIsActiveInteractCheck);
	//}

	if (GetOwner()->GetLocalRole() == ENetRole::ROLE_AutonomousProxy)
	{
		InteractCheckStateChange(bBeginPlayIsActiveInteractCheck);
	}
}

void UIS_InteractComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

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

bool UIS_InteractComponent::StartInteract(UIS_BeInteractComponent* BeInteractComponent)
{
	if (BeInteractComponent)
	{
		if (CurStartInteractComponent)//当前是否有已经在交互的另一个资源
		{
			EndCurInteract();
		}
		CurStartInteractComponent = BeInteractComponent;
		IIS_BeInteractInterface::Execute_InteractStart(BeInteractComponent, this);//通知目标开始与他交互了
	}
	
	return true;
}

void UIS_InteractComponent::EndCurInteract()
{
	//有些组件在交互完后被立刻删除了，可能会导致来不及触发结束
	if (CurStartInteractComponent)
	{
		IIS_BeInteractInterface::Execute_InteractEnd(CurStartInteractComponent, this);
		CurStartInteractComponent = nullptr;
	}
}

bool UIS_InteractComponent::InteractConditionCheck(UIS_BeInteractComponent* BeInteractComponent, EIS_InteractTraceType InteractTraceType, FText& FailText)
{
	bool IsCheck = false;
	//被交互组件有效 && 允许被该类型检测到 && 没有被Tag忽略
	if (BeInteractComponent && BeInteractComponent->BeInteractInfo.InteractCheckEnterCondition.Contains(InteractTraceType) && !BeInteractComponent->BeInteractInfo.InteractTag.HasAnyExact(InteractIgnoreTag))
	{
		IsCheck = true;
		FIS_BeInteractCheckCondition Condition = BeInteractComponent->BeInteractInfo.InteractCheckEnterCondition[InteractTraceType];
		if (Condition.bIsActiveTraceCheck)
		{
			IsCheck = IIS_BeInteractInterface::Execute_IsInteractActive(BeInteractComponent);
		}
		if (IsCheck && Condition.bIsVerifyTraceCheck)//是否开启了验证
		{
			if (IsCheck && Condition.bIsDistanceVerifyTraceCheck && BeInteractComponent->BeInteractInfo.InteractVerifyInfo.bOverride_InteractDistanceVerify)//是否有距离验证，且被交互目标配置了距离验证
			{
				IsCheck = BeInteractComponent->BeInteractInfo.InteractVerifyInfo.DistanceVerify(this, BeInteractComponent, FailText);
			}
			if (IsCheck && Condition.bIsBeInteractAngleVerifyTraceCheck && BeInteractComponent->BeInteractInfo.InteractVerifyInfo.bOverride_BeInteractAngleVerify)//是否有被交互目标的角度验证，且被交互目标配置了角度验证
			{
				IsCheck = BeInteractComponent->BeInteractInfo.InteractVerifyInfo.BeInteractAngleVerifyCheck(this, BeInteractComponent, FailText);
			}
			if (IsCheck && Condition.bIsInteractorsAngleVerifyTraceCheck && BeInteractComponent->BeInteractInfo.InteractVerifyInfo.bOverride_InteractorsAngleVerify)//是否有交互者的角度验证，且被交互目标配置了角度验证
			{
				IsCheck = BeInteractComponent->BeInteractInfo.InteractVerifyInfo.InteractorsAngleVerifyCheck(this, BeInteractComponent, FailText);
			}
		}
		if (IsCheck && Condition.bIsCompareTraceCheck)
		{
			IsCheck = BeInteractComponent->BeInteractInfo.BeCompareInfo.CompareResult(DefaultCompareInfo, FailText);
		}
	}
	return IsCheck;
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
		if (IsPass && VerifyInfo.bIsCompareTraceCheck)//比对
		{
			IsPass = BeInteractComponent->BeInteractInfo.BeCompareInfo.CompareResult(DefaultCompareInfo, FailText);
		}
		if (IsPass)//验证
		{
			IsPass = BeInteractComponent->BeInteractInfo.InteractTypeVerifyInfo[TraceType].Verify(this, BeInteractComponent, FailText);
		}
	}
	return IsPass;
}

TArray<UIS_BeInteractComponent*> UIS_InteractComponent::TraceOutHitCheck(EIS_InteractTraceType InteractTraceType, const TArray<FHitResult>& OutHit, UIS_BeInteractComponent*& TopPriorityCom, FText& FailText)
{
	TArray<UIS_BeInteractComponent*> AllBeInteract;//全部可被交互组件
	for (const FHitResult& HitResult : OutHit)
	{
		UIS_BeInteractComponent* BeInteractComponent = Cast<UIS_BeInteractComponent>(HitResult.GetComponent());
		if (InteractConditionCheck(BeInteractComponent, InteractTraceType, FailText))
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

TArray<UIS_BeInteractComponent*> UIS_InteractComponent::TraceOutHitVerify(FGameplayTag TraceType, const TArray<FHitResult>& OutHit, UIS_BeInteractComponent*& TopPriorityCom, FText& FailText)
{
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

TArray<UIS_BeInteractComponent*> UIS_InteractComponent::GetInteractCheckComponentFromType(EIS_InteractTraceType InteractTraceType, UIS_BeInteractComponent*& TopPriorityCom)
{
	TArray<UIS_BeInteractComponent*> AllBeInteract;//全部可被交互组件
	TopPriorityCom = nullptr;
	for (FIS_TraceCheckComponentInfo& TraceCheckComponentInfo : AllTraceCheckComponent)
	{
		if (TraceCheckComponentInfo.BeInteractComponent)
		{
			if (InteractTraceType == EIS_InteractTraceType::None || TraceCheckComponentInfo.BeInteractComponent->BeInteractDynamicInfo.AllEnterTraceType.Contains(InteractTraceType))
			{
				AllBeInteract.Add(TraceCheckComponentInfo.BeInteractComponent);
				if (!TopPriorityCom || //为空直接设置
					IIS_BeInteractInterface::Execute_GetInteractPriority(TraceCheckComponentInfo.BeInteractComponent) > IIS_BeInteractInterface::Execute_GetInteractPriority(TopPriorityCom))
				{
					TopPriorityCom = TraceCheckComponentInfo.BeInteractComponent;
				}
			}
		}
	}
	return AllBeInteract;
}

bool UIS_InteractComponent::TriggerInteract(UPARAM(Ref)UIS_BeInteractComponent*& BeInteractCom, FCC_CompareInfo CompareInfo, EIS_InteractTraceType TraceType, FText& FailText)
{
	bool ReturnBool = false;
	if (BeInteractCom && IIS_BeInteractInterface::Execute_CanInteract(BeInteractCom, this, CompareInfo, FailText))
	{
		StartInteract(BeInteractCom);
		UpdateInteractTarget(BeInteractCom, TraceType);
		ReturnBool = true;
	}
	return ReturnBool;
}

TArray<UIS_BeInteractComponent*> UIS_InteractComponent::CameraTraceGetBeInteractCom(FIS_InteractRayInfo InteractRayInfo, UIS_BeInteractComponent*& TopPriorityCom, FText& FailText)
{
	TArray<UIS_BeInteractComponent*> AllBeInteract;//全部可被交互组件
	TopPriorityCom = nullptr;
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (Pawn && Pawn->GetController())
	{
		FVector CameraLocation, TraceEndLocation;
		FRotator CameraRotation;
		Pawn->GetController()->GetPlayerViewPoint(CameraLocation, CameraRotation);
		TraceEndLocation = (UKismetMathLibrary::GetForwardVector(CameraRotation) * TraceDistance) + CameraLocation;

		//如果考虑模糊处理，这里可以使用圆形的射线
		TArray<FHitResult> OutHit;
		UKismetSystemLibrary::LineTraceMulti(this, CameraLocation, TraceEndLocation, TraceChannel, InteractRayInfo.bTraceComplex, InteractRayInfo.ActorsToIgnore, DrawDebugType, OutHit, InteractRayInfo.bIgnoreSelf, TraceColor, TraceHitColor, DrawTime);
		AllBeInteract = TraceOutHitCheck(EIS_InteractTraceType::CameraTrace, OutHit, TopPriorityCom, FailText);
	}
	return AllBeInteract;
}

bool UIS_InteractComponent::TryTriggerInteract_CameraTrace(FCC_CompareInfo CompareInfo, FIS_InteractRayInfo InteractRayInfo, UIS_BeInteractComponent*& TopPriorityCom, TArray<UIS_BeInteractComponent*>& AllBeInteract, FText& FailText)
{
	AllBeInteract = CameraTraceGetBeInteractCom(InteractRayInfo, TopPriorityCom, FailText);
	return TriggerInteract(TopPriorityCom, CompareInfo, EIS_InteractTraceType::CameraTrace, FailText);
}

TArray<UIS_BeInteractComponent*> UIS_InteractComponent::SphereTraceGetBeInteractCom(FVector Start, FVector End, float Radius, FIS_InteractRayInfo InteractRayInfo, UIS_BeInteractComponent*& TopPriorityCom, FText& FailText)
{
	TArray<UIS_BeInteractComponent*> AllBeInteract;//全部可被交互组件
	TopPriorityCom = nullptr;
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (Pawn && Pawn->GetController())
	{
		FVector CameraLocation, TraceEndLocation;
		FRotator CameraRotation;
		Pawn->GetController()->GetPlayerViewPoint(CameraLocation, CameraRotation);
		TraceEndLocation = (UKismetMathLibrary::GetForwardVector(CameraRotation) * TraceDistance) + CameraLocation;

		//如果考虑模糊处理，这里可以使用圆形的射线
		TArray<FHitResult> OutHit;
		UKismetSystemLibrary::SphereTraceMulti(this, Start, End, Radius, TraceChannel, InteractRayInfo.bTraceComplex, InteractRayInfo.ActorsToIgnore, DrawDebugType, OutHit, InteractRayInfo.bIgnoreSelf, TraceColor, TraceHitColor, DrawTime);
		AllBeInteract = TraceOutHitCheck(EIS_InteractTraceType::SphereTrace, OutHit, TopPriorityCom, FailText);
	}
	return AllBeInteract;
}

bool UIS_InteractComponent::TryTriggerInteract_SphereTrace(FCC_CompareInfo CompareInfo, FIS_InteractRayInfo InteractRayInfo, FVector Start, FVector End, float Radius, UIS_BeInteractComponent*& TopPriorityCom, TArray<UIS_BeInteractComponent*>& AllBeInteract, FText& FailText)
{
	AllBeInteract = SphereTraceGetBeInteractCom(Start, End, Radius, InteractRayInfo, TopPriorityCom, FailText);
	return TriggerInteract(TopPriorityCom, CompareInfo, EIS_InteractTraceType::SphereTrace, FailText);
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
		//移除/清空当前移入的交互目标
		for (FIS_TraceCheckComponentInfo& TraceCheckComponentInfo : AllTraceCheckComponent)
		{
			ServerLeaveInteractCheck(TraceCheckComponentInfo.BeInteractComponent, TraceCheckComponentInfo.EnterTraceType);
			IIS_BeInteractInterface::Execute_InteractLeave(TraceCheckComponentInfo.BeInteractComponent, this, TraceCheckComponentInfo.EnterTraceType);//客户端仍然需要移除该检测类型
		}
		UpdateInteractTarget(nullptr, EIS_InteractTraceType::None);//广播交互事件
		AllTraceCheckComponent.Empty();
		CurStartInteractComponent = nullptr;
	}
}

void UIS_InteractComponent::InteractEnterCheck()
{
	TArray<FIS_TraceCheckComponentInfo> TraceCheckComponents;
	for (EIS_InteractTraceType& TraceType : InteractCheckTypes)
	{
		for (UIS_BeInteractComponent*& BeInteractCom : InteractCheckFromEnterType(TraceType))
		{
			TraceCheckComponents.Add(FIS_TraceCheckComponentInfo(BeInteractCom, TraceType));
		}
	}
	AllTraceCheckComponent = TraceCheckComponents;
}

TArray<UIS_BeInteractComponent*> UIS_InteractComponent::InteractCheckFromEnterType(EIS_InteractTraceType InteractTraceType)
{
	UIS_BeInteractComponent* TopPriorityCom = nullptr;
	TArray<UIS_BeInteractComponent*> AllBeInteractCom;
	FText FailText;
	switch (InteractTraceType)
	{
	case EIS_InteractTraceType::CameraTrace:
	{
		AllBeInteractCom = CameraTraceGetBeInteractCom(InteractCheckRayInfo, TopPriorityCom, FailText);
		break;
	}
	case EIS_InteractTraceType::SphereTrace:
	{
		AllBeInteractCom = SphereTraceGetBeInteractCom(GetOwner()->GetActorLocation(), GetOwner()->GetActorLocation(), SphereDistance, InteractCheckRayInfo, TopPriorityCom, FailText);
		break;
	}
	default:
		break;
	}

	//离开事件判断
	for (FIS_TraceCheckComponentInfo& TraceCheckComponentInfo : AllTraceCheckComponent)
	{
		//BeInteractCom有效（拾取道具会在交互后删除目标） && 曾经进入的交互组件不在新一批的交互组件中 && 检测类型是否通过
		if (TraceCheckComponentInfo.BeInteractComponent && !AllBeInteractCom.Contains(TraceCheckComponentInfo.BeInteractComponent) && TraceCheckComponentInfo.BeInteractComponent->TraceTypeCheck(InteractTraceType, false))
		{
			//不在，触发离开事件 离开需要触发交互结束，因此离开需要在服务器上被调用
			ServerLeaveInteractCheck(TraceCheckComponentInfo.BeInteractComponent, InteractTraceType);
			IIS_BeInteractInterface::Execute_InteractLeave(TraceCheckComponentInfo.BeInteractComponent, this, InteractTraceType);//客户端仍然需要移除该检测类型
			UpdateInteractTarget(nullptr, InteractTraceType);//广播交互事件
		}
	}

	//进入事件判断
	for (UIS_BeInteractComponent*& BeInteractCom : AllBeInteractCom)
	{
		if (BeInteractCom->TraceTypeCheck(InteractTraceType))
		{
			FIS_BeInteractCheckCondition Condition = BeInteractCom->BeInteractInfo.InteractCheckEnterCondition[InteractTraceType];
			EIS_BeInteractCheckType BeInteractEnterType = Condition.InteractCheckType;
			switch (BeInteractEnterType)//不在，尝试触发进入事件
			{
			case EIS_BeInteractCheckType::AnyTrigger:
			{
				IIS_BeInteractInterface::Execute_InteractEnter(BeInteractCom, this, InteractTraceType);//移入新交互目标
				UpdateInteractTarget(BeInteractCom, InteractTraceType);//广播交互事件
				break;
			}
			case EIS_BeInteractCheckType::TopPriorityTrigger:
			{
				if (TopPriorityCom == BeInteractCom)
				{
					IIS_BeInteractInterface::Execute_InteractEnter(BeInteractCom, this, InteractTraceType);//移入新交互目标
					UpdateInteractTarget(BeInteractCom, InteractTraceType);//广播交互事件
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

FVector UIS_InteractComponent::GetLocationFromTraceInfo(const FIS_InteractTypeInfo& TraceInfo, bool IsStartLocation)
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
		ReturnVector = GetLocationFromTraceType(TraceInfo.InteractType, IsStartLocation) + Location;
		break;
	}
	default:
		break;
	}

	return ReturnVector;
}

TArray<UIS_BeInteractComponent*> UIS_InteractComponent::InteractEnterCheckFromTraceInfo(FIS_InteractTypeInfo InteractTraceInfo)
{
	TArray<FHitResult> OutHit;
	UIS_BeInteractComponent* TopPriorityCom = nullptr;
	FText FailText;
	FVector Start = GetLocationFromTraceInfo(InteractTraceInfo, true);
	FVector End = GetLocationFromTraceInfo(InteractTraceInfo, false);

	switch (InteractTraceInfo.InteractTraceType)
	{
	case EIS_TraceType::Line:
	{
		switch (InteractTraceInfo.BeTraceType)
		{
		case EIS_BeTraceType::Channel:
		{
			UKismetSystemLibrary::LineTraceMulti(this, Start, End, InteractTraceInfo.TraceChannel, InteractTraceInfo.bTraceComplex, InteractTraceInfo.ActorsToIgnore, InteractTraceInfo.DrawDebugType, OutHit, InteractTraceInfo.bIgnoreSelf, InteractTraceInfo.TraceColor, InteractTraceInfo.TraceHitColor, InteractTraceInfo.DrawTime);
			break;
		}
		case EIS_BeTraceType::Objects:
		{
			UKismetSystemLibrary::LineTraceMultiForObjects(this, Start, End, InteractTraceInfo.ObjectTypes, InteractTraceInfo.bTraceComplex, InteractTraceInfo.ActorsToIgnore, InteractTraceInfo.DrawDebugType, OutHit, InteractTraceInfo.bIgnoreSelf, InteractTraceInfo.TraceColor, InteractTraceInfo.TraceHitColor);
			break;
		}
		case EIS_BeTraceType::Profile:
		{
			UKismetSystemLibrary::LineTraceMultiByProfile(this, Start, End, InteractTraceInfo.ProfileName, InteractTraceInfo.bTraceComplex, InteractTraceInfo.ActorsToIgnore, InteractTraceInfo.DrawDebugType, OutHit, InteractTraceInfo.bIgnoreSelf, InteractTraceInfo.TraceColor, InteractTraceInfo.TraceHitColor);
			break;
		}
		default:
			break;
		}
		break;
	}
	case EIS_TraceType::Sphere:
	{
		switch (InteractTraceInfo.BeTraceType)
		{
		case EIS_BeTraceType::Channel:
		{
			UKismetSystemLibrary::SphereTraceMulti(this, Start, End, InteractTraceInfo.Radius, InteractTraceInfo.TraceChannel, InteractTraceInfo.bTraceComplex, InteractTraceInfo.ActorsToIgnore, InteractTraceInfo.DrawDebugType, OutHit, InteractTraceInfo.bIgnoreSelf, InteractTraceInfo.TraceColor, InteractTraceInfo.TraceHitColor, InteractTraceInfo.DrawTime);
			break;
		}
		case EIS_BeTraceType::Objects:
		{
			UKismetSystemLibrary::SphereTraceMultiForObjects(this, Start, End, InteractTraceInfo.Radius, InteractTraceInfo.ObjectTypes, InteractTraceInfo.bTraceComplex, InteractTraceInfo.ActorsToIgnore, InteractTraceInfo.DrawDebugType, OutHit, InteractTraceInfo.bIgnoreSelf, InteractTraceInfo.TraceColor, InteractTraceInfo.TraceHitColor);
			break;
		}
		case EIS_BeTraceType::Profile:
		{
			UKismetSystemLibrary::SphereTraceMultiByProfile(this, Start, End, InteractTraceInfo.Radius, InteractTraceInfo.ProfileName, InteractTraceInfo.bTraceComplex, InteractTraceInfo.ActorsToIgnore, InteractTraceInfo.DrawDebugType, OutHit, InteractTraceInfo.bIgnoreSelf, InteractTraceInfo.TraceColor, InteractTraceInfo.TraceHitColor);
			break;
		}
		default:
			break;
		}
		break;
	}
	case EIS_TraceType::Box:
	{
		switch (InteractTraceInfo.BeTraceType)
		{
		case EIS_BeTraceType::Channel:
		{
			UKismetSystemLibrary::BoxTraceMulti(this, Start, End, InteractTraceInfo.HalfSize, InteractTraceInfo.Orientation, InteractTraceInfo.TraceChannel, InteractTraceInfo.bTraceComplex, InteractTraceInfo.ActorsToIgnore, InteractTraceInfo.DrawDebugType, OutHit, InteractTraceInfo.bIgnoreSelf, InteractTraceInfo.TraceColor, InteractTraceInfo.TraceHitColor, InteractTraceInfo.DrawTime);
			break;
		}
		case EIS_BeTraceType::Objects:
		{
			UKismetSystemLibrary::BoxTraceMultiForObjects(this, Start, End, InteractTraceInfo.HalfSize, InteractTraceInfo.Orientation, InteractTraceInfo.ObjectTypes, InteractTraceInfo.bTraceComplex, InteractTraceInfo.ActorsToIgnore, InteractTraceInfo.DrawDebugType, OutHit, InteractTraceInfo.bIgnoreSelf, InteractTraceInfo.TraceColor, InteractTraceInfo.TraceHitColor);
			break;
		}
		case EIS_BeTraceType::Profile:
		{
			UKismetSystemLibrary::BoxTraceMultiByProfile(this, Start, End, InteractTraceInfo.HalfSize, InteractTraceInfo.Orientation, InteractTraceInfo.ProfileName, InteractTraceInfo.bTraceComplex, InteractTraceInfo.ActorsToIgnore, InteractTraceInfo.DrawDebugType, OutHit, InteractTraceInfo.bIgnoreSelf, InteractTraceInfo.TraceColor, InteractTraceInfo.TraceHitColor);
			break;
		}
		default:
			break;
		}
		break;
	}
	case EIS_TraceType::Capsule:
	{
		switch (InteractTraceInfo.BeTraceType)
		{
		case EIS_BeTraceType::Channel:
		{
			UKismetSystemLibrary::CapsuleTraceMulti(this, Start, End, InteractTraceInfo.Radius, InteractTraceInfo.HalfHeight, InteractTraceInfo.TraceChannel, InteractTraceInfo.bTraceComplex, InteractTraceInfo.ActorsToIgnore, InteractTraceInfo.DrawDebugType, OutHit, InteractTraceInfo.bIgnoreSelf, InteractTraceInfo.TraceColor, InteractTraceInfo.TraceHitColor, InteractTraceInfo.DrawTime);
			break;
		}
		case EIS_BeTraceType::Objects:
		{
			UKismetSystemLibrary::CapsuleTraceMultiForObjects(this, Start, End, InteractTraceInfo.Radius, InteractTraceInfo.HalfHeight, InteractTraceInfo.ObjectTypes, InteractTraceInfo.bTraceComplex, InteractTraceInfo.ActorsToIgnore, InteractTraceInfo.DrawDebugType, OutHit, InteractTraceInfo.bIgnoreSelf, InteractTraceInfo.TraceColor, InteractTraceInfo.TraceHitColor);
			break;
		}
		case EIS_BeTraceType::Profile:
		{
			UKismetSystemLibrary::CapsuleTraceMultiByProfile(this, Start, End, InteractTraceInfo.Radius, InteractTraceInfo.HalfHeight, InteractTraceInfo.ProfileName, InteractTraceInfo.bTraceComplex, InteractTraceInfo.ActorsToIgnore, InteractTraceInfo.DrawDebugType, OutHit, InteractTraceInfo.bIgnoreSelf, InteractTraceInfo.TraceColor, InteractTraceInfo.TraceHitColor);
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

	return TraceOutHitVerify(InteractTraceInfo.InteractType, OutHit, TopPriorityCom, FailText);
}

FVector UIS_InteractComponent::GetLocationFromTraceType_Implementation(FGameplayTag TraceType, bool IsStartLocation)
{
	return FVector();
}

TArray<UIS_BeInteractComponent*> UIS_InteractComponent::InteractEnterCheckFromTraceType_Implementation(FGameplayTag TraceType)
{
	TArray<UIS_BeInteractComponent*> AllBeInteractCom;
	if (UIS_Config::GetInstance()->InteractTraceTypeMapping.Contains(TraceType))
	{
		AllBeInteractCom = InteractEnterCheckFromTraceInfo(UIS_Config::GetInstance()->InteractTraceTypeMapping[TraceType]);
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

void UIS_InteractComponent::UpdateInteractTarget_Implementation(UIS_BeInteractComponent* BeInteractComponent, EIS_InteractTraceType TraceType)
{
	UpdateInteractEvent.Broadcast(this, BeInteractComponent, TraceType);
}

void UIS_InteractComponent::ServerVerifyCurInteractComplete_Implementation()
{
	if (CurStartInteractComponent)
	{
		IIS_BeInteractInterface::Execute_InteractComplete(CurStartInteractComponent, this);
	}
}

void UIS_InteractComponent::ServerVerifyCurInteractEnd_Implementation()
{
	if (CurStartInteractComponent)
	{
		CurStartInteractComponent->BeInteractDynamicInfo.bIsVerifying = false;
		IIS_BeInteractInterface::Execute_InteractEnd(CurStartInteractComponent, this);
	}
}

void UIS_InteractComponent::ServerLeaveInteractCheck_Implementation(UIS_BeInteractComponent* BeInteractComponent, EIS_InteractTraceType InteractTraceType)
{
	if (BeInteractComponent)
	{
		IIS_BeInteractInterface::Execute_InteractLeave(BeInteractComponent, this, InteractTraceType);//移出旧交互目标
	}
}
