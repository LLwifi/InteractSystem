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
			for (UActorComponent*& Com : GetCom)
			{
				if (Tag.Num() > 0)
				{
					for (FName& OneTag : Tag)
					{
						if (Com->ComponentHasTag(OneTag))
						{
							AllComponents.AddUnique(Com);
							if (NumIsSufficient(AllComponents.Num()))
							{
								return AllComponents;
							}
							break;
						}
					}
				}
				else
				{
					AllComponents.AddUnique(Com);
					if (NumIsSufficient(AllComponents.Num()))
					{
						return AllComponents;
					}
				}

			}
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
			for (UActorComponent*& Com : GetCom)
			{
				if (Tag.Num() > 0)
				{
					for (FName& OneTag : Tag)
					{
						if (Com->ComponentHasTag(OneTag))
						{
							AllComponents.AddUnique(Com);
							if (NumIsSufficient(AllComponents.Num()))
							{
								return AllComponents;
							}
							break;
						}
					}
				}
				else
				{
					AllComponents.AddUnique(Com);
					if (NumIsSufficient(AllComponents.Num()))
					{
						return AllComponents;
					}
				}

			}
		}
	}
	return AllComponents;
}
