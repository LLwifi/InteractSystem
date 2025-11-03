// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent/BeInteractExtend/IS_BIEHideHoldItem.h"

void UIS_BIEHideHoldItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(UIS_BIEHideHoldItem, AllSceneComponent);
}

void UIS_BIEHideHoldItem::Init_Implementation(UIS_BeInteractComponent* BeInteractCom, UIS_BeInteractExtendBase* Data)
{
	Super::Init_Implementation(BeInteractCom, Data);

	for (UActorComponent*& ActorCom : GetComponent())
	{
		USceneComponent* SceneCom = Cast<USceneComponent>(ActorCom);
		if (SceneCom)
		{
			AllSceneComponent.Add(SceneCom);
		}
	}
}

void UIS_BIEHideHoldItem::InteractStart_Implementation(UIS_InteractComponent* InteractComponent)
{
	for (USceneComponent*& SceneCom : AllSceneComponent)
	{
		SceneCom->SetVisibility(false, true);
	}
}

void UIS_BIEHideHoldItem::InteractEnd_Implementation(UIS_InteractComponent* InteractComponent)
{
	for (USceneComponent*& SceneCom : AllSceneComponent)
	{
		SceneCom->SetVisibility(true, true);
	}
}
