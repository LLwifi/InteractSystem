// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent/BeInteractExtend/IS_BIEGetComponent.h"
#include "ActorComponent/IS_BeInteractComponent.h"

void UIS_BIEGetComponent::Init_Implementation(UIS_BeInteractComponent* BeInteractCom, UIS_BeInteractExtendBase* Data)
{
	Super::Init_Implementation(BeInteractCom, Data);

	UIS_BIEGetComponent* DataCom = Cast<UIS_BIEGetComponent>(Data);
	if (DataCom)
	{
		GetComponentNum = DataCom->GetComponentNum;
		ComponentClass = DataCom->ComponentClass;
		Interface = DataCom->Interface;
		Tag = DataCom->Tag;
	}
}

bool UIS_BIEGetComponent::NumIsSufficient(int32 CurComponentNum)
{
	if (GetComponentNum > 0)
	{
		return CurComponentNum >= GetComponentNum;
	}
	return false;
}

TArray<UActorComponent*> UIS_BIEGetComponent::GetComponent()
{
	AllComponents.Empty();
	GetComponentFromComponentClass();
	if (NumIsSufficient(AllComponents.Num()))
	{
		return AllComponents;
	}

	GetComponentFromInterface();
    return AllComponents;
}

TArray<UActorComponent*> UIS_BIEGetComponent::GetComponentFromComponentClass()
{
	if (BeInteractComponent)
	{
		for (TSubclassOf<UActorComponent> ComClass : ComponentClass)
		{
			TArray<UActorComponent*> GetCom;
			BeInteractComponent->GetOwner()->GetComponents(ComClass, GetCom);
			ComponentConditionCheck(GetCom);
		}
	}
	return AllComponents;
}

TArray<UActorComponent*> UIS_BIEGetComponent::GetComponentFromInterface()
{
	if (BeInteractComponent)
	{
		for (TSubclassOf<UInterface> InterfaceClass : Interface)
		{
			TArray<UActorComponent*> GetCom;
			GetCom = BeInteractComponent->GetOwner()->GetComponentsByInterface(InterfaceClass);
			ComponentConditionCheck(GetCom);
		}
	}
	return AllComponents;
}

void UIS_BIEGetComponent::ComponentConditionCheck(TArray<UActorComponent*> Components)
{
	//TArray<UActorComponent*> ReturnComponents;
	for (UActorComponent*& Com : Components)
	{
		bool b = false;
		//Tag判断
		if (Tag.Num() > 0)
		{
			for (FName& OneTag : Tag)
			{
				if (Com->ComponentHasTag(OneTag))
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

		//Socket判断
		if (SocketName.Num() > 0)
		{
			if (b)//如果上面的判断没过，就没必要进行SocketName的判断了
			{
				b = false;//重置，默认认为没有通过
				USceneComponent* SceneCom = Cast<USceneComponent>(Com);
				if (SceneCom)
				{
					for (FName& OneSocketName : SocketName)
					{
						if (SceneCom->GetAttachSocketName() == OneSocketName)
						{
							b = true;
							break;
						}
					}
				}
			}
			
		}

		//上述判断均通过，添加组件
		if (b)
		{
			AllComponents.AddUnique(Com);
			if (NumIsSufficient(AllComponents.Num()))
			{
				return;
			}
		}
	}
	//return ReturnComponents;
}
