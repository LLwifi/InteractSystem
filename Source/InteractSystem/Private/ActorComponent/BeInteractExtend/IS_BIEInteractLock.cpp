// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent/BeInteractExtend/IS_BIEInteractLock.h"
#include "ActorComponent/IS_InteractComponent.h"

void UIS_BIEInteractLock::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UIS_BIEInteractLock, LockTimeConfig);
	DOREPLIFETIME(UIS_BIEInteractLock, LockFailText);
	DOREPLIFETIME(UIS_BIEInteractLock, RealTimeRefresh);
	DOREPLIFETIME(UIS_BIEInteractLock, DelayRefreshTime);
	DOREPLIFETIME(UIS_BIEInteractLock, AllLockInfo);

}

void UIS_BIEInteractLock::Init_Implementation(UIS_BeInteractComponent* BeInteractCom, UIS_BeInteractExtendBase* Data)
{
	Super::Init_Implementation(BeInteractCom, Data);

	UIS_BIEInteractLock* BIEInteractLock = Cast<UIS_BIEInteractLock>(Data);
	if (BIEInteractLock)
	{
		LockTimeConfig = BIEInteractLock->LockTimeConfig;
		LockFailText = BIEInteractLock->LockFailText;
		RealTimeRefresh = BIEInteractLock->RealTimeRefresh;
		DelayRefreshTime = BIEInteractLock->DelayRefreshTime;
	}

	if (!RealTimeRefresh)
	{
		if (DelayRefreshTime > 0.0f)
		{
			GetWorld()->GetTimerManager().SetTimer(DelayRefreshTimeHandle, this, &UIS_BIEInteractLock::DelayRefreshTimeBack, DelayRefreshTime);
		}
		else
		{
			DelayRefreshTimeBack();
		}
	}
}

bool UIS_BIEInteractLock::CanInteract_Implementation(UIS_InteractComponent* InteractComponent, FCC_CompareInfo OuterCompareInfo, FText& FailText)
{
	Super::CanInteract_Implementation(InteractComponent, OuterCompareInfo, FailText);

	if (InteractComponent && InteractComponent->GetOwner())
	{
		FailText = LockFailText;
		FIS_BIELockInfo LockInfo;
		return !ActorLockCheck(InteractComponent->GetOwner(), LockInfo);
	}
	return true;
}

void UIS_BIEInteractLock::ReSetLock(bool IsOverrideLockTime, float OverrideLockTime)
{
	if (RealTimeRefresh)
	{
		GetActor();
	}

	for (AActor*& OneActor : All)
	{
		if (OneActor)
		{
			AddLock(OneActor, IsOverrideLockTime? OverrideLockTime: LockTimeConfig);
		}
	}
}

void UIS_BIEInteractLock::LockBack()
{
	TArray<FIS_BIELockInfo> LockInfos = AllLockInfo;
	for (FIS_BIELockInfo& LockInfo : LockInfos)
	{
		float TimerHandleTime = UKismetSystemLibrary::K2_GetTimerElapsedTimeHandle(this, LockInfo.LockTimerHandle);//获取计时器已用时间
		if (TimerHandleTime >= LockInfo.LockTime || TimerHandleTime <= 0.0f)//<0表示已经结束了
		{
			AllLockInfo.Remove(LockInfo);
		}
	}
}

void UIS_BIEInteractLock::AddLock(AActor* LockTarget, float LockTime)
{
	FTimerHandle TimeHandle;
	GetWorld()->GetTimerManager().SetTimer(TimeHandle, this, &UIS_BIEInteractLock::LockBack, LockTime);
	FIS_BIELockInfo LockInfo = FIS_BIELockInfo(LockTarget, LockTime, TimeHandle);
	if (AllLockInfo.Contains(LockInfo))
	{
		AllLockInfo.Remove(LockInfo);
	}
	AllLockInfo.Add(LockInfo);
}

bool UIS_BIEInteractLock::ActorLockCheck(AActor* Target, FIS_BIELockInfo& LockInfo)
{
	if (Target)
	{
		for (FIS_BIELockInfo& BIELockInfo : AllLockInfo)
		{
			if (BIELockInfo.LockTarget == Target)
			{
				float TimerHandleTime = UKismetSystemLibrary::K2_GetTimerElapsedTimeHandle(this, BIELockInfo.LockTimerHandle);//获取计时器已用时间
				LockInfo = BIELockInfo;
				return (TimerHandleTime < LockInfo.LockTime || TimerHandleTime > 0.0f);
			}
		}
	}
	return false;//没锁定
}

void UIS_BIEInteractLock::DelayRefreshTimeBack()
{
	GetActor();
	ReSetLock();
}
