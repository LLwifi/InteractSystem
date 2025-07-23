// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent/IS_InteractComponent.h"
#include "ActorComponent/IS_BeInteractComponent.h"
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetMathLibrary.h>
#include "Common/IS_BeInterface.h"
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
	if (UKismetSystemLibrary::IsServer(this))
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
		IIS_BeInterface::Execute_InteractStart(BeInteractComponent, this);//通知目标开始与他交互了
		UpdateInteractTarget(BeInteractComponent);
	}
	
	return true;
}

void UIS_InteractComponent::EndCurInteract()
{
	if (CurStartInteractComponent)
	{
		IIS_BeInterface::Execute_InteractEnd(CurStartInteractComponent, this);
		CurStartInteractComponent = nullptr;
	}
}

TArray<UIS_BeInteractComponent*> UIS_InteractComponent::CameraTraceGetBeInteractCom(FCC_InteractRayInfo InteractRayInfo, UIS_BeInteractComponent*& TopPriorityCom)
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
		for (FHitResult& HitResult : OutHit)
		{
			UIS_BeInteractComponent* BeInteractComponent = Cast<UIS_BeInteractComponent>(HitResult.GetComponent());
			if (BeInteractComponent)
			{
				AllBeInteract.Add(BeInteractComponent);
				if (!TopPriorityCom || //为空直接设置
					IIS_BeInterface::Execute_GetInteractPriority(BeInteractComponent) > IIS_BeInterface::Execute_GetInteractPriority(TopPriorityCom))
				{
					TopPriorityCom = BeInteractComponent;
				}
			}
		}
	}
	return AllBeInteract;
}

bool UIS_InteractComponent::TryTriggerInteract_CameraTrace(FCC_CompareInfo CompareInfo, FCC_InteractRayInfo InteractRayInfo, UIS_BeInteractComponent*& TopPriorityCom, TArray<UIS_BeInteractComponent*>& AllBeInteract, FText& FailText)
{
	InteractCheck();
	bool ReturnBool = false;
	AllBeInteract = CameraTraceGetBeInteractCom(InteractRayInfo, TopPriorityCom);
	if (TopPriorityCom && IIS_BeInterface::Execute_CanInteract(TopPriorityCom, this, CompareInfo, FailText))
	{
		StartInteract(TopPriorityCom);
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
	UIS_BeInteractComponent* TopPriorityCom;
	CameraTraceGetBeInteractCom(InteractCheckRayInfo, TopPriorityCom);
	if (InteractCheckComponent)//当前是否保存了交互检测的目标
	{
		if (!TopPriorityCom)//新目标为空
		{
			IIS_BeInterface::Execute_InteractLeave(InteractCheckComponent, this);//移出旧交互目标
		}
		else if(TopPriorityCom != InteractCheckComponent)//新目标与保存的目标不一致
		{
			IIS_BeInterface::Execute_InteractLeave(InteractCheckComponent, this);//移出旧交互目标
			IIS_BeInterface::Execute_InteractEnter(TopPriorityCom, this);//移入新交互目标
			UpdateInteractTarget(TopPriorityCom);
		}
	}
	else if(TopPriorityCom)
	{
		IIS_BeInterface::Execute_InteractEnter(TopPriorityCom, this);//移入新交互目标
		UpdateInteractTarget(TopPriorityCom);
	}
	InteractCheckComponent = TopPriorityCom;
}

void UIS_InteractComponent::ServerVerifyCurInteractComplete_Implementation()
{
	if (CurStartInteractComponent)
	{
		IIS_BeInterface::Execute_InteractComplete(CurStartInteractComponent, this);
	}
}

void UIS_InteractComponent::ServerVerifyCurInteractEnd_Implementation()
{
	if (CurStartInteractComponent)
	{
		CurStartInteractComponent->BeInteractDynamicInfo.bIsVerifying = false;
		IIS_BeInterface::Execute_InteractEnd(CurStartInteractComponent, this);
	}
}

void UIS_InteractComponent::UpdateInteractTarget_Implementation(UIS_BeInteractComponent* BeInteractComponent)
{
	UpdateInteractEvent.Broadcast(this, BeInteractComponent);
}
