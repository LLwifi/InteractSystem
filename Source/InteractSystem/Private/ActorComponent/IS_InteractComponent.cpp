// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent/IS_InteractComponent.h"
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetMathLibrary.h>
#include "Common/IS_Interface.h"


// Sets default values for this component's properties
UIS_InteractComponent::UIS_InteractComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

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
	InteractCheckStateChange(bBeginPlayIsActiveInteractCheck);
}

void UIS_InteractComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(UTS_TaskComponent, AllTask);
}

// Called every frame
void UIS_InteractComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UIS_InteractComponent::Interact(USceneComponent* BeInteractComponent)
{
	return true;
}

USceneComponent* UIS_InteractComponent::CameraTraceGetTopPriority(FCC_InteractRayInfo InteractRayInfo)
{
	USceneComponent* TopPriorityCom = nullptr;//最大优先级的可被交互资源
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (Pawn)
	{
		FVector CameraLocation, TraceEndLocation;
		FRotator CameraRotation;
		Pawn->GetController()->GetPlayerViewPoint(CameraLocation, CameraRotation);
		TraceEndLocation = (UKismetMathLibrary::GetForwardVector(CameraRotation) * TraceDistance) + CameraLocation;

		//如果考虑模糊处理，这里可以使用圆形的射线
		TArray<FHitResult> OutHit;
		if (UKismetSystemLibrary::LineTraceMulti(this, CameraLocation, TraceEndLocation, TraceChannel, InteractRayInfo.bTraceComplex, InteractRayInfo.ActorsToIgnore, DrawDebugType, OutHit, InteractRayInfo.bIgnoreSelf, TraceColor, TraceHitColor, DrawTime))
		{
			for (FHitResult& HitResult : OutHit)
			{
				USceneComponent* SceneComponent = Cast<USceneComponent>(HitResult.GetComponent());
				if (SceneComponent && SceneComponent->Implements<UInterface>())
				{
					if (!TopPriorityCom || //为空直接设置
						IIS_Interface::Execute_GetInteractPriority(SceneComponent) > IIS_Interface::Execute_GetInteractPriority(TopPriorityCom))
					{
						TopPriorityCom = SceneComponent;
					}
				}
			}
		}
	}
	return TopPriorityCom;
}

bool UIS_InteractComponent::TryTriggerInteract_CameraTrace(FCC_InteractRayInfo InteractRayInfo, TArray<FHitResult>& OutHit)
{
	bool ReturnBool = false;
	USceneComponent* BeInteractComponent = CameraTraceGetTopPriority(InteractRayInfo);
	if (BeInteractComponent)
	{
		Interact(BeInteractComponent);
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
	USceneComponent* BeInteractComponent = CameraTraceGetTopPriority(InteractCheckRayInfo);
	if (InteractCheckComponent)//当前是否保存了交互检测的目标
	{
		if (!BeInteractComponent)//新目标为空
		{
			IIS_Interface::Execute_InteractLeave(InteractCheckComponent, this);//移出旧交互目标
		}
		else if(BeInteractComponent != InteractCheckComponent)//新目标与保存的目标不一致
		{
			IIS_Interface::Execute_InteractLeave(InteractCheckComponent, this);//移出旧交互目标
			IIS_Interface::Execute_InteractEnter(BeInteractComponent, this);//移入新交互目标
		}
	}
	else if(BeInteractComponent)
	{
		IIS_Interface::Execute_InteractEnter(BeInteractComponent, this);//移入新交互目标
	}
	InteractCheckComponent = BeInteractComponent;
}
