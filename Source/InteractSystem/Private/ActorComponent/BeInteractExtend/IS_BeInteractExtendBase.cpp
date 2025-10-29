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

void UIS_BeInteractExtendBase::Init_Implementation(UIS_BeInteractComponent* BeInteractCom, UIS_BeInteractExtendBase* Data)
{
	BeInteractComponent = BeInteractCom;
}

bool UIS_BeInteractExtendBase::CanInteract_Implementation(UIS_InteractComponent* InteractComponent, FCC_CompareInfo OuterCompareInfo, FText& FailText)
{
	return true;
}

void UIS_BeInteractExtendBase::InteractEnter_Implementation(UIS_InteractComponent* InteractComponent, EIS_InteractTraceType TraceType)
{
}

void UIS_BeInteractExtendBase::InteractLeave_Implementation(UIS_InteractComponent* InteractComponent, EIS_InteractTraceType TraceType)
{
}

bool UIS_BeInteractExtendBase::InteractLeaveIsEnd_Implementation()
{
	return false;
}

bool UIS_BeInteractExtendBase::TryInteract_Implementation(UIS_InteractComponent* InteractComponent)
{
	return false;
}

bool UIS_BeInteractExtendBase::InteractCheck_Implementation(UIS_InteractComponent* InteractComponent)
{
	return false;
}

void UIS_BeInteractExtendBase::InteractStart_Implementation(UIS_InteractComponent* InteractComponent)
{
}

void UIS_BeInteractExtendBase::InteractEnd_Implementation(UIS_InteractComponent* InteractComponent)
{
}

void UIS_BeInteractExtendBase::InteractComplete_Implementation(UIS_InteractComponent* InteractComponent)
{
}

void UIS_BeInteractExtendBase::InteractComplete_MultiSegment_Implementation(UIS_InteractComponent* InteractComponent)
{
}

void UIS_BeInteractExtendBase::InteractAttachTo_Implementation(UIS_InteractComponent* InteractComponent)
{
}

void UIS_BeInteractExtendBase::InteractAttachDetach_Implementation(UIS_InteractComponent* InteractComponent)
{
}

