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
		if (CurStartInteractComponent)//��ǰ�Ƿ����Ѿ��ڽ�������һ����Դ
		{
			EndCurInteract();
		}
		CurStartInteractComponent = BeInteractComponent;
		IIS_BeInterface::Execute_InteractStart(BeInteractComponent, this);//֪ͨĿ�꿪ʼ����������
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
	TArray<UIS_BeInteractComponent*> AllBeInteract;//ȫ���ɱ��������
	TopPriorityCom = nullptr;
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (Pawn && Pawn->GetController())
	{
		FVector CameraLocation, TraceEndLocation;
		FRotator CameraRotation;
		Pawn->GetController()->GetPlayerViewPoint(CameraLocation, CameraRotation);
		TraceEndLocation = (UKismetMathLibrary::GetForwardVector(CameraRotation) * TraceDistance) + CameraLocation;

		//�������ģ�������������ʹ��Բ�ε�����
		TArray<FHitResult> OutHit;
		UKismetSystemLibrary::LineTraceMulti(this, CameraLocation, TraceEndLocation, TraceChannel, InteractRayInfo.bTraceComplex, InteractRayInfo.ActorsToIgnore, DrawDebugType, OutHit, InteractRayInfo.bIgnoreSelf, TraceColor, TraceHitColor, DrawTime);
		for (FHitResult& HitResult : OutHit)
		{
			UIS_BeInteractComponent* BeInteractComponent = Cast<UIS_BeInteractComponent>(HitResult.GetComponent());
			if (BeInteractComponent)
			{
				AllBeInteract.Add(BeInteractComponent);
				if (!TopPriorityCom || //Ϊ��ֱ������
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
	if (InteractCheckComponent)//��ǰ�Ƿ񱣴��˽�������Ŀ��
	{
		if (!TopPriorityCom)//��Ŀ��Ϊ��
		{
			IIS_BeInterface::Execute_InteractLeave(InteractCheckComponent, this);//�Ƴ��ɽ���Ŀ��
		}
		else if(TopPriorityCom != InteractCheckComponent)//��Ŀ���뱣���Ŀ�겻һ��
		{
			IIS_BeInterface::Execute_InteractLeave(InteractCheckComponent, this);//�Ƴ��ɽ���Ŀ��
			IIS_BeInterface::Execute_InteractEnter(TopPriorityCom, this);//�����½���Ŀ��
			UpdateInteractTarget(TopPriorityCom);
		}
	}
	else if(TopPriorityCom)
	{
		IIS_BeInterface::Execute_InteractEnter(TopPriorityCom, this);//�����½���Ŀ��
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
