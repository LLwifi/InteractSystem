// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent/BeInteractExtend/IS_BIEGetActor.h"
#include "Kismet/GameplayStatics.h"
#include "ActorComponent/IS_BeInteractComponent.h"

void UIS_BIEGetActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UIS_BIEGetActor, GetNum);
	DOREPLIFETIME(UIS_BIEGetActor, GetClass);
	DOREPLIFETIME(UIS_BIEGetActor, GetInterface);
	DOREPLIFETIME(UIS_BIEGetActor, GetTag);
	DOREPLIFETIME(UIS_BIEGetActor, All);
	DOREPLIFETIME(UIS_BIEGetActor, Ignore);
}

void UIS_BIEGetActor::Init_Implementation(UIS_BeInteractComponent* BeInteractCom, UIS_BeInteractExtendBase* Data)
{
	Super::Init_Implementation(BeInteractCom, Data);

	UIS_BIEGetActor* BIEGetActor = Cast<UIS_BIEGetActor>(Data);
	if (BIEGetActor)
	{
		GetNum = BIEGetActor->GetNum;
		GetClass = BIEGetActor->GetClass;
		GetInterface = BIEGetActor->GetInterface;
		GetTag = BIEGetActor->GetTag;
	}
}

bool UIS_BIEGetActor::NumIsSufficient(int32 CurNum)
{
	if (GetNum > 0)
	{
		return CurNum >= GetNum;
	}
	return false;
}

TArray<AActor*> UIS_BIEGetActor::GetActor()
{
	All.Empty();
	GetActorFromClass();
	if (NumIsSufficient(All.Num()))
	{
		return All;
	}
	GetActorFromInterface();
	return All;
}

TArray<AActor*> UIS_BIEGetActor::GetActorFromClass()
{
	TArray<AActor*> Actors;
	for (TSubclassOf<AActor>& ActorClass : GetClass)
	{
		UGameplayStatics::GetAllActorsOfClass(BeInteractComponent, ActorClass, Actors);
		ActorConditionCheck(Actors);
	}
	return All;
}

TArray<AActor*> UIS_BIEGetActor::GetActorFromInterface()
{
	TArray<AActor*> Actors;
	for (TSubclassOf<UInterface>& InterfaceClass : GetInterface)
	{
		UGameplayStatics::GetAllActorsWithInterface(BeInteractComponent, InterfaceClass, Actors);
		ActorConditionCheck(Actors);
	}
	return All;
}

void UIS_BIEGetActor::ActorConditionCheck(TArray<AActor*> Actors)
{
	for (AActor*& One : Actors)
	{
		if (!Ignore.Contains(One))
		{
			bool b = false;
			//Tag判断
			if (GetTag.Num() > 0)
			{
				for (FName& OneTag : GetTag)
				{
					if (One->ActorHasTag(OneTag))
					{
						b = true;
						break;
					}
				}
			}
			else
			{
				b = true;
			}

			//上述判断均通过，添加组件
			if (b)
			{
				All.AddUnique(One);
				if (NumIsSufficient(All.Num()))
				{
					return;
				}
			}
		}
	}
}

void UIS_BIEGetActor::AddIgnore(AActor* Actor)
{
	Ignore.Add(Actor);
}
