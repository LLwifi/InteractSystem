// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent/BeInteractExtend/IS_BIEInteractOutLine.h"
#include "ActorComponent/IS_BeInteractComponent.h"

void UIS_BIEInteractOutLine::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UIS_BIEInteractOutLine, FindOutLineMeshComponnetTag);
	DOREPLIFETIME(UIS_BIEInteractOutLine, CustomDepthStencilValue_OutLine);
	DOREPLIFETIME(UIS_BIEInteractOutLine, EnterInteractTraceType);
	DOREPLIFETIME(UIS_BIEInteractOutLine, OutLineMeshComponnets);
}

void UIS_BIEInteractOutLine::Init_Implementation(UIS_BeInteractComponent* BeInteractCom, UIS_BeInteractExtendBase* Data)
{
	Super::Init_Implementation(BeInteractCom, Data);

	UIS_BIEInteractOutLine* OutLine = Cast<UIS_BIEInteractOutLine>(Data);
	if(OutLine)
	{
		FindOutLineMeshComponnetTag = OutLine->FindOutLineMeshComponnetTag;
		CustomDepthStencilValue_OutLine = OutLine->CustomDepthStencilValue_OutLine;
		EnterInteractTraceType = OutLine->EnterInteractTraceType;
	}

	if (BeInteractComponent)
	{
		if (FindOutLineMeshComponnetTag.IsNone())//���û��ָ��tag����ôѰ��ȫ����mesh���
		{
			BeInteractComponent->GetOwner()->GetComponents<UMeshComponent>(OutLineMeshComponnets);
		}
		else
		{
			TArray<UActorComponent*> AllComponnets;
			AllComponnets = BeInteractComponent->GetOwner()->GetComponentsByTag(UMeshComponent::StaticClass(), FindOutLineMeshComponnetTag);
			for (UActorComponent*& Com : AllComponnets)
			{
				OutLineMeshComponnets.Add(Cast<UMeshComponent>(Com));
			}
		}
	}
}

void UIS_BIEInteractOutLine::InteractEnter_Implementation(UIS_InteractComponent* InteractComponent, EIS_InteractTraceType TraceType)
{
	Super::InteractEnter_Implementation(InteractComponent, TraceType);

	NetClient_OnInteractEnter(InteractComponent, TraceType);
}

void UIS_BIEInteractOutLine::InteractLeave_Implementation(UIS_InteractComponent* InteractComponent, EIS_InteractTraceType TraceType)
{
	Super::InteractLeave_Implementation(InteractComponent, TraceType);

	NetClient_OnInteractLeave(InteractComponent, TraceType);
}

bool UIS_BIEInteractOutLine::InteractLeaveIsEnd_Implementation()
{
	return false;
}

bool UIS_BIEInteractOutLine::TryInteract_Implementation(UIS_InteractComponent* InteractComponent)
{
	return false;
}

bool UIS_BIEInteractOutLine::InteractCheck_Implementation(UIS_InteractComponent* InteractComponent)
{
	return false;
}

void UIS_BIEInteractOutLine::InteractStart_Implementation(UIS_InteractComponent* InteractComponent)
{
}

void UIS_BIEInteractOutLine::InteractEnd_Implementation(UIS_InteractComponent* InteractComponent)
{
}

void UIS_BIEInteractOutLine::InteractComplete_Implementation(UIS_InteractComponent* InteractComponent)
{
}

void UIS_BIEInteractOutLine::InteractComplete_MultiSegment_Implementation(UIS_InteractComponent* InteractComponent)
{
}

void UIS_BIEInteractOutLine::InteractAttachTo_Implementation(UIS_InteractComponent* InteractComponent)
{
}

void UIS_BIEInteractOutLine::InteractAttachDetach_Implementation(UIS_InteractComponent* InteractComponent)
{
}

void UIS_BIEInteractOutLine::NetClient_OnInteractEnter_Implementation(UIS_InteractComponent* InteractComponent, EIS_InteractTraceType TraceType)
{
	if (EnterInteractTraceType.Contains(TraceType))
	{
		if (!CurEnterInteractTraceType.Contains(TraceType))
		{
			CurEnterInteractTraceType.Add(TraceType);
			ChangeOutLineCount(1);
		}
	}
}

void UIS_BIEInteractOutLine::NetClient_OnInteractLeave_Implementation(UIS_InteractComponent* InteractComponent, EIS_InteractTraceType TraceType)
{
	if (EnterInteractTraceType.Contains(TraceType))
	{
		if (CurEnterInteractTraceType.Contains(TraceType))
		{
			CurEnterInteractTraceType.Remove(TraceType);
			ChangeOutLineCount(-1);
		}
	}
}

int32 UIS_BIEInteractOutLine::ChangeOutLineCount(int32 AddOutLineNum)
{
	if (OutLineMeshComponnets.Num() > 0)
	{
		OutLineCount += AddOutLineNum;
		if (OutLineCount > 0)
		{
			for (UMeshComponent*& Com : OutLineMeshComponnets)
			{
				if (Com)
				{
					Com->SetRenderCustomDepth(true);
					Com->SetCustomDepthStencilValue(CustomDepthStencilValue_OutLine);
				}
			}
		}
		else
		{
			for (UMeshComponent*& Com : OutLineMeshComponnets)
			{
				if (Com)
				{
					Com->SetRenderCustomDepth(false);
				}
			}
		}
	}
	return OutLineCount;
}
