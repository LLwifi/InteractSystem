// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent/BeInteractExtend/IS_BeInteractExtendBase.h"
#include <ActorComponent/IS_BeInteractComponent.h>
#include "GameFramework/Actor.h"

// Sets default values for this component's properties
UIS_BeInteractExtendBase::UIS_BeInteractExtendBase()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UIS_BeInteractExtendBase::BeginPlay()
{
	Super::BeginPlay();

	// ...
	for (FName& Tag : ComponentTags)
	{
		TArray<UActorComponent*> AllActorCom = GetOwner()->GetComponentsByTag(UIS_BeInteractComponent::StaticClass(), Tag);
		for (UActorComponent*& ActorCom : AllActorCom)
		{
			UIS_BeInteractComponent* BeInteractCom = Cast<UIS_BeInteractComponent>(ActorCom);
			if (BeInteractCom)
			{
				BeInteractCom->AllExtendComponent.Add(this);
				FScriptDelegate ScriptDelegate;
				ScriptDelegate.BindUFunction(this,"LinkInteractEnter");
				BeInteractCom->OnInteractEnter.Add(ScriptDelegate);
				ScriptDelegate.BindUFunction(this, "LinkInteractEnd");
				BeInteractCom->OnInteractEnd.Add(ScriptDelegate);
				ScriptDelegate.BindUFunction(this, "LinkInteractComplete");
				BeInteractCom->OnInteractComplete.Add(ScriptDelegate);
			}
		}
	}
}


// Called every frame
void UIS_BeInteractExtendBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UIS_BeInteractExtendBase::LinkInteractEnter_Implementation(UIS_InteractComponent* InteractComponent, EIS_InteractTraceType TraceType)
{
	
}

void UIS_BeInteractExtendBase::LinkInteractEnd_Implementation(UIS_InteractComponent* InteractComponent)
{
}

void UIS_BeInteractExtendBase::LinkInteractComplete_Implementation(UIS_InteractComponent* InteractComponent)
{

}

