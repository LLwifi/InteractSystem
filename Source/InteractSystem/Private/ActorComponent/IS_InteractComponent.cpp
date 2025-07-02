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

	auto Property = PropertyChangedEvent.Property;//�õ��ı������
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
	USceneComponent* TopPriorityCom = nullptr;//������ȼ��Ŀɱ�������Դ
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (Pawn)
	{
		FVector CameraLocation, TraceEndLocation;
		FRotator CameraRotation;
		Pawn->GetController()->GetPlayerViewPoint(CameraLocation, CameraRotation);
		TraceEndLocation = (UKismetMathLibrary::GetForwardVector(CameraRotation) * TraceDistance) + CameraLocation;

		//�������ģ�������������ʹ��Բ�ε�����
		TArray<FHitResult> OutHit;
		if (UKismetSystemLibrary::LineTraceMulti(this, CameraLocation, TraceEndLocation, TraceChannel, InteractRayInfo.bTraceComplex, InteractRayInfo.ActorsToIgnore, DrawDebugType, OutHit, InteractRayInfo.bIgnoreSelf, TraceColor, TraceHitColor, DrawTime))
		{
			for (FHitResult& HitResult : OutHit)
			{
				USceneComponent* SceneComponent = Cast<USceneComponent>(HitResult.GetComponent());
				if (SceneComponent && SceneComponent->Implements<UInterface>())
				{
					if (!TopPriorityCom || //Ϊ��ֱ������
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
	if (InteractCheckComponent)//��ǰ�Ƿ񱣴��˽�������Ŀ��
	{
		if (!BeInteractComponent)//��Ŀ��Ϊ��
		{
			IIS_Interface::Execute_InteractLeave(InteractCheckComponent, this);//�Ƴ��ɽ���Ŀ��
		}
		else if(BeInteractComponent != InteractCheckComponent)//��Ŀ���뱣���Ŀ�겻һ��
		{
			IIS_Interface::Execute_InteractLeave(InteractCheckComponent, this);//�Ƴ��ɽ���Ŀ��
			IIS_Interface::Execute_InteractEnter(BeInteractComponent, this);//�����½���Ŀ��
		}
	}
	else if(BeInteractComponent)
	{
		IIS_Interface::Execute_InteractEnter(BeInteractComponent, this);//�����½���Ŀ��
	}
	InteractCheckComponent = BeInteractComponent;
}
