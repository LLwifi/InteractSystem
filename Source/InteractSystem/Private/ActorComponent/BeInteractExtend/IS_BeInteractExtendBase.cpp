// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent/BeInteractExtend/IS_BeInteractExtendBase.h"
#include <ActorComponent/IS_BeInteractComponent.h>
#include "GameFramework/Actor.h"

// Sets default values for this component's properties
UIS_BeInteractExtendBase::UIS_BeInteractExtendBase()
{
}

UWorld* UIS_BeInteractExtendBase::GetWorld() const
{
	UObject* outer = GetOuter();
	if (outer && (outer->IsA<AActor>() || outer->IsA<UActorComponent>() || outer->IsA<USubsystem>()) && !outer->HasAnyFlags(RF_ClassDefaultObject))
	{
		return outer->GetWorld();
	}
	return nullptr;
}

bool UIS_BeInteractExtendBase::IsSupportedForNetworking() const
{
	return true;
}

void UIS_BeInteractExtendBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UIS_BeInteractExtendBase, ExtendHandle);
	DOREPLIFETIME(UIS_BeInteractExtendBase, NetType);
	DOREPLIFETIME(UIS_BeInteractExtendBase, DTData);
	DOREPLIFETIME(UIS_BeInteractExtendBase, BeInteractComponent);
}

//void UIS_BeInteractExtendBase::ReplicatedUsing_bIsInit()
//{
//	//
//}

void UIS_BeInteractExtendBase::ReplicatedUsing_NetType()
{
	switch (NetType)
	{
	case EIS_InteractEventNetType::Server:
	{
		break;
	}
	case EIS_InteractEventNetType::Client:
	{
		Init(BeInteractComponent, DTData);
		break;
	}
	case EIS_InteractEventNetType::NetMulticast:
	{
		Init(BeInteractComponent, DTData);
		break;
	}
	default:
		break;
	}
}

//void UIS_BeInteractExtendBase::ReplicatedUsing_BeInteractComponent()
//{
//	
//}

void UIS_BeInteractExtendBase::Init_RepCheck(UIS_BeInteractComponent* BeInteractCom, UIS_BeInteractExtendBase* Data)
{
	BeInteractComponent = BeInteractCom;
	DTData = Data;
	NetType = DTData->NetType;
	//该函数在服务器上被调用
	switch (NetType)
	{
	case EIS_InteractEventNetType::Server:
	{
		Init(BeInteractCom, Data);
		break;
	}
	case EIS_InteractEventNetType::Client:
	{
		break;
	}
	case EIS_InteractEventNetType::NetMulticast:
	{
		Init(BeInteractCom, Data);
		break;
	}
	default:
		break;
	}
}

void UIS_BeInteractExtendBase::Init_Implementation(UIS_BeInteractComponent* BeInteractCom, UIS_BeInteractExtendBase* Data)
{

}

bool UIS_BeInteractExtendBase::CanInteract_Implementation(UIS_InteractComponent* InteractComponent, FCC_CompareInfo OuterCompareInfo, FText& FailText)
{
	return true;
}

void UIS_BeInteractExtendBase::InteractEnter_Implementation(UIS_InteractComponent* InteractComponent, FGameplayTag TraceTypeTag)
{
}

void UIS_BeInteractExtendBase::InteractLeave_Implementation(UIS_InteractComponent* InteractComponent, FGameplayTag TraceTypeTag)
{
}

bool UIS_BeInteractExtendBase::InteractLeaveIsEnd_Implementation()
{
	return false;
}

void UIS_BeInteractExtendBase::InteractStart_Implementation(UIS_InteractComponent* InteractComponent, FGameplayTag TraceTypeTag)
{
}

void UIS_BeInteractExtendBase::InteractEnd_Implementation(UIS_InteractComponent* InteractComponent, FGameplayTag TraceTypeTag)
{
}

void UIS_BeInteractExtendBase::InteractComplete_Implementation(UIS_InteractComponent* InteractComponent, FGameplayTag TraceTypeTag)
{
}

void UIS_BeInteractExtendBase::InteractComplete_MultiSegment_Implementation(UIS_InteractComponent* InteractComponent, FGameplayTag TraceTypeTag)
{
}

void UIS_BeInteractExtendBase::InteractAttachTo_Implementation(UIS_InteractComponent* InteractComponent, FGameplayTag TraceTypeTag)
{
}

void UIS_BeInteractExtendBase::InteractAttachDetach_Implementation(UIS_InteractComponent* InteractComponent, FGameplayTag TraceTypeTag)
{
}

