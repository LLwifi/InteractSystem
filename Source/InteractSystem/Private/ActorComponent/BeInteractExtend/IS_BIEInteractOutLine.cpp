// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent/BeInteractExtend/IS_BIEInteractOutLine.h"
#include "ActorComponent/IS_BeInteractComponent.h"
#include "ActorComponent/IS_InteractComponent.h"
#include "TimerManager.h"

void UIS_BIEInteractOutLine::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UIS_BIEInteractOutLine, CustomDepthStencilValue_OutLine);
	DOREPLIFETIME(UIS_BIEInteractOutLine, RealTimeRefresh);
	DOREPLIFETIME(UIS_BIEInteractOutLine, DelayRefreshTime);
	DOREPLIFETIME(UIS_BIEInteractOutLine, OutLineMeshComponnets);
	DOREPLIFETIME(UIS_BIEInteractOutLine, OutLineInteractTraceTypeTags);
}

void UIS_BIEInteractOutLine::Init_Implementation(UIS_BeInteractComponent* BeInteractCom, UIS_BeInteractExtendBase* Data)
{
	Super::Init_Implementation(BeInteractCom, Data);

	UIS_BIEInteractOutLine* DataCom = Cast<UIS_BIEInteractOutLine>(Data);
	if(DataCom)
	{
		CustomDepthStencilValue_OutLine = DataCom->CustomDepthStencilValue_OutLine;
		OutLineInteractTraceTypeTags = DataCom->OutLineInteractTraceTypeTags;
		RealTimeRefresh = DataCom->RealTimeRefresh;
		DelayRefreshTime = DataCom->DelayRefreshTime;
	}

	if (!RealTimeRefresh)
	{
		if (DelayRefreshTime > 0.0f)
		{
			GetWorld()->GetTimerManager().SetTimer(DelayRefreshTimeHandle, this, &UIS_BIEInteractOutLine::RefreshOutLineMeshComponnet, DelayRefreshTime);
		}
		else
		{
			RefreshOutLineMeshComponnet();
		}
	}
}

void UIS_BIEInteractOutLine::InteractEnter_Implementation(UIS_InteractComponent* InteractComponent, FGameplayTag TraceTypeTag)
{
	Super::InteractEnter_Implementation(InteractComponent, TraceTypeTag);

	if (RealTimeRefresh)
	{
		RefreshOutLineMeshComponnet();
	}

	FText FailText;
	if (OutLineInteractTraceTypeTags.HasTagExact(TraceTypeTag))
	{
		if (!CurEnterInteractTraceTypeTags.HasTagExact(TraceTypeTag))
		{
			CurEnterInteractTraceTypeTags.AddTag(TraceTypeTag);
			ChangeOutLineCount(1);
		}
	}
}

void UIS_BIEInteractOutLine::InteractLeave_Implementation(UIS_InteractComponent* InteractComponent, FGameplayTag TraceTypeTag)
{
	Super::InteractLeave_Implementation(InteractComponent, TraceTypeTag);

	if (OutLineInteractTraceTypeTags.HasTagExact(TraceTypeTag))
	{
		if (CurEnterInteractTraceTypeTags.HasTagExact(TraceTypeTag))
		{
			CurEnterInteractTraceTypeTags.RemoveTag(TraceTypeTag);
			ChangeOutLineCount(-1);
		}
	}
}

bool UIS_BIEInteractOutLine::InteractLeaveIsEnd_Implementation()
{
	return false;
}

void UIS_BIEInteractOutLine::InteractStart_Implementation(UIS_InteractComponent* InteractComponent, FGameplayTag TraceTypeTag)
{
}

void UIS_BIEInteractOutLine::InteractEnd_Implementation(UIS_InteractComponent* InteractComponent, FGameplayTag TraceTypeTag)
{
}

void UIS_BIEInteractOutLine::InteractComplete_Implementation(UIS_InteractComponent* InteractComponent, FGameplayTag TraceTypeTag)
{
}

void UIS_BIEInteractOutLine::InteractComplete_MultiSegment_Implementation(UIS_InteractComponent* InteractComponent, FGameplayTag TraceTypeTag)
{
}

void UIS_BIEInteractOutLine::InteractAttachTo_Implementation(UIS_InteractComponent* InteractComponent, FGameplayTag TraceTypeTag)
{
}

void UIS_BIEInteractOutLine::InteractAttachDetach_Implementation(UIS_InteractComponent* InteractComponent, FGameplayTag TraceTypeTag)
{
}

int32 UIS_BIEInteractOutLine::ChangeOutLineCount(int32 AddOutLineNum)
{
	if (OutLineMeshComponnets.Num() > 0)
	{
		OutLineCount += AddOutLineNum;
		if (OutLineCount > 0)
		{
			for (UPrimitiveComponent*& Com : OutLineMeshComponnets)
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
			for (UPrimitiveComponent*& Com : OutLineMeshComponnets)
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

void UIS_BIEInteractOutLine::ReplicatedUsing_OutLineInteractTraceTypeTag()
{

}

void UIS_BIEInteractOutLine::RefreshOutLineMeshComponnet()
{
	OutLineMeshComponnets.Empty();
	for (UActorComponent*& ActorCom : GetComponent())
	{
		UPrimitiveComponent* PrimitiveCom = Cast<UPrimitiveComponent>(ActorCom);
		if (PrimitiveCom)
		{
			OutLineMeshComponnets.Add(PrimitiveCom);
		}
	}
}
