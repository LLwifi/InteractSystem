// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent/BeInteractExtend/IS_BIEInteractOutLine.h"
#include "ActorComponent/IS_BeInteractComponent.h"
#include "ActorComponent/IS_InteractComponent.h"
#include "TimerManager.h"

void UIS_BIEInteractOutLine::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UIS_BIEInteractOutLine, CustomDepthStencilValue_OutLine);
	DOREPLIFETIME(UIS_BIEInteractOutLine, OutLineInteractTraceTypeInfo);
	DOREPLIFETIME(UIS_BIEInteractOutLine, OutLineMeshComponnets);
}

void UIS_BIEInteractOutLine::Init_Implementation(UIS_BeInteractComponent* BeInteractCom, UIS_BeInteractExtendBase* Data)
{
	Super::Init_Implementation(BeInteractCom, Data);

	UIS_BIEInteractOutLine* DataCom = Cast<UIS_BIEInteractOutLine>(Data);
	if(DataCom)
	{
		CustomDepthStencilValue_OutLine = DataCom->CustomDepthStencilValue_OutLine;
		OutLineInteractTraceTypeInfo = DataCom->OutLineInteractTraceTypeInfo;
	}

	for (UActorComponent*& ActorCom : GetComponent())
	{
		UPrimitiveComponent* PrimitiveCom = Cast<UPrimitiveComponent>(ActorCom);
		if (PrimitiveCom)
		{
			OutLineMeshComponnets.Add(PrimitiveCom);
		}
	}

	//主动调的这一次主要是为服务器设置
	ReplicatedUsing_OutLineInteractTraceTypeInfo();
}

void UIS_BIEInteractOutLine::InteractEnter_Implementation(UIS_InteractComponent* InteractComponent, EIS_InteractTraceType TraceType)
{
	Super::InteractEnter_Implementation(InteractComponent, TraceType);

	FText FailText;
	if (OutLineInteractTraceTypeInfoMap.Contains(TraceType))
	{
		if (OutLineInteractTraceTypeInfoMap[TraceType].Verify(InteractComponent, BeInteractComponent, FailText))
		{
			if (!CurEnterInteractTraceType.Contains(TraceType))
			{
				CurEnterInteractTraceType.Add(TraceType);
				ChangeOutLineCount(1);
			}
		}
		else//只是验证不通过而不是不允许时，会开启间隔检测
		{
			OutLineBeVerifyInfoChange(true, FIS_OutLineBeVerifyInfo(InteractComponent, TraceType));
		}

	}
}

void UIS_BIEInteractOutLine::InteractLeave_Implementation(UIS_InteractComponent* InteractComponent, EIS_InteractTraceType TraceType)
{
	Super::InteractLeave_Implementation(InteractComponent, TraceType);

	if (OutLineInteractTraceTypeInfoMap.Contains(TraceType))
	{
		if (CurEnterInteractTraceType.Contains(TraceType))
		{
			CurEnterInteractTraceType.Remove(TraceType);
			ChangeOutLineCount(-1);
		}
		OutLineBeVerifyInfoChange(false, FIS_OutLineBeVerifyInfo(InteractComponent, TraceType));
	}
}

bool UIS_BIEInteractOutLine::InteractLeaveIsEnd_Implementation()
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

void UIS_BIEInteractOutLine::ReplicatedUsing_OutLineInteractTraceTypeInfo()
{
	for (FIS_OutLineInteractTraceTypeInfo& Info : OutLineInteractTraceTypeInfo)
	{
		OutLineInteractTraceTypeInfoMap.Add(Info.InteractTraceType, Info.InteractVerifyInfo);
	}
}

void UIS_BIEInteractOutLine::OutLineCheckIntervalBack()
{
	for (FIS_OutLineBeVerifyInfo& VerifyInfo : AllOutLineBeVerifyInfo)
	{
		if (VerifyInfo.InteractComponent)
		{
			FText FailText;
			if (OutLineInteractTraceTypeInfoMap[VerifyInfo.InteractTraceType].Verify(VerifyInfo.InteractComponent, BeInteractComponent, FailText))
			{
				if (!CurEnterInteractTraceType.Contains(VerifyInfo.InteractTraceType))
				{
					CurEnterInteractTraceType.Add(VerifyInfo.InteractTraceType);
					ChangeOutLineCount(1);
				}
			}
			else
			{
				if (CurEnterInteractTraceType.Contains(VerifyInfo.InteractTraceType))
				{
					CurEnterInteractTraceType.Remove(VerifyInfo.InteractTraceType);
					ChangeOutLineCount(-1);
				}
			}
		}
	}
}

void UIS_BIEInteractOutLine::OutLineBeVerifyInfoChange(bool IsAdd, FIS_OutLineBeVerifyInfo VerifyInfo)
{
	if (IsAdd)
	{
		AllOutLineBeVerifyInfo.Add(VerifyInfo);
	}
	else
	{
		AllOutLineBeVerifyInfo.Remove(VerifyInfo);
	}

	if (AllOutLineBeVerifyInfo.Num() > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(OutLineCheckTimeHandle, this, &UIS_BIEInteractOutLine::OutLineCheckIntervalBack, OutLineCheckInterval, true);
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(OutLineCheckTimeHandle);
	}
}
