// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent/BeInteractExtend/IS_BIEInteractHideHoldItem.h"

void UIS_BIEInteractHideHoldItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(UIS_BIEInteractHideHoldItem, FindOutLineMeshComponnetTag);
}

void UIS_BIEInteractHideHoldItem::InteractStart_Implementation(UIS_InteractComponent* InteractComponent)
{
	Super::InteractStart_Implementation(InteractComponent);
	NetMulti_OnInteractStart(InteractComponent);
}

void UIS_BIEInteractHideHoldItem::InteractEnd_Implementation(UIS_InteractComponent* InteractComponent)
{
	Super::InteractEnd_Implementation(InteractComponent);
	NetMulti_OnInteractEnd(InteractComponent);
}

void UIS_BIEInteractHideHoldItem::NetMulti_OnInteractStart_Implementation(UIS_InteractComponent* InteractComponent)
{

}

void UIS_BIEInteractHideHoldItem::NetMulti_OnInteractEnd_Implementation(UIS_InteractComponent* InteractComponent)
{

}
