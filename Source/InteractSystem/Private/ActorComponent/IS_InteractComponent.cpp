// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent/IS_InteractComponent.h"
#include <Kismet/GameplayStatics.h>

// Sets default values for this component's properties
UIS_InteractComponent::UIS_InteractComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

#if WITH_EDITOR
void UIS_InteractComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	auto Property = PropertyChangedEvent.Property;//拿到改变的属性
	//if (Property->GetFName() == GET_MEMBER_NAME_CHECKED(UTS_TaskComponent, TaskRole))
	//{
	//	if (TaskRole == ETS_TaskRole::Player)
	//	{
	//		SetIsReplicated(true);
	//	}
	//	else
	//	{
	//		SetIsReplicated(false);
	//	}
	//}
}
#endif

// Called when the game starts
void UIS_InteractComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UIS_InteractComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(UTS_TaskComponent, AllTask);
}

// Called every frame
void UIS_InteractComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}