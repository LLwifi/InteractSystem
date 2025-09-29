// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent/BeInteractExtend/IS_BIEInteractCD.h"

//void UIS_BIEInteractCD::LinkInteractEnter_Implementation(UIS_InteractComponent* InteractComponent, EIS_InteractTraceType TraceType)
//{
//}
//
//void UIS_BIEInteractCD::LinkInteractEnd_Implementation(UIS_InteractComponent* InteractComponent)
//{
//	if (Config.InteractNumSubtractType == EIS_InteractNumSubtractType::End)
//	{
//		InteractNum++;
//		if (InteractNum >= Config.EnterCDInteractNum)
//		{
//			InteractNum = 0;
//			EnterCD();
//		}
//	}
//}
//
//void UIS_BIEInteractCD::LinkInteractComplete_Implementation(UIS_InteractComponent* InteractComponent)
//{
//	if (Config.InteractNumSubtractType == EIS_InteractNumSubtractType::Complete)
//	{
//		InteractNum++;
//	}
//}

void UIS_BIEInteractCD::EnterCD()
{
	GetWorld()->GetTimerManager().SetTimer(CDTimerHandle, this, &UIS_BIEInteractCD::CDTimerBack, Config.CDTime);
	bIsCD = true;
}

void UIS_BIEInteractCD::CDTimerBack()
{
	
}
