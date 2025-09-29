// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorComponent/BeInteractExtend/IS_BeInteractExtendBase.h"
#include "IS_BIEInteractHideHoldItem.generated.h"

/**可被交互物的功能扩展——隐藏持有物
 *
 */
UCLASS()
class INTERACTSYSTEM_API UIS_BIEInteractHideHoldItem : public UIS_BeInteractExtendBase
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	virtual void InteractStart_Implementation(UIS_InteractComponent* InteractComponent) override;
	virtual void InteractEnd_Implementation(UIS_InteractComponent* InteractComponent) override;

	UFUNCTION(NetMulticast, Reliable)
	void NetMulti_OnInteractStart(UIS_InteractComponent* InteractComponent);
	UFUNCTION(NetMulticast, Reliable)
	void NetMulti_OnInteractEnd(UIS_InteractComponent* InteractComponent);

public:
	
};
