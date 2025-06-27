// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent/IS_BeInteractComponent.h"

// Sets default values for this component's properties
UIS_BeInteractComponent_Box::UIS_BeInteractComponent_Box()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

#if WITH_EDITOR

void UIS_BeInteractComponent_Box::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FProperty* Property = PropertyChangedEvent.Property;//拿到改变的属性
	if (BeInteractInfo.InteractType == EIS_InteractType::MultiSegment)//多段持续交互时
	{
		if (Property->GetFName() == "MultiInteractNum" || Property->GetFName() == "InteractTime" || Property->GetFName() == "InteractType")
		{
			BeInteractInfo.InteractTime.SetNum(BeInteractInfo.MultiInteractNum);
		}
	}
	else
	{
		BeInteractInfo.InteractTime.SetNum(1);
	}

}

#endif


// Called when the game starts
void UIS_BeInteractComponent_Box::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UIS_BeInteractComponent_Box::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

FText UIS_BeInteractComponent_Box::GetInteractText()
{
	return BeInteractInfo.InteractText;
}

FText UIS_BeInteractComponent_Box::GetInteractFailText()
{
	return BeInteractInfo.InteractFailText;
}

EIS_InteractType UIS_BeInteractComponent_Box::GetInteractType()
{
	return BeInteractInfo.InteractType;
}

int32 UIS_BeInteractComponent_Box::GetMultiInteractNum()
{
	return BeInteractInfo.MultiInteractNum;
}

TArray<float> UIS_BeInteractComponent_Box::GetInteractTime()
{
	return BeInteractInfo.InteractTime;
}

int32 UIS_BeInteractComponent_Box::GetInteractNum()
{
	return BeInteractInfo.InteractNum;
}

EIS_InteractNumSubtractType UIS_BeInteractComponent_Box::GetInteractNumSubtractType()
{
	return BeInteractInfo.InteractNumSubtractType;
}

FGameplayTagContainer UIS_BeInteractComponent_Box::GetInteractTag()
{
	return BeInteractInfo.InteractTag;
}

int32 UIS_BeInteractComponent_Box::GetInteractPriority()
{
	return BeInteractInfo.InteractPriority;
}

void UIS_BeInteractComponent_Box::InteractEnter(UIS_InteractComponent* EnterInteractCom)
{

}

void UIS_BeInteractComponent_Box::InteractLeave(UIS_InteractComponent* EnterInteractCom)
{

}

bool UIS_BeInteractComponent_Box::TryInteract(UIS_InteractComponent* EnterInteractCom)
{
	if (InteractCheck(EnterInteractCom))
	{
		InteractStart(EnterInteractCom);
		return true;
	}
	return false;
}

bool UIS_BeInteractComponent_Box::InteractCheck(UIS_InteractComponent* EnterInteractCom)
{
	return true;
}

void UIS_BeInteractComponent_Box::InteractStart(UIS_InteractComponent* EnterInteractCom)
{

}

void UIS_BeInteractComponent_Box::InteractEnd(UIS_InteractComponent* EnterInteractCom)
{

}