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
	//DOREPLIFETIME(UIS_InteractComponent, InteractCheckComponents);
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

TArray<UIS_BeInteractComponent*> UIS_InteractComponent::TraceOutHitCheck(FIS_InteractRayInfo InteractRayInfo, const TArray<FHitResult>& OutHit, UIS_BeInteractComponent*& TopPriorityCom)
{
	TArray<UIS_BeInteractComponent*> AllBeInteract;//全部可被交互组件
	for (const FHitResult& HitResult : OutHit)
	{
		UIS_BeInteractComponent* BeInteractComponent = Cast<UIS_BeInteractComponent>(HitResult.GetComponent());
		if (BeInteractComponent && (!BeInteractComponent->BeInteractInfo.bNotActiveIsTraceCheck ? IIS_BeInteractInterface::Execute_IsInteractActive(BeInteractComponent) : true))
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
	for (UIS_BeInteractComponent*& BeInteractComponent : InteractCheckComponents)
	{
		if (InteractTraceType == EIS_InteractTraceType::None || BeInteractComponent->BeInteractDynamicInfo.AllEnterTraceType.Contains(InteractTraceType))
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

TArray<UIS_BeInteractComponent*> UIS_InteractComponent::CameraTraceGetBeInteractCom(FIS_InteractRayInfo InteractRayInfo, UIS_BeInteractComponent*& TopPriorityCom)
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
		AllBeInteract = TraceOutHitCheck(InteractRayInfo, OutHit, TopPriorityCom);
	}
	return AllBeInteract;
}

bool UIS_InteractComponent::TryTriggerInteract_CameraTrace(FCC_CompareInfo CompareInfo, FIS_InteractRayInfo InteractRayInfo, UIS_BeInteractComponent*& TopPriorityCom, TArray<UIS_BeInteractComponent*>& AllBeInteract, FText& FailText)
{
	InteractCheck();
	bool ReturnBool = false;
	AllBeInteract = CameraTraceGetBeInteractCom(InteractRayInfo, TopPriorityCom);
	if (TopPriorityCom && IIS_BeInteractInterface::Execute_CanInteract(TopPriorityCom, this, CompareInfo, FailText))
	{
		StartInteract(TopPriorityCom);
		UpdateInteractTarget(TopPriorityCom, EIS_InteractTraceType::CameraTrace);
		ReturnBool = true;
	}
	return ReturnBool;
}

TArray<UIS_BeInteractComponent*> UIS_InteractComponent::SphereTraceGetBeInteractCom(FVector Start, FVector End, float Radius, FIS_InteractRayInfo InteractRayInfo, UIS_BeInteractComponent*& TopPriorityCom)
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
		AllBeInteract = TraceOutHitCheck(InteractRayInfo, OutHit, TopPriorityCom);
	}
	return AllBeInteract;
}

bool UIS_InteractComponent::TryTriggerInteract_SphereTrace(FCC_CompareInfo CompareInfo, FIS_InteractRayInfo InteractRayInfo, FVector Start, FVector End, float Radius, UIS_BeInteractComponent*& TopPriorityCom, TArray<UIS_BeInteractComponent*>& AllBeInteract, FText& FailText)
{
	InteractCheck();
	bool ReturnBool = false;
	AllBeInteract = SphereTraceGetBeInteractCom(Start, End, Radius, InteractRayInfo, TopPriorityCom);
	if (TopPriorityCom && IIS_BeInteractInterface::Execute_CanInteract(TopPriorityCom, this, CompareInfo, FailText))
	{
		StartInteract(TopPriorityCom);
		UpdateInteractTarget(TopPriorityCom, EIS_InteractTraceType::SphereTrace);
		ReturnBool = true;
	}
	return ReturnBool;
}

void UIS_InteractComponent::InteractCheckStateChange(bool IsActive)
{
	if (IsActive)
	{
		GetWorld()->GetTimerManager().SetTimer(InteractCheckTimeHandle, this, &UIS_InteractComponent::InteractCheck, InteractCheckInterval, true);
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(InteractCheckTimeHandle);
	}
}

void UIS_InteractComponent::InteractCheck()
{
	//Enter可以被多次触发，但每种检测的类型只能触发一次
	TSet<UIS_BeInteractComponent*> AllBeInteractCom;
	for (EIS_InteractTraceType& TraceType : InteractCheckTypes)
	{
		AllBeInteractCom.Append(InteractCheckFromEnterType(TraceType));
	}

	InteractCheckComponents = AllBeInteractCom.Array();
}

TArray<UIS_BeInteractComponent*> UIS_InteractComponent::InteractCheckFromEnterType(EIS_InteractTraceType InteractTraceType)
{
	UIS_BeInteractComponent* TopPriorityCom = nullptr;
	TArray<UIS_BeInteractComponent*> AllBeInteractCom;
	switch (InteractTraceType)
	{
	case EIS_InteractTraceType::CameraTrace:
	{
		AllBeInteractCom = CameraTraceGetBeInteractCom(InteractCheckRayInfo, TopPriorityCom);
		break;
	}
	case EIS_InteractTraceType::SphereTrace:
	{
		AllBeInteractCom = SphereTraceGetBeInteractCom(GetOwner()->GetActorLocation(), GetOwner()->GetActorLocation(), SphereDistance, InteractCheckRayInfo, TopPriorityCom);
		break;
	}
	default:
		break;
	}

	TArray<UIS_BeInteractComponent*> LastInteractCheckComponents = InteractCheckComponents;
	for (UIS_BeInteractComponent*& BeInteractCom : LastInteractCheckComponents)
	{
		//曾经进入的交互组件还在不在新一批的交互组件中 && 检测类型是否通过
		if (!AllBeInteractCom.Contains(BeInteractCom) && BeInteractCom->TraceTypeCheck(InteractTraceType, false))
		{
			//不在，触发离开事件
			IIS_BeInteractInterface::Execute_InteractLeave(BeInteractCom, this, InteractTraceType);//移出旧交互目标
			UpdateInteractTarget(nullptr, InteractTraceType);
		}
	}

	for (UIS_BeInteractComponent*& BeInteractCom : AllBeInteractCom)
	{
		if (BeInteractCom->TraceTypeCheck(InteractTraceType))
		{
			EIS_BeInteractEnterType BeInteractEnterType = BeInteractCom->BeInteractInfo.InteractEnterTriggerType[InteractTraceType];
			switch (BeInteractEnterType)//不在，尝试触发进入事件
			{
			case EIS_BeInteractEnterType::AnyTrigger:
			{
				IIS_BeInteractInterface::Execute_InteractEnter(BeInteractCom, this, InteractTraceType);//移入新交互目标
				UpdateInteractTarget(BeInteractCom, InteractTraceType);
				break;
			}
			case EIS_BeInteractEnterType::TopPriorityTrigger:
			{
				if (TopPriorityCom == BeInteractCom)
				{
					IIS_BeInteractInterface::Execute_InteractEnter(BeInteractCom, this, InteractTraceType);//移入新交互目标
					UpdateInteractTarget(BeInteractCom, InteractTraceType);
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

void UIS_InteractComponent::UpdateInteractTarget_Implementation(UIS_BeInteractComponent* BeInteractComponent, EIS_InteractTraceType TraceType)
{
	UpdateInteractEvent.Broadcast(this, BeInteractComponent, TraceType);
}
